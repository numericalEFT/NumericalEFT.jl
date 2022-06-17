include("QR.jl")
# using QR
using Lehmann
using StaticArrays, Printf

const Float = FQR.Float
const Double = FQR.Double
const DotF = FQR.Float
const Tiny = DotF(1e-5)

struct FreqGrid{D} <: FQR.Grid
    sector::Int                       # sector
    omega::SVector{D,Float}             # actual location of the grid point   
    coord::SVector{D,Int}         # integer coordinate of the grid point on the fine meshes
end

Base.show(io::IO, grid::FreqGrid{2}) = print(io, "ω$(grid.sector) = ($(@sprintf("%12.4f", grid.omega[1])), $(@sprintf("%12.4f", grid.omega[2])))")

struct FreqFineMesh{D} <: FQR.FineMesh
    color::Int                            # D+1 sectors
    symmetry::Int                         # symmetrize colors and (omega1, omega2) <-> (omega2, omega1)
    candidates::Vector{FreqGrid{D}}       # vector of grid points
    selected::Vector{Bool}
    residual::Vector{Double}

    ## for frequency mesh only ###
    fineGrid::Vector{Float}         # fine grid for each dimension
    cache1::Vector{DotF}            # cache for exp(-x)
    cache2::Matrix{DotF}            # cache for exp(-x-y)


    function FreqFineMesh{D}(Λ, rtol; sym = 0) where {D}
        # initialize the residual on fineGrid with <g, g>
        _finegrid = Float.(fineGrid(Λ, rtol))
        separationTest(D, _finegrid)
        Nfine = length(_finegrid)

        _cache1 = zeros(DotF, Nfine)
        _cache2 = zeros(DotF, (Nfine, Nfine))
        for (xi, x) in enumerate(_finegrid)
            _cache1[xi] = exp(-DotF(x))
            for (yi, y) in enumerate(_finegrid)
                _cache2[xi, yi] = exp(-DotF(x) - DotF(y))
            end
        end

        color = D + 1
        # color = 1
        mesh = new{D}(color, sym, [], [], [], _finegrid, _cache1, _cache2)

        if D == 2
            for (xi, x) in enumerate(_finegrid)
                for (yi, y) in enumerate(_finegrid)
                    coord = (xi, yi)
                    for sector in 1:color
                        if irreducible(D, sector, coord, sym)  # if grid point is in the reducible zone, then skip residual initalization
                            g = FreqGrid{D}(sector, (x, y), coord)
                            push!(mesh.candidates, g)
                            push!(mesh.residual, FQR.dot(mesh, g, g))
                            push!(mesh.selected, false)
                        end
                    end
                end
            end
            # elseif D == 3
        else
            error("not implemented!")
        end
        println("fine mesh initialized.")
        return mesh
    end
end

"""
composite expoential grid
"""
function fineGrid(Λ, rtol)
    ############## use composite grid #############################################
    # degree = 8
    # ratio = Float(1.4)
    # N = Int(floor(log(Λ) / log(ratio) + 1))
    # panel = [Λ / ratio^(N - i) for i in 1:N]
    # grid = Vector{Float}(undef, 0)
    # for i in 1:length(panel)-1
    #     uniform = [panel[i] + (panel[i+1] - panel[i]) / degree * j for j in 0:degree-1]
    #     append!(grid, uniform)
    # end
    # append!(grid, Λ)
    # println(grid)
    # println("Composite expoential grid size: $(length(grid))")
    # return grid

    ############# DLR based fine grid ##########################################
    dlr = DLRGrid(Euv = Float64(Λ), beta = 1.0, rtol = Float64(rtol) / 100, isFermi = true, symmetry = :ph, rebuild = true)
    # println("fine basis number: $(dlr.size)\n", dlr.ω)
    degree = 4
    grid = Vector{Double}(undef, 0)
    panel = Double.(dlr.ω)
    for i in 1:length(panel)-1
        uniform = [panel[i] + (panel[i+1] - panel[i]) / degree * j for j in 0:degree-1]
        append!(grid, uniform)
    end

    println("fine grid size: $(length(grid)) within [$(grid[1]), $(grid[2])]")
    return grid
end

"""
Test the finegrids do not overlap
"""
function separationTest(D, finegrid)
    if D == 2
        epsilon = eps(DotF(1)) * 10
        for (i, f) in enumerate(finegrid)
            # either zero, or sufficiently large
            @assert abs(f) < epsilon || abs(f) > Tiny "$i: $f should either smaller than $epsilon or larger than $Tiny"
            for (j, g) in enumerate(finegrid)
                # two frequencies are either the same, or well separated
                @assert abs(f - g) < epsilon || abs(f - g) > Tiny "$i: $f and $j: $g should either closer than $epsilon or further than $Tiny"
                fg = f + g
                for (k, l) in enumerate(finegrid)
                    @assert abs(l - fg) < epsilon || abs(l - fg) > Tiny "$i: $f + $j: $g = $fg and $k: $l should either closer than $epsilon or further than $Tiny"
                end
            end
        end
    elseif D==1
        return
    else
        error("not implemented!")
    end
end

function coord2omega(mesh::FreqFineMesh{dim}, coord) where {dim}
    fineGrid = mesh.fineGrid
    if dim == 1
        return fineGrid[coord[1]]
    elseif dim == 2
        return (fineGrid[coord[1]], fineGrid[coord[2]])
    elseif dim == 3
        return (fineGrid[coord[1]], fineGrid[coord[2]], fineGrid[coord[3]])
    else
        error("not implemented!")
    end
end

function irreducible(D, sector, coord, symmetry)
    if symmetry == 0
        return true
    else
        if D == 2
            # return (coord[1] <= coord[2]) && (sector == 1)
            return (coord[1] <= coord[2])
        elseif D == 3
            # return (coord[1] <= coord[2] <= coord[3]) && (sector == 1)
            return (coord[1] <= coord[2] <= coord[3])
        else
            error("not implemented!")
        end
    end
end

function FQR.irreducible(grid::FreqGrid{D}) where {D}
    return irreducible(D, grid.sector, grid.coord, mesh.symmetry)
end

function FQR.mirror(mesh::FreqFineMesh{D}, idx) where {D}
    grid = mesh.candidates[idx]
    coord, s = grid.coord, grid.sector
    if mesh.symmetry == 0
        return []
    end
    if D == 2
        x, y = coord
        coords = unique([(x, y), (y, x),])
        # println(coords)
    elseif D == 3
        x, y, z = coord
        coords = unique([(x, y, z), (x, z, y), (y, x, z), (y, z, x), (z, x, y), (z, y, x)])
    else
        error("not implemented!")
    end
    newgrids = FreqGrid{D}[]
    # for s in 1:mesh.color
    for c in coords
        if s != grid.sector || c != Tuple(grid.coord)
            push!(newgrids, FreqGrid{D}(s, coord2omega(mesh, c), c))
        end
    end
    # end
    return newgrids
end

"""
F(x) = (1-exp(-y))/(x-y)
"""
# @inline function G2d(a::T, b::T, expa::T, expb::T) where {T}
#     if abs(a - b) > Tiny
#         return (expa - expb) / (b - a)
#     else
#         return (expa + expb) / 2
#     end
# end

"""
G(x, y) = (exp(-x)-exp(-y))/(x-y)
G(x, x) = -exp(-x)
"""
@inline function G2d(a::T, b::T, expa::T, expb::T) where {T}
    if abs(a - b) > Tiny
        return (expa - expb) / (b - a)
    else
        return (expa + expb) / 2
    end
end

"""
F(a, b, c) = (G(a, c)-G(a, c))/(a-b) where a != b, but a or b could be equal to c
"""
@inline function F2d(a::T, b::T, c::T, expa::T, expb::T, expc::T) where {T}
    @assert abs(a - b) > Tiny "$a - $c > $Tiny"
    return (G2d(a, c, expa, expc) - G2d(b, c, expb, expc)) / (b - a)
end

"""
F(any, any, 0)
"""
@inline function Fii2d(ω1::T, ω2::T, expω1::T, expω2::T) where {T}
    if ω1 < Tiny && ω2 < Tiny
        return T(1) / 2
    elseif ω1 < Tiny && ω2 > Tiny
        return (1 - ω2 - expω2) / ω2 / (ω1 - ω2)
    elseif ω1 > Tiny && ω2 < Tiny
        return (1 - ω1 - expω1) / ω1 / (ω2 - ω1)
    elseif abs(ω1 - ω2) < Tiny
        # @assert abs(ω1 - ω2) < eps(Float(1)) * 1000 "$ω1 - $ω2 = $(ω1-ω2)"
        ω = (ω1 + ω2) / 2
        expω = (expω1 + expω2) / 2
        return T((1 - expω * (1 + ω)) / ω / ω)
    else
        return T((ω1 - ω2 + expω1 * ω2 - expω2 * ω1) / (ω1 * ω2 * (ω1 - ω2)))
    end
end

"""
F(a,b,c)
"""
@inline function Fij2d(a::T, b::T, c::T, expa::T, expb::T, expc::T) where {T}
    if abs(a - b) > Tiny #a!=b
        return F2d(a, b, c, expa, expb, expc)
    else # a=b
        if abs(a - c) > Tiny # a=b != c
            return F2d(a, c, b, expa, expc, expb)
        else # a==b==c: exp(-a)/2
            return (expa + expb + expc) / 6
        end
    end
end

"""
basis dot for 2D
"""
function FQR.dot(mesh::FreqFineMesh{2}, g1::FreqGrid{2}, g2::FreqGrid{2})
    # println("dot: ", g1, ", ", g2)
    cache1 = mesh.cache1
    cache2 = mesh.cache2
    s1, s2 = g1.sector, g2.sector
    c1, c2 = g1.coord, g2.coord
    if s1 == s2  # F11, F22, F33
        ω1, ω2 = g1.omega[1] + g2.omega[1], g1.omega[2] + g2.omega[2]
        expω1 = cache2[c1[1], c2[1]]
        expω2 = cache2[c1[2], c2[2]]
        return Fii2d(ω1, ω2, expω1, expω2)
    elseif (s1 == 1 && s2 == 2) || (s1 == 2 && s2 == 3) || (s1 == 3 && s2 == 1) #F12, F23, F31
        a, b, c = g2.omega[2], g1.omega[1], g1.omega[2] + g2.omega[1]
        ea, eb, ec = cache1[c2[2]], cache1[c1[1]], cache2[c1[2], c2[1]]
        return Fij2d(a, b, c, ea, eb, ec)
    else  #F21, F32, F13
        a, b, c = g1.omega[2], g2.omega[1], g2.omega[2] + g1.omega[1]
        ea, eb, ec = cache1[c1[2]], cache1[c2[1]], cache2[c2[2], c1[1]]
        return Fij2d(a, b, c, ea, eb, ec)
    end
end

if abspath(PROGRAM_FILE) == @__FILE__

    D = 2

    lambda, rtol = 10, 1e-4
    mesh = FreqFineMesh{D}(lambda, rtol, sym = 0)

    # KK = zeros(3, 3)
    # n = (2, 2)
    # o = (mesh.fineGrid[n[1]], mesh.fineGrid[n[2]])
    # for i in 1:3
    #     g1 = FreqGrid{2}(i, o, n)
    #     for j in 1:3
    #         g2 = FreqGrid{2}(j, o, n)
    #         println(g1, ", ", g2)
    #         KK[i, j] = FQR.dot(mesh, g1, g2)
    #     end
    # end
    # display(KK)
    # println()

    basis = FQR.Basis{D,FreqGrid{D}}(lambda, rtol, mesh)
    FQR.qr!(basis, verbose = 1)

    lambda, rtol = 1000, 1e-8
    mesh = FreqFineMesh{D}(lambda, rtol, sym = 0)
    basis = FQR.Basis{D,FreqGrid{D}}(lambda, rtol, mesh)
    @time FQR.qr!(basis, verbose = 1)

    FQR.test(basis)

    mesh = basis.mesh
    grids = basis.grid
    open("basis.dat", "w") do io
        for (i, grid) in enumerate(grids)
            if grid.sector == 1
                println(io, grid.omega[1], "   ", grid.omega[2])
            end
        end
    end
    Nfine = length(mesh.fineGrid)
    open("finegrid.dat", "w") do io
        for i in 1:Nfine
            println(io, basis.mesh.fineGrid[i])
        end
    end
    open("residual.dat", "w") do io
        # println(mesh.symmetry)
        residual = zeros(Double, Nfine, Nfine)
        for i in 1:length(mesh.candidates)
            if mesh.candidates[i].sector == 1
                x, y = mesh.candidates[i].coord
                residual[x, y] = mesh.residual[i]
                # println(x, ", ", y, " -> ", length(mirror(mesh, i)))

                for grid in FQR.mirror(mesh, i)
                    if grid.sector == 1
                        xp, yp = grid.coord
                        residual[xp, yp] = residual[x, y]
                        # println(xp, ", ", yp)
                    end
                end
            end
        end

        for i in 1:Nfine
            for j in 1:Nfine
                println(io, residual[i, j])
            end
        end
    end
end