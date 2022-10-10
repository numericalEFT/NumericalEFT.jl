module BaseMesh

using ..StaticArrays
using ..LinearAlgebra

using ..BaryCheb

export UniformMesh, BaryChebMesh, CenteredMesh, EdgedMesh, AbstractMesh, locate, volume

abstract type AbstractMesh{N} <: AbstractArray{Float64,N} end
locate(mesh::AbstractMesh, x) = error("implement for concrete type required!")
volume(mesh::AbstractMesh, i) = error("implement for concrete type required!")

abstract type MeshType end
struct CenteredMesh <: MeshType end
struct EdgedMesh <: MeshType end

struct UniformMesh{DIM,N,MT} <: AbstractMesh{DIM}
    origin::SVector{DIM,Float64}
    latvec::SMatrix{DIM,DIM,Float64}
    invlatvec::SMatrix{DIM,DIM,Float64}

    function UniformMesh{DIM,N,MT}(origin, latvec) where {DIM,N,MT<:MeshType}
        return new{DIM,N,MT}(origin, latvec, inv(latvec))
    end
end

function UniformMesh{DIM,N}(origin, latvec) where {DIM,N}
    # Centered Mesh is the default
    return UniformMesh{DIM,N,CenteredMesh}(origin, latvec)
end

Base.length(mesh::UniformMesh{DIM,N}) where {DIM,N} = N^DIM
Base.size(mesh::UniformMesh{DIM,N}) where {DIM,N} = NTuple{DIM,Int}(ones(Int, DIM) .* N)

function Base.show(io::IO, mesh::UniformMesh)
    println("UniformMesh Grid:")
    for (pi, p) in enumerate(mesh)
        println(p)
    end
end

# Base.view(mesh::UniformMesh, i::Int) = Base.view(mesh.mesh, :, i)

# # set is not allowed for meshs
meshshift(::Type) = error("not implement!")
meshshift(::Type{<:CenteredMesh}) = 0.5
meshshift(::Type{<:EdgedMesh}) = 0.0

function Base.getindex(mesh::UniformMesh{DIM,N,MT}, inds...) where {DIM,N,MT}
    pos = Vector(mesh.origin)
    for (ni, n) in enumerate(inds)
        pos = pos .+ mesh.latvec[:, ni] .* (n - 1 + meshshift(MT)) ./ (N)
    end
    return pos
end

function Base.getindex(mesh::UniformMesh{DIM,N,MT}, i::Int) where {DIM,N,MT}
    return Base.getindex(mesh, _ind2inds(i, N, DIM)...)
end
Base.firstindex(mesh::UniformMesh) = 1
Base.lastindex(mesh::UniformMesh) = length(mesh)
# # iterator
Base.iterate(mesh::UniformMesh) = (mesh[1], 1)
Base.iterate(mesh::UniformMesh, state) = (state >= length(mesh)) ? nothing : (mesh[state+1], state + 1)

_ind2inds(i::Int, N::Int, DIM::Int) = digits(i - 1, base=N, pad=DIM) .+ 1

function _inds2ind(inds, N::Int)
    indexall = 1
    for i in 1:length(inds)
        indexall += (inds[i] - 1) * N^(i - 1)
    end
    return indexall
end

function _indfloor(x, N; edgeshift = 1)
    # edgeshift = 1 by default in floor function so that end point return N-1
    # edgeshift = 0 in locate function
    if x < 1
        return 1
    elseif x >= N
        return N - edgeshift
    else
        return floor(Int, x)
    end
end

function Base.floor(mesh::UniformMesh{DIM,N}, x) where {DIM,N}
    # find index of nearest grid point to the point
    displacement = SVector{DIM,Float64}(x) - mesh.origin
    # println(displacement)
    inds = (mesh.invlatvec * displacement) .* (N) .+ 0.5 .+ 2 * eps(1.0 * N)
    indexall = 1
    # println((mesh.invlatvec * displacement))
    # println(inds)
    for i in 1:DIM
        indexall += (_indfloor(inds[i], N) - 1) * N^(i - 1)
    end

    return indexall
end

function locate(mesh::UniformMesh{DIM, N, MT}, x) where {DIM,N,MT}
    # find index of nearest grid point to the point
    displacement = SVector{DIM,Float64}(x) - mesh.origin
    # println(displacement)
    inds = (mesh.invlatvec * displacement) .* (N) .+ 1.5 .- meshshift(MT) .+ 2 * eps(1.0 * N)
    indexall = 1
    # println((mesh.invlatvec * displacement))
    # println(inds)
    for i in 1:DIM
        indexall += (_indfloor(inds[i], N; edgeshift=0) - 1) * N^(i - 1)
    end
    return indexall
end

volume(mesh::UniformMesh) = abs(det(mesh.latvec))
function volume(mesh::UniformMesh{DIM,N,MT}, i) where {DIM,N,MT}
    volume(MT, mesh, i)
end

function volume(::Type{<:CenteredMesh}, mesh, i)
    # for uniform centered mesh, all mesh points share the same volume
    return abs(det(mesh.latvec)) / length(mesh)
end

function volume(::Type{<:EdgedMesh}, mesh::UniformMesh{DIM,N,MT}, i) where {DIM,N,MT}
    inds = _ind2inds(i, N, DIM)
    n1, nend = count(i -> i == 1, inds), count(i -> i == N, inds)
    cellarea = 2^(DIM - n1 - nend) * 3^nend / 2^DIM
    return cellarea / length(mesh) * volume(mesh)
end

function interp(data, mesh::UniformMesh{DIM,N}, x) where {DIM,N}
    error("Not implemented!")
end

function interp(data, mesh::UniformMesh{2,N,MT}, x) where {N,MT}
    # find floor index and normalized x y
    displacement = SVector{2,Float64}(x) - mesh.origin
    xy = (mesh.invlatvec * displacement) .* N .+ (1 - meshshift(MT)) .* (1 + 2 * eps(N * 1.0))
    xi, yi = _indfloor(xy[1], N), _indfloor(xy[2], N)

    return linear2D(data, xi, yi, xy..., N)
end

@inline function linear2D(data, xi, yi, x, y, N)
    # accept data, floored index, normalized x and y, return linear interp
    # (xi, yi) should be [(1, 1) - size(data)], x and y normalized to the same scale as xi and yi
    xd, yd = x - xi, y - yi

    c0 = data[xi+(yi-1)*N] * (1 - xd) + data[xi+1+(yi-1)*N] * xd
    c1 = data[xi+(yi)*N] * (1 - xd) + data[xi+1+(yi)*N] * xd

    return c0 * (1 - yd) + c1 * yd
end

function interp(data, mesh::UniformMesh{3,N,MT}, x) where {T,N,MT}
    # find floor index and normalized x y z
    displacement = SVector{3,Float64}(x) - mesh.origin
    xyz = (mesh.invlatvec * displacement) .* N .+ (1 - meshshift(MT)) .* (1 + 4 * eps(N * 1.0))
    xi, yi, zi = _indfloor(xyz[1], N), _indfloor(xyz[2], N), _indfloor(xyz[3], N)

    return linear3D(data, xi, yi, zi, xyz..., N)
end

@inline function linear3D(data, xi, yi, zi, x, y, z, N) where {T}
    xd, yd, zd = x - xi, y - yi, z - zi

    c00 = data[xi+(yi-1)*N+(zi-1)*N^2] * (1 - xd) + data[xi+(yi-1)*N+(zi-1)*N^2] * xd
    c01 = data[xi+(yi-1)*N+(zi)*N^2] * (1 - xd) + data[xi+(yi-1)*N+(zi)*N^2] * xd
    c10 = data[xi+(yi)*N+(zi-1)*N^2] * (1 - xd) + data[xi+(yi)*N+(zi-1)*N^2] * xd
    c11 = data[xi+(yi)*N+(zi)*N^2] * (1 - xd) + data[xi+(yi)*N+(zi)*N^2] * xd

    c0 = c00 * (1 - yd) + c10 * yd
    c1 = c01 * (1 - yd) + c11 * yd

    return c0 * (1 - zd) + c1 * zd
end

function integrate(data, mesh::UniformMesh{DIM,N,CenteredMesh}) where {DIM,N}
    area = abs(det(mesh.latvec))
    avg = sum(data) / length(data)
    return avg * area
end

function integrate(data, mesh::UniformMesh{DIM,N,EdgedMesh}) where {DIM,N}
    area = abs(det(mesh.latvec))
    avg = 0.0
    for i in 1:length(data)
        inds = _ind2inds(i, N, DIM)
        n1, nend = count(i -> i == 1, inds), count(i -> i == N, inds)
        avg += 2^(DIM - n1 - nend) * 3^nend / 2^DIM * data[i]
    end
    avg = avg / length(data)
    return avg * area
end

struct BaryChebMesh{DIM,N} <: AbstractMesh{DIM}
    origin::SVector{DIM,Float64}
    latvec::SMatrix{DIM,DIM,Float64}
    invlatvec::SMatrix{DIM,DIM,Float64}
    barycheb::BaryCheb1D{N} # grid in barycheb is [-1, 1]

    function BaryChebMesh{DIM,N}(origin, latvec, barycheb::BaryCheb1D{N}) where {DIM,N}
        return new{DIM,N}(origin, latvec, inv(latvec), barycheb)
    end
end

function BaryChebMesh(origin, latvec, DIM, N)
    barycheb = BaryCheb1D(N)
    return BaryChebMesh{DIM,N}(origin, latvec, barycheb)
end

Base.length(mesh::BaryChebMesh{DIM,N}) where {DIM,N} = N^DIM
Base.size(mesh::BaryChebMesh{DIM,N}) where {DIM,N} = NTuple{DIM,Int}(ones(Int, DIM) .* N)
function Base.show(io::IO, mesh::BaryChebMesh)
    println("BaryChebMesh Grid:")
    for (pi, p) in enumerate(mesh)
        println(p)
    end
end
function Base.getindex(mesh::BaryChebMesh{DIM,N}, inds...) where {DIM,N}
    pos = Vector(mesh.origin)
    for (ni, n) in enumerate(inds)
        pos = pos .+ mesh.latvec[:, ni] .* (mesh.barycheb[n] + 1.0) ./ 2.0
    end
    return pos
end
function Base.getindex(mesh::BaryChebMesh{DIM,N}, i::Int) where {DIM,N}
    return Base.getindex(mesh, _ind2inds(i, N, DIM)...)
end

Base.firstindex(mesh::BaryChebMesh) = 1
Base.lastindex(mesh::BaryChebMesh) = length(mesh)
# # iterator
Base.iterate(mesh::BaryChebMesh) = (mesh[1], 1)
Base.iterate(mesh::BaryChebMesh, state) = (state >= length(mesh)) ? nothing : (mesh[state+1], state + 1)

function interp(data, mesh::BaryChebMesh{DIM,N}, x) where {DIM,N}
    # translate into dimensionless
    displacement = SVector{DIM,Float64}(x) - mesh.origin
    xs = (mesh.invlatvec * displacement) .* 2.0 .- 1.0
    return interpND(data, mesh.barycheb, xs)
end

function integrate(data, mesh::BaryChebMesh{DIM,N}) where {DIM,N}
    area = abs(det(mesh.latvec))
    return integrateND(data, mesh.barycheb, DIM) * area / 2^DIM
end

function locate1d(g::BaryCheb1D, x)
    grid = g.x
    if x <= grid[1]
        return 1
    elseif x >= grid[end]
        if length(grid)!=1
            return length(grid)
        end
    end

    i2 = searchsortedfirst(grid, x)
    i1 = i2-1
    return abs(grid[i1]-x)<abs(grid[i2]-x) ? i1 : i2
end

function volume1d(g::BaryCheb1D, i)
    grid = g.x
    if i != 1 && i != length(grid)
        return (grid[i+1]-grid[i-1])/2
    elseif i == 1
        return (grid[i+1]+grid[i])/2 - (-1)
    else
        return 1 - (grid[i]+grid[i-1])/2
    end
end

function locate(mesh::BaryChebMesh{DIM,N}, x) where {DIM,N}
    displacement = SVector{DIM,Float64}(x) - mesh.origin
    xs = (mesh.invlatvec * displacement) .* 2.0 .- 1.0
    inds = [locate1d(mesh.barycheb, xs[i]) for i in 1:DIM]
    return _inds2ind(inds,N)
end

volume(mesh::BaryChebMesh) = abs(det(mesh.latvec))
function volume(mesh::BaryChebMesh{DIM,N}, i) where {DIM,N}
    inds = _ind2inds(i, N, DIM)
    return reduce(*, volume1d(mesh.barycheb, inds[j]) for j in 1:DIM) *volume(mesh)/2^DIM
end

end
  
