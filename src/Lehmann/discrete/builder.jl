module Discrete
using Printf
using LinearAlgebra
using ..Spectral
using ..Interp
include("kernel.jl")

function ωQR(kernel, rtol, print::Bool = true)
    # print && println(τ.grid[end], ", ", τ.panel[end])
    # print && println(ω.grid[end], ", ", ω.panel[end])
    ################# find the rank ##############################
    """
    For a given index k, decompose R=[R11, R12; 0, R22] where R11 is a k×k matrix. 
    If R11 is well-conditioned, then 
    σᵢ(R11) ≤ σᵢ(kernel) for 1≤i≤k, and
    σⱼ(kernel) ≤ σⱼ₋ₖ(R22) for k+1≤j≤N
    See Page 487 of the book: Golub, G.H. and Van Loan, C.F., 2013. Matrix computations. 4th. Johns Hopkins.
    Thus, the effective rank is defined as the minimal k that satisfy rtol≤ σ₁(R22)/σ₁(kernel)
    """
    Nτ, Nω = size(kernel)

    u, σ, v = svd(kernel)
    rank, err = 1, 0.0
    for (si, s) in enumerate(σ)
        # println(si, " => ", s / σ[1])
        if s / σ[1] < rtol
            rank = si - 1
            err = s[1] / σ[1]
            break
        end
    end
    print && println("Kernel ϵ-rank = ", rank, ", rtol ≈ ", err)

    Q, R, p = qr(kernel, Val(true)) # julia qr has a strange design, Val(true) will do a pivot QR
    # size(R) == (Nτ, Nω) if Nω>Nτ
    # or size(R) == (Nω, Nω) if Nω<Nτ

    for idx = rank:min(Nτ, Nω)
        if Nω > Nτ
            R22 = R[idx:Nτ, idx:Nω]
        else
            R22 = R[idx:Nω, idx:Nω]
        end
        u2, s2, v2 = svd(R22)
        # println(idx, " => ", s2[1] / σ[1])
        if s2[1] / σ[1] < rtol
            rank = idx
            err = s2[1] / σ[1]
            break
        end
    end
    print && println("DLR rank      = ", rank, ", rtol ≈ ", err)

    # @assert err ≈ 4.58983288255442e-13

    return p[1:rank]
end

function τnQR(kernel, rank, print::Bool = true)
    ###########  dlr grid for τ  ###################
    print && println("Calculating τ and ωn grid ...")
    @assert rank == size(kernel)[2] #the ω dimension of the τkernel should be the effective rank
    τnqr = qr(transpose(kernel), Val(true)) # julia qr has a strange, Val(true) will do a pivot QR
    return τnqr.p[1:rank]
end

function buildωn(dlrGrid, print::Bool = true)
    ###########  dlr grid for ωn  ###################
    print && println("Calculating ωn grid ...")
    symmetry = dlrGrid.symmetry
    rank = dlrGrid.size

    if symmetry == :ph || symmetry == :pha
        Nωn = Int(ceil(Λ)) * 2 # expect Nω ~ para.Λ/2π, drop 2π on the safe side
        ωnkernel = zeros(Float64, (rank, Nωn + 1))
        ωnGrid = [w for w = 0:Nωn]
        # fermionic Matsubara frequency ωn=(2n+1)π for type==:acorr
        # bosonic Matsubara frequency ωn=2nπ for type==:corr
    else
        Nωn = Int(ceil(Λ)) * 2 # expect Nω ~ para.Λ/2π, drop 2π on the safe side
        ωnkernel = zeros(Complex{Float64}, (rank, 2Nωn + 1))
        ωnGrid = [w for w = -Nωn:Nωn] # fermionic Matsubara frequency ωn=(2n+1)π
    end

    for (ni, n) in enumerate(ωnGrid)
        for r = 1:rank
            ωnkernel[r, ni] = Spectral.kernelΩ(type, n, ωGridDLR[r])
        end
    end
    nqr = qr(ωnkernel, Val(true)) # julia qr has a strange, Val(true) will do a pivot QR
    nGridDLR = sort(ωnGrid[nqr.p[1:rank]])
    return nGridDLR, nqr.p[1:rank]
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
    degree = 24 # number of Chebyshev nodes in each panel
    Λ = dlrGrid.Λ
    rtol = dlrGrid.rtol
    τ = τChebyGrid(dlrGrid, degree, print)
    ω = ωChebyGrid(dlrGrid, degree, print)
    print && println("τ grid size = $(τ.ngrid)")
    print && println("ω grid size = $(ω.ngrid)")

    kernel = preciseKernelT(dlrGrid, τ, ω, print)
    testInterpolation(dlrGrid, τ, ω, kernel, print)

    ωIndex = ωQR(kernel, rtol, print)
    rank = length(ωIndex)
    ωGrid = sort(ω.grid[ωIndex])

    τIndex = τnQR(kernel[:, ωIndex], rank, print)
    τGrid = sort(τ.grid[τIndex])

    nFineGrid, nFermiKernel, nBoseKernel = preciseKernelΩn(dlrGrid, ωGrid, print)

    nFermiIndex = τnQR(nFermiKernel, rank, print)
    nFermiGrid = sort(nFineGrid[nFermiIndex])

    nBoseIndex = τnQR(nBoseKernel, rank, print)
    nBoseGrid = sort(nFineGrid[nBoseIndex])

    ########### output  ############################
    print && @printf("%5s  %32s  %32s  %11s  %11s\n", "index", "real freq", "tau", "fermi ωn", "bose ωn")
    for r = 1:rank
        print && @printf("%5i  %32.17g  %32.17g  %16i %16i\n", r, ωGrid[r], τGrid[r], nFermiGrid[r], nBoseGrid[r])
    end

    # dlr = Dict([(:ω, ωGrid), (:τ, τGrid), (:ωn, nFermiGrid)])
    return ωGrid, τGrid, nFermiGrid, nBoseGrid
end
end