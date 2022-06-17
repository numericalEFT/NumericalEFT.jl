module FQR

using LinearAlgebra, Printf
using StaticArrays
# using GenericLinearAlgebra

const Float = Float64

### faster, a couple of less digits
using DoubleFloats
# const Float = Double64
const Double = Double64

# similar speed as DoubleFloats
# using MultiFloats
# const Float = Float64x2
# const Double = Float64x2

### a couple of more digits, but slower
# using Quadmath
# const Float = Float128

### 64 digits by default, but a lot more slower
# const Float = BigFloat

###################### traits to the functional QR  ############################
abstract type Grid end
abstract type FineMesh end

dot(mesh, g1, g2) = error("QR.dot is not implemented!")
mirror(g) = error("QR.mirror for $(typeof(g)) is not implemented!")
irreducible(g) = error("QR.irreducible for $(typeof(g)) is not implemented!")
#################################################################################

mutable struct Basis{D,Grid,Mesh}
    ############    fundamental parameters  ##################
    Λ::Float  # UV energy cutoff * inverse temperature
    rtol::Float # error tolerance

    ###############     DLR grids    ###############################
    N::Int # number of basis
    grid::Vector{Grid} # grid for the basis
    error::Vector{Float}  # the relative error achieved by adding the current grid point 

    ###############  linear coefficients for orthognalization #######
    Q::Matrix{Double} # , Q = R^{-1}, Q*R'= I
    R::Matrix{Double}

    ############ fine mesh #################
    mesh::Mesh

    function Basis{d,Grid}(Λ, rtol, mesh::Mesh) where {d,Grid,Mesh}
        _Q = Matrix{Float}(undef, (0, 0))
        _R = similar(_Q)
        return new{d,Grid,Mesh}(Λ, rtol, 0, [], [], _Q, _R, mesh)
    end
end

function addBasis!(basis::Basis{D,G,M}, grid, verbose) where {D,G,M}
    basis.N += 1
    push!(basis.grid, grid)

    basis.Q, basis.R = GramSchmidt(basis)

    # println(maximum(basis.mesh.residual))
    # update the residual on the fine mesh
    updateResidual!(basis)

    # println(maximum(basis.mesh.residual))
    # the new rtol achieved by adding the new grid point
    push!(basis.error, sqrt(maximum(basis.mesh.residual)))

    (verbose > 0) && @printf("%3i %s -> error=%16.8g, Rmin=%16.8g\n", basis.N, "$(grid)", basis.error[end], basis.R[end, end])
end

function addBasisBlock!(basis::Basis{D,G,M}, idx, verbose) where {D,G,M}
    _norm = sqrt(basis.mesh.residual[idx]) # the norm derived from the delta update in updateResidual
    addBasis!(basis, basis.mesh.candidates[idx], verbose)
    _R = basis.R[end, end] # the norm derived from the GramSchmidt

    @assert abs(_norm - _R) < basis.rtol * 100 "inconsistent norm on the grid $(basis.grid[end]) $_norm - $_R = $(_norm-_R)"
    if abs(_norm - _R) > basis.rtol * 10
        @warn("inconsistent norm on the grid $(basis.grid[end]) $_norm - $_R = $(_norm-_R)")
    end

    ## set the residual of the selected grid point to be zero
    basis.mesh.selected[idx] = true
    basis.mesh.residual[idx] = 0 # the selected mesh grid has zero residual

    # println(mirror(basis.mesh, idx))
    for grid in mirror(basis.mesh, idx)
        addBasis!(basis, grid, verbose)
    end
end

function updateResidual!(basis::Basis{D}) where {D}
    mesh = basis.mesh

    # q = Float.(basis.Q[end, :])
    q = Double.(basis.Q[:, end])

    Threads.@threads for idx in 1:length(mesh.candidates)
        if mesh.selected[idx] == false
            candidate = mesh.candidates[idx]
            pp = sum(q[j] * dot(mesh, basis.grid[j], candidate) for j in 1:basis.N)
            _residual = mesh.residual[idx] - pp * pp
            # @assert isnan(_residual) == false "$pp and $([q[j] for j in 1:basis.N]) => $([dot(mesh, basis.grid[j], candidate) for j in 1:basis.N])"
            # println("working on $candidate : $_residual")
            if _residual < 0
                if _residual < -basis.rtol
                    @warn("warning: residual smaller than 0 at $candidate got $(mesh.residual[idx]) - $(pp)^2 = $_residual")
                end
                mesh.residual[idx] = 0
            else
                mesh.residual[idx] = _residual
            end
        end
    end
end

"""
Gram-Schmidt process to the last grid point in basis.grid
"""
function GramSchmidt(basis::Basis{D,G,M}) where {D,G,M}
    _Q = zeros(Double, (basis.N, basis.N))
    _Q[1:end-1, 1:end-1] = basis.Q

    _R = zeros(Double, (basis.N, basis.N))
    _R[1:end-1, 1:end-1] = basis.R
    _Q[end, end] = 1

    newgrid = basis.grid[end]

    overlap = [dot(basis.mesh, basis.grid[j], newgrid) for j in 1:basis.N-1]

    for qi in 1:basis.N-1
        _R[qi, end] = basis.Q[:, qi]' * overlap
        _Q[:, end] -= _R[qi, end] * _Q[:, qi]  # <q, qnew> q
    end

    _norm = dot(basis.mesh, newgrid, newgrid) - _R[:, end]' * _R[:, end]
    _norm = sqrt(abs(_norm))

    @assert _norm > eps(Double(1)) * 100 "$_norm is too small as a denominator!\nnewgrid = $newgrid\nexisting grid = $(basis.grid)\noverlap=$overlap\nR=$_R\nQ=$_Q"

    _R[end, end] = _norm
    _Q[:, end] /= _norm

    return _Q, _R
end

function test(basis::Basis{D}) where {D}
    println("testing orthognalization...")
    KK = zeros(Double, (basis.N, basis.N))
    Threads.@threads for i in 1:basis.N
        g1 = basis.grid[i]
        for (j, g2) in enumerate(basis.grid)
            KK[i, j] = dot(basis.mesh, g1, g2)
        end
    end
    maxerr = maximum(abs.(KK - basis.R' * basis.R))
    println("Max overlap matrix R'*R Error: ", maxerr)

    maxerr = maximum(abs.(basis.R * basis.Q - I))
    println("Max R*R^{-1} Error: ", maxerr)

    II = basis.Q' * KK * basis.Q
    maxerr = maximum(abs.(II - I))
    println("Max Orthognalization Error: ", maxerr)

    # KK = zeros(Float, (basis.N, basis.N))
    # Threads.@threads for i in 1:basis.N
    #     g1 = basis.grid[i]
    #     for (j, g2) in enumerate(basis.grid)
    #         KK[i, j] = dot(basis.mesh, g1, g2)
    #     end
    # end
    # println(maximum(abs.(KK' - KK)))
    # A = cholesky(KK, Val{true}())
    # println(maximum(abs.(A.L * A.U - KK)))
    # println(maximum(abs.(A.L' - A.U)))
end

# function testResidual(basis, proj)
#     # residual = [Residual(basis, proj, basis.grid[i, :]) for i in 1:basis.N]
#     # println("Max deviation from zero residual: ", maximum(abs.(residual)))
#     println("Max deviation from zero residual on the DLR grids: ", maximum(abs.(basis.residualFineGrid[basis.gridIdx])))
# end

function qr!(basis::Basis{dim,G,M}; initial = [], N = 10000, verbose = 0) where {dim,G,M}
    #### add the grid in the idx vector first

    for i in initial
        addBasisBlock!(basis, i, verbose)
    end

    ####### add grids that has the maximum residual
    maxResidual, idx = findmax(basis.mesh.residual)
    while sqrt(maxResidual) > basis.rtol && basis.N < N
        addBasisBlock!(basis, idx, verbose)
        maxResidual, idx = findmax(basis.mesh.residual)
    end
    @printf("rtol = %.16e\n", sqrt(maxResidual))
    return basis
end

end