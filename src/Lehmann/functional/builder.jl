
module Functional

using LinearAlgebra: Matrix, zero, similar
using LinearAlgebra, Printf
# using Roots
# using Optim
using Quadmath
# using ProfileView
# using InteractiveUtils

using ..Discrete

# const Float = Float64
const Float = BigFloat
# const Float = Float128

include("kernel.jl")
include("findmax.jl")


# using Plots
# function plotResidual(basis, proj, gmin, gmax, candidate=nothing, residual=nothing)
#     ω = LinRange(gmin, gmax, 1000)
#     y = [Residual(basis, proj, w) for w in ω]
#     p = plot(ω, y, xlims=(gmin, gmax))
#     if isnothing(candidate) == false
#         plot!(p, candidate, residual, seriestype=:scatter)
#     end
#     display(p)
#     readline()
# end

mutable struct Basis
    Λ::Float
    rtol::Float

    N::Int # number of basis
    grid::Vector{Float} # grid for the basis
    residual::Vector{Float} # achieved error by each basis
    Q::Matrix{Float} # K = Q*R
    proj::Matrix{Float} # the overlap of basis functions <K(g_i), K(g_j)>

    candidate::Vector{Float}
    candidateResidual::Vector{Float}

    function Basis(Λ, rtol)
        _Q = Matrix{Float}(undef, (0, 0))
        return new(Λ, rtol, 0, [], [], _Q, similar(_Q), [], [])
    end
end

function addBasis!(basis, proj, g0::Float)
    basis.N += 1
    if basis.N == 1
        idx = 1
        basis.grid = [g0,]
        basis.Q = zeros(Float, (basis.N, basis.N))
        basis.Q[1, 1] = 1 / sqrt(proj(basis.Λ, g0, g0))
        basis.proj = projKernel(basis, proj)
    else
        idxList = findall(x -> x > g0, basis.grid)
        # if ω is larger than any existing freqencies, then idx is an empty list
        idx = length(idxList) == 0 ? basis.N : idxList[1] # the index to insert the new frequency

        insert!(basis.grid, idx, g0)
        basis.proj = projKernel(basis, proj)
        _Q = copy(basis.Q)
        basis.Q = zeros(Float, (basis.N, basis.N))
        basis.Q[1:idx-1, 1:idx-1] = _Q[1:idx-1, 1:idx-1]
        basis.Q[1:idx-1, idx+1:end] = _Q[1:idx-1, idx:end]
        basis.Q[idx+1:end, 1:idx-1] = _Q[idx:end, 1:idx-1]
        basis.Q[idx+1:end, idx+1:end] = _Q[idx:end, idx:end]
        # println(maximum(abs.(GramSchmidt(basis, idx, g0) .- mGramSchmidt(basis, idx, g0))))
        basis.Q[idx, :] = mGramSchmidt(basis, idx, g0)
    end

    scanResidual!(basis, proj, g0, idx)
    insert!(basis.residual, idx, maximum(basis.candidateResidual)) # record error after the new grid is added
    return idx
end

function scanResidual!(basis, proj, g0, idx)
    grids = copy(basis.grid)
    if basis.grid[1] > Float(0)
        insert!(grids, 1, Float(0))
    end
    if basis.grid[end] < basis.Λ
        append!(grids, basis.Λ)
    end
    resize!(basis.candidate, length(grids) - 1)
    resize!(basis.candidateResidual, length(grids) - 1)
    # println(g0, " and ", idx)
    # println(grids)

    for i = 1:length(grids)-1 # because of the separation of scales, the grids far away from idx is rarely affected
        g = findCandidate(basis, proj, grids[i], grids[i+1])
        basis.candidate[i] = g
        basis.candidateResidual[i] = Residual(basis, proj, g)
    end
end

function printCandidate(basis, idx)
    lower = (idx == 1) ? 0 : basis.grid[idx-1]
    upper = (idx == basis.N) ? basis.Λ : basis.grid[idx+1]

    @printf("%3i : ω=%24.8f ∈ (%24.8f, %24.8f) -> error=%24.16g\n", basis.N, basis.grid[idx], lower, upper, basis.residual[idx])
end

function QR(Λ, rtol, proj, g0; N = nothing, verbose = false)
    basis = Basis(Λ, rtol)
    # println(g0)
    for g in g0
        idx = addBasis!(basis, proj, Float(g))
        # @printf("%3i : ω=%24.8f ∈ (%24.8f, %24.8f) -> error=%24.16g\n", 1, g, 0, Λ, basis.residual[idx])
        verbose && printCandidate(basis, idx)
    end

    # @code_warntype Residual(basis, proj, Float(1.0))
    # exit(0)
    maxResidual, ωi = findmax(basis.candidateResidual)
    # plotResidual(basis, proj, Float(0), Float(100), basis.candidate, basis.candidateResidual)

    while isnothing(N) ? maxResidual > rtol / 10 : basis.N < N

        newω = basis.candidate[ωi]
        idx = addBasis!(basis, proj, newω)
        verbose && printCandidate(basis, idx)
        # println(length(basis.grid))
        # println(idx)
        # lower = (idx == 1) ? 0 : basis.grid[idx - 1]
        # upper = (idx == basis.N) ? Λ : basis.grid[idx + 1]

        # verbose && @printf("%3i : ω=%24.8f ∈ (%24.8f, %24.8f) -> error=%24.16g\n", basis.N, newω, lower, upper, basis.residual[idx])
        # println("$(length(freq)) basis: ω=$(Float64(newω)) between ($(Float64(freq[idx - 1])), $(Float64(freq[idx + 1])))")
        # plotResidual(basis, proj, Float(0), Float(100), candidate, residual)
        maxResidual, ωi = findmax(basis.candidateResidual)
    end
    testOrthgonal(basis, verbose)
    # @printf("residual = %.10e, Fnorm/F0 = %.10e\n", residual, residualF(freq, Q, Λ))
    verbose && @printf("residual = %.10e\n", maximum(basis.candidateResidual))
    # plotResidual(basis, proj, Float(0), Float(100), basis.candidate, basis.candidateResidual)
    return basis
end

"""
q1=sum_j c_j K_j
q2=sum_k d_k K_k
return <q1, q2> = sum_jk c_j*d_k <K_j, K_k>
"""
projqq(basis, q1::Vector{Float}, q2::Vector{Float}) = q1' * basis.proj * q2

"""
<K(g_i), K(g_j)>
"""
function projKernel(basis, proj)
    K = zeros(Float, (basis.N, basis.N))
    for i = 1:basis.N
        for j = 1:basis.N
            K[i, j] = proj(basis.Λ, basis.grid[i], basis.grid[j])
        end
    end
    return K
end

"""
modified Gram-Schmidt process
"""
function mGramSchmidt(basis, idx, g::Float)
    qnew = zeros(Float, basis.N)
    qnew[idx] = 1

    for qi = 1:basis.N
        if qi == idx
            continue
        end
        q = basis.Q[qi, :]
        qnew -= projqq(basis, q, qnew) .* q  # <q, qnew> q
    end
    return qnew / sqrt(projqq(basis, qnew, qnew))
end

# """
# Gram-Schmidt process
# """
# function GramSchmidt(basis, idx, g::Float)
#     q0 = zeros(Float, basis.N)
#     q0[idx] = 1
#     qnew = copy(q0)

#     for qi in 1:basis.N
#         if qi == idx
#     continue
#     end
#         q = basis.Q[qi, :]
#         qnew -=  projqq(basis, q, q0) .* q
#     end

#     norm = sqrt(projqq(basis, qnew, qnew))
#     return qnew / norm
# end

function Residual(basis, proj, g::Float)
    # norm2 = proj(g, g) - \sum_i (<qi, K_g>)^2
    # qi=\sum_j Q_ij K_j ==> (<qi, K_g>)^2 = (\sum_j Q_ij <K_j, K_g>)^2 = \sum_jk Q_ij*Q_ik <K_j, K_g>*<K_k, Kg>

    KK = [proj(basis.Λ, gj, g) for gj in basis.grid]
    norm2 = proj(basis.Λ, g, g) - (norm(basis.Q * KK))^2

    # norm2 = proj(basis.Λ, g, g)
    # for j in 1:basis.N
    #     norm2 -= basisQ[j, :]
    # end
    return norm2 < 0 ? Float(0) : sqrt(norm2)
end


function testOrthgonal(basis, verbose)
    # println("testing orthognalization...")
    II = basis.Q * basis.proj * basis.Q'
    maxerr = maximum(abs.(II - I))
    verbose && println("Max Orthognalization Error: ", maxerr)
end

"""
function build(dlrGrid, print::Bool = true)
    Construct discrete Lehmann representation

#Arguments:
- `dlrGrid`: struct that contains the information to construct the DLR grid. The following entries are required:
   Λ: the dimensionless scale β*Euv, rtol: the required relative accuracy, isFermi: fermionic or bosonic, symmetry: particle-hole symmetry/antisymmetry or none
- `print`: print the internal information or not
"""
function build(dlrGrid, print::Bool = true)
    print && println("Using the functional algorithm to build DLR ...")
    Λ = Float(dlrGrid.Λ)
    rtol = Float(dlrGrid.rtol)
    symmetry = dlrGrid.symmetry
    if symmetry == :ph
        print && println("Building ω grid ... ")
        ωBasis = QR(Λ, rtol, projPH_ω, [Float(0), Float(Λ)], verbose = print)
        ωGrid = ωBasis.grid
        rank = ωBasis.N
    elseif symmetry == :pha
        print && println("Building ω grid ... ")
        ωBasis = QR(Λ, rtol, projPHA_ω, [Float(Λ),], verbose = print)
        ωGrid = ωBasis.grid
        rank = ωBasis.N
    else
        error("Functional algorithm for the symmetry $symmetry has not yet been implemented!")
        # elseif type == :fermi
        #     println("Building ω grid ... ")
        #     ωBasis = QR(Λ, rtol, projPH_ω, [Float(0), Float(Λ)])
        #     ωGrid = vcat(-ωBasis.grid[end:-1:2], ωBasis.grid)
        #     rank = length(ωGrid)
        #     println("rank: $rank")
        #     println("Building τ grid ... ")
        #     τBasis = tauGrid(ωGrid, rank, Λ, rtol, :fermi)
        #     # τBasis = QR(Λ / 2, rtol / 10, projPH_τ, Float(0), N=ωBasis.N)
        #     println("Building n grid ... ")
        #     nBasis = MatFreqGrid(ωGrid, rank, Λ, :fermi)
    end

    ωGrid = Float64.(ωGrid)
    degree = 128
    τ = Discrete.τChebyGrid(dlrGrid, degree, print)
    kernel = Discrete.preciseKernelT(dlrGrid, τ, ωGrid, print)
    Discrete.testInterpolation(dlrGrid, τ, ωGrid, kernel, print)

    τIndex = Discrete.τnQR(kernel, rank, print)
    τGrid = sort(τ.grid[τIndex])

    nFineGrid, nFermiKernel, nBoseKernel = Discrete.preciseKernelΩn(dlrGrid, ωGrid, print)

    nFermiIndex = Discrete.τnQR(nFermiKernel, rank, print)
    nFermiGrid = sort(nFineGrid[nFermiIndex])

    nBoseIndex = Discrete.τnQR(nBoseKernel, rank, print)
    nBoseGrid = sort(nFineGrid[nBoseIndex])

    # τGrid = τBasis / Λ
    # τGrid = τBasis
    # nGrid = nBasis
    ########### output  ############################
    # @printf("%5s  %32s  %32s  %8s\n", "index", "real freq", "tau", "ωn")
    # for r = 1:rank
    #     @printf("%5i  %32.17g  %32.17g  %16i\n", r, ωGrid[r], τGrid[r], nGrid[r])
    # end
    ########### output  ############################
    print && @printf("%5s  %32s  %32s  %11s  %11s\n", "index", "real freq", "tau", "fermi ωn", "bose ωn")
    for r = 1:rank
        print && @printf("%5i  %32.17g  %32.17g  %16i %16i\n", r, ωGrid[r], τGrid[r], nFermiGrid[r], nBoseGrid[r])
    end

    return ωGrid, τGrid, nFermiGrid, nBoseGrid
    # return Dict([(:ω, ωGrid), (:τ, τGrid), (:ωn, nGrid)])
end

end

if abspath(PROGRAM_FILE) == @__FILE__
    # freq, Q = findBasis(1.0e-3, Float(100))
    # basis = QR(100, 1e-3)
    Λ = 1e10
    # Λ = 100
    # @time ωBasis = QR(Λ, 1e-13, projPH_ω, [Float(0), Float(Λ)])
    @time ωBasis = Functional.QR(Λ, 1e-12, projPHA_ω, [Float(Λ),])
    # @time τBasis = QR(Λ / 2, 1e-11, projPHA_τ, Float(0), N=ωBasis.N)
    # nBasis = MatFreqGrid(ωBasis.grid, ωBasis.N, Λ, :acorr)

    # @time basis = QR(100, 1e-10)
    # readline()
    # basis = QR(100, 1e-3)

end