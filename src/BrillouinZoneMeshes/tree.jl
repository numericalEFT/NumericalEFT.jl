module GridTree

using ..AbstractTrees
using ..StaticArrays
using ..BaseMesh
using ..Statistics
using ..LinearAlgebra
using ..BaryCheb

export GridNode, TreeGrid, uniformtreegrid, treegridfromdensity, efficiency, SymMap, interp, MappedData

struct GridNode{DIM}
    index::Vector{Int} # index in treegrid.subgrids, 0 if has children
    depth::Int
    pos::SVector{DIM,Int64}
    children::Vector{GridNode{DIM}}
end

function GridNode{DIM}(
    isfine;
    depth=0, pos=SVector{DIM,Int64}(zeros(Int64, DIM)), maxdepth=10, mindepth=0) where {DIM}
    if (isfine(depth, pos) && depth >= mindepth) || depth >= maxdepth
        return GridNode{DIM}([0,], depth, pos, Vector{GridNode{DIM}}([]))
    else
        children = Vector{GridNode{DIM}}([])
        for i in 0:2^DIM-1
            bin = digits(i, base=2, pad=DIM) |> reverse
            childdepth = depth + 1
            childpos = pos .* 2 .+ bin
            push!(children, GridNode{DIM}(isfine; depth=childdepth, pos=childpos, maxdepth=maxdepth, mindepth=mindepth))
        end
        return GridNode{DIM}([0,], depth, pos, children)
    end
end

AbstractTrees.children(node::GridNode) = node.children

function Base.floor(node::GridNode{DIM}, x) where {DIM}
    # x is dimensionless pos that matches pos ./ 2^depth
    if isempty(node.children)
        return node.index[1]
    else
        midpos = (node.pos .* 2.0 .+ 1.0) ./ 2^(node.depth + 1)

        index = 1
        for i in 1:DIM
            if x[i] > midpos[i]
                index = index + 2^(DIM - i)
            end
        end

        return floor(node.children[index], x)
    end
end

function efficiency(root::GridNode{DIM}) where {DIM}
    np, depth = 0, 0
    for node in PostOrderDFS(root)
        if node.depth > depth
            depth = node.depth
        end
        if isempty(node.children)
            np = np + 1
        end
    end
    return np / 2^(depth * DIM)
end

struct TreeGrid{DIM,SG} <: AbstractMesh{DIM}
    origin::SVector{DIM,Float64}
    root::GridNode{DIM}
    latvec::SMatrix{DIM,DIM,Float64}
    invlatvec::SMatrix{DIM,DIM,Float64}
    subgrids::Vector{SG}
end

efficiency(tg::TreeGrid) = efficiency(tg.root)

function Base.floor(tg::TreeGrid{DIM,SG}, x) where {DIM,SG}
    dimlessx = tg.invlatvec * SVector{DIM,Float64}(x) .+ 0.5

    tgi = floor(tg.root, dimlessx)
    mesh = tg.subgrids[tgi]

    sgi = floor(mesh, x)

    sgsize = length(tg.subgrids[1])

    return (tgi - 1) * sgsize + sgi
end

function BaseMesh.locate(tg::TreeGrid{DIM,SG}, x) where {DIM,SG}
    dimlessx = tg.invlatvec * SVector{DIM,Float64}(x) .+ 0.5

    tgi = floor(tg.root, dimlessx)
    mesh = tg.subgrids[tgi]

    sgi = locate(mesh, x)

    sgsize = length(tg.subgrids[1])

    return (tgi - 1) * sgsize + sgi
end

BaseMesh.volume(tg::TreeGrid{DIM,SG}) where {DIM,SG} = _calc_area(tg.latvec)
function BaseMesh.volume(tg::TreeGrid{DIM,SG}, i) where {DIM,SG}
    sgsize = length(tg.subgrids[1])

    tgi, sgi = (i - 1) ÷ sgsize + 1, (i - 1) % sgsize + 1

    return volume(tg.subgrids[tgi], sgi)
end

function interp(data, tg::TreeGrid{DIM,SG}, x) where {DIM,SG}
    dimlessx = tg.invlatvec * SVector{DIM,Float64}(x) .+ 0.5
    sgsize = length(tg.subgrids[1])

    tgi = floor(tg.root, dimlessx)
    mesh = tg.subgrids[tgi]

    data_slice = view(data, (tgi-1)*sgsize+1:tgi*sgsize)

    return BaseMesh.interp(data_slice, mesh, x)
end

function integrate(data, tg::TreeGrid{DIM,SG}) where {DIM,SG}
    sgsize = length(tg.subgrids[1])
    result = 0.0

    for tgi in 1:size(tg)[1]
        mesh = tg.subgrids[tgi]
        data_slice = view(data, (tgi-1)*sgsize+1:tgi*sgsize)
        result += BaseMesh.integrate(data_slice, mesh)
    end
    return result
end

function _calc_area(latvec)
    return abs(det(latvec))
end

function _calc_point(depth, pos, latvec)
    DIM = length(pos)
    ratio = pos ./ 2^depth .- 0.5
    origin = zeros(size(ratio))

    for i in 1:DIM
        origin .+= ratio[i] .* latvec[:, i]
    end

    return origin
end

function _calc_origin(node::GridNode{DIM}, latvec) where {DIM}
    return _calc_point(node.depth, node.pos, latvec)
end

function _calc_subpoints(depth, pos, latvec, N)
    DIM = length(pos)
    points = []
    for i in 0:N^DIM-1
        ii = digits(i, base=N, pad=DIM)
        push!(points, _calc_point(depth, pos .+ ii / (N - 1), latvec))
    end
    return points
end

function _calc_cornerpoints(depth, pos, latvec)
    # DIM = length(pos)
    # points = []
    # for i in 0:2^DIM-1
    #     ii = digits(i, base = 2, pad = DIM)
    #     push!(points, _calc_point(depth, pos .+ ii, latvec))
    # end
    # return points
    return _calc_subpoints(depth, pos, latvec, 2)
end

function uniformtreegrid(isfine, latvec; maxdepth=10, mindepth=0, DIM=2, N=2)
    root = GridNode{DIM}(isfine; maxdepth=maxdepth, mindepth=mindepth)
    subgrids = Vector{UniformMesh{DIM,N}}([])

    i = 1
    for node in PostOrderDFS(root)
        if isempty(node.children)
            depth = node.depth
            origin = _calc_origin(node, latvec)
            mesh = UniformMesh{DIM,N}(origin, latvec ./ 2^depth)
            push!(subgrids, mesh)
            node.index[1] = i
            i = i + 1
        end
    end
    origin = _calc_origin(root, latvec)
    return TreeGrid{DIM,UniformMesh{DIM,N}}(origin, root, latvec, inv(latvec), subgrids)
end

function barychebtreegrid(isfine, latvec; maxdepth=10, mindepth=0, DIM=2, N=2)
    root = GridNode{DIM}(isfine; maxdepth=maxdepth, mindepth=mindepth)
    subgrids = Vector{BaryChebMesh{DIM,N}}([])

    i = 1
    barycheb = BaryCheb1D(N)
    for node in PostOrderDFS(root)
        if isempty(node.children)
            depth = node.depth
            origin = _calc_origin(node, latvec)
            mesh = BaryChebMesh{DIM,N}(origin, latvec ./ 2^depth, barycheb)
            push!(subgrids, mesh)
            node.index[1] = i
            i = i + 1
        end
    end

    origin = _calc_origin(root, latvec)
    return TreeGrid{DIM,BaryChebMesh{DIM,N}}(origin, root, latvec, inv(latvec), subgrids)
end

Base.length(tg::TreeGrid{DIM,SG}) where {DIM,SG} = length(tg.subgrids) * length(tg.subgrids[1])
Base.size(tg::TreeGrid{DIM,SG}) where {DIM,SG} = (length(tg.subgrids), length(tg.subgrids[1]))
# index and iterator
function Base.getindex(tg::TreeGrid{DIM,SG}, i) where {DIM,SG}
    sgsize = length(tg.subgrids[1])

    tgi, sgi = (i - 1) ÷ sgsize + 1, (i - 1) % sgsize + 1

    return getindex(tg.subgrids[tgi], sgi)
end
function Base.getindex(tg::TreeGrid{DIM,SG}, i, j) where {DIM,SG}
    return getindex(tg.subgrids[i], j)
end
Base.firstindex(tg::TreeGrid) = 1
Base.lastindex(tg::TreeGrid) = length(tg)

Base.iterate(tg::TreeGrid) = (tg[1], 1)
Base.iterate(tg::TreeGrid, state) = (state >= length(tg)) ? nothing : (tg[state+1], state + 1)

function densityisfine(density, latvec, depth, pos, atol, DIM; N=3)
    # compare results from subgrid of N+1 and N+3
    # area = _calc_area(latvec) / 2^(depth*DIM)
    area = 1.0 / 2^(depth * DIM)
    # cornerpoints1 = _calc_subpoints(depth, pos, latvec, N)
    cornerpoints2 = _calc_subpoints(depth, pos, latvec, 10)
    # val1 = [density(p) for p in cornerpoints1]
    val2 = [density(p) for p in cornerpoints2]
    # return abs(sum(val1) / length(val1) - sum(val2) / length(val2)) * area < atol
    # return abs(sum(val2) / length(val2)) * area < atol
    # println("max:$(abs(maximum(val2)))")
    # println("area:$(area)")
    # println("atol:$(atol)")
    return abs(maximum(val2) - minimum(val2)) * area < atol
    # return std(val2) * area < atol
end

function barychebdensityisfine(density, latvec, depth, pos, atol, DIM; N=4)
    area = _calc_area(latvec) / 2^(depth * DIM)

    origin = _calc_point(depth, pos, latvec)
    mesh1 = BaryChebMesh(origin, latvec ./ 2^depth, DIM, N)
    mesh2 = BaryChebMesh(origin, latvec ./ 2^depth, DIM, N + 2)

    data1 = [density(p) for p in mesh1]
    # data2 = [density(p) for p in mesh2]

    # return abs(BaseMesh.integrate(data1, mesh1) - BaseMesh.integrate(data2, mesh2)) < atol
    diff = [abs(density(p) - BaseMesh.interp(data1, mesh1, p)) for p in mesh2]
    return sum(diff) / length(diff) * area < atol
end

function treegridfromdensity(density, latvec; atol=1e-4, maxdepth=10, mindepth=0, DIM=2, N=2, type=:uniform)
    isfine(depth, pos) = barychebdensityisfine(density, latvec, depth, pos, atol, DIM)
    # isfine(depth, pos) = densityisfine(density, latvec, depth, pos, atol, DIM; N=N)
    if type == :uniform
        return uniformtreegrid(isfine, latvec; maxdepth=maxdepth, mindepth=mindepth, DIM=DIM, N=N)
    elseif type == :barycheb
        return barychebtreegrid(isfine, latvec; maxdepth=maxdepth, mindepth=mindepth, DIM=DIM, N=N)
    else
        error("not implemented!")
    end
end

function _find_in(x, arr::AbstractArray; atol=1e-6, rtol=1e-6)
    # return index if in, return 0 otherwise
    for yi in 1:length(arr)
        y = arr[yi]
        if isapprox(x, y, atol=atol, rtol=rtol)
            return yi
        end
    end

    return 0
end

struct SymMap{T,N}
    map::Vector{Int}
    reduced_length::Int
    _vals::Vector{T}
    inv_map::Vector{Vector{Int}}

    function SymMap{T}(tg::TreeGrid, density; atol=1e-6, rtol=1e-6) where {T}
        map = zeros(Int, length(tg))
        reduced_vals = []
        inv_map = []
        for pi in 1:length(tg)
            # println(pi, " ", p)
            p = tg[pi]
            val = density(p)
            # println(val)
            pos = _find_in(val, reduced_vals; atol=atol, rtol=rtol)
            if pos == 0
                push!(reduced_vals, val)
                push!(inv_map, [pi,])
                map[pi] = length(reduced_vals)
            else
                push!(inv_map[pos], pi)
                map[pi] = pos
            end
        end

        return new{T,length(tg)}(map, length(reduced_vals), reduced_vals, inv_map)
    end
end

struct MappedData{T,N} <: AbstractArray{T,N}
    smap::SymMap{T,N}
    data::Vector{T}

    function MappedData(smap::SymMap{T,N}) where {T,N}
        data = zeros(T, smap.reduced_length)
        return new{T,N}(smap, data)
    end
end

Base.length(md::MappedData) = length(md.smap.map)
Base.size(md::MappedData) = (length(md),)
# index and iterator
Base.getindex(md::MappedData, i) = md.data[md.smap.map[i]]
function Base.setindex!(md::MappedData, x, i)
    md.data[md.smap.map[i]] = x
end
Base.firstindex(md::MappedData) = 1
Base.lastindex(md::MappedData) = length(tg)

Base.iterate(md::MappedData) = (md[1], 1)
Base.iterate(md::MappedData, state) = (state >= length(md)) ? nothing : (md[state+1], state + 1)

end

