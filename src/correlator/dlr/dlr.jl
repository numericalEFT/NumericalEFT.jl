"""
discrete Lehmann Representation
"""
module DLR

include("chebyshev.jl")
include("../spectral.jl")
using .Spectral
using LinearAlgebra, Printf

"""
gridparams(Λ, p, npt, npo, nt, no)

    Set parameters for composite Chebyshev fine grid

#Arguments:
- `Λ`: cutoff = UV Energy scale of the spectral density * inverse temperature
- `rtol`: tolerance absolute error
"""
struct Params
    Λ::Float64 
    rtol::Float64
    p::Int # Chebyshev degree in each subinterval
    npt::Int # subintervals on [0,1/2] in tau space (# subintervals on [0,1] is 2*npt)
    npo::Int # subintervals on [0,lambda] in omega space (subintervals on [-lambda,lambda] is 2*npo)
    nt::Int # fine grid points in tau = 2*npt*p
    no::Int # fine grid points in omega = 2*npo*p
    function Params(Λ, rtol)
        p = 24
        npt = Int(ceil(log(Λ) / log(2.0))) - 2
        npo = Int(ceil(log(Λ) / log(2.0)))
        nt = 2 * p * npt
        no = 2 * p * npo
        return new(Λ, rtol, p, npt, npo, nt, no)
    end
end

"""
kernalFineGrid(type, Λ, npt, npo)

Discretization of kernel K(tau,omega) on composite Chebyshev fine grids in tau and omega

#Arguments:
- `type`: :fermi or :bose
- `para':  Paramaters

#Returns
- `τGrid`: tau fine grid points on (0,1/2) (half of full grid)
- `ωGrid`: omega fine grid points
- `kernel`: K(tau,omega) on fine grid 
- `err`: Error of composite Chebyshev interpolant of K(tau,omega). err(1) is ~= max relative L^inf error in tau over all omega in fine grid. err(2) is ~= max L^inf error in omega over all tau in fine grid.
"""
function kernalFineGrid(type, para::Params)
    p = para.p
    Λ, npt, npo = para.Λ, para.npt, para.npo
    nt = 2npt * p
    no = 2npo * p
    xc, wc = barychebinit(p)
    ############# Tau discretization ##############
    # Panel break points
    pbpt = zeros(Float64, 2npt + 1)
    pbpt[1] = 0.0
    for i in 1:npt
        pbpt[i + 1] = 1.0 / 2^(npt - i + 1)
    end
    pbpt[npt + 2:2npt + 1] = 1 .- pbpt[npt:-1:1]
    # for i in 1:npt
    #     pbpt[npt + i + 1] = 0.5 / 2^(npt - i)
    # end

    # Grid points
    τGrid = zeros(Float64, 2npt * p)
    for i in 1:2npt
        a = pbpt[i]
        b = pbpt[i + 1]
        τGrid[(i - 1) * p + 1:i * p] = a .+ (b - a) .* (xc .+ 1.0) ./ 2
    end
    
    ############ ω discretization ##################
    # Panel break points
    pbpo = zeros(Float64, 2npo + 1)
    pbpo[npo + 1] = 0.0
    for i in 1:npo
        pbpo[npo + i + 1] = Λ / 2^(npo - i)
    end
    
    pbpo[1:npo] = -pbpo[2npo + 1:-1:npo + 2]

    # Grid points
    
    ωGrid = zeros(Float64, 2npo * p)
    for i in 1:2npo
        a = pbpo[i]
        b = pbpo[i + 1]
        ωGrid[(i - 1) * p + 1:i * p] = a .+ (b - a) .* (xc .+ 1.0) ./ 2
    end
    
    kernel = Spectral.kernelT(type, τGrid, ωGrid, 1.0)

    # Check accuracy of Cheb interpolant on each panel in tau for fixed omega, and each panel in omega for fixed tau, by comparing with K(tau,omega) on Cheb grid of 2*p nodes 
    err = [0.0, 0.0]
    xc2, wc2 = barychebinit(2p) # double the chebyshev interpolation order
    
    for j in 1:no
        errtmp = 0.0
        for i in 1:npt
            a, b = pbpt[i], pbpt[i + 1]
            for k in 1:2p
                xx = a + (b - a) * (xc2[k] + 1.0) / 2
                ktrue = Spectral.kernelT(type, xx, ωGrid[j], 1.0)
                ktest = barycheb(p, xc2[k], kernel[(i - 1) * p + 1:i * p,j], wc, xc)
                errtmp = max(errtmp, abs(ktrue - ktest))
            end
        end
        err[1] = max(err[1], errtmp / maximum(kernel[:,j]))
    end
    
    for j in 1:Int(nt / 2)
        errtmp = 0.0
        for i in 1:2npo
            a, b = pbpo[i], pbpo[i + 1]
            for k in 1:2p
                xx = a + (b - a) * (xc2[k] + 1.0) / 2
                ktrue = Spectral.kernelT(type, τGrid[j], xx, 1.0)
                ktest = barycheb(p, xc2[k], kernel[j,(i - 1) * p + 1:i * p], wc, xc)
                errtmp = max(errtmp, abs(ktrue - ktest))
            end
        end
        err[2] = max(err[2], errtmp / maximum(kernel[j, :]))
    end
    
    println("=====  Kernel Discretization =====")
    println("fine grid points for τ     = ", length(τGrid))
    println("fine grid points for ω     = ", length(ωGrid))
    println("Max relative L∞ error in τ = ", err[1])
    println("Max relative L∞ error in ω = ", err[2])
    
    return τGrid, ωGrid, kernel
end

"""
function dlr(type, Λ, rtol)
    Construct discrete Lehmann representation

#Arguments:
- `type`: type of kernel, :fermi, :boson
- `Λ`: cutoff = UV Energy scale of the spectral density * inverse temperature
- `rtol`: tolerance absolute error
"""
function dlr(type, Λ, rtol)
    Λ = Float64(Λ)
    @assert 0.0 < rtol < 1.0
    para = Params(Λ, rtol)
    τGrid, ωGrid, kernel = kernalFineGrid(type, para)
    result = qr(kernel, Val(true)) # julia qr has a strange, Val(true) will do a pivot QR
    Q, R, p = result.Q, result.R, result.p
    Nτ, Nω = length(τGrid), length(ωGrid)
    ################# find the rank ##############################
    """
    For a given index k, decompose R=[R11, R12; 0, R22] where R11 is a k×k matrix. 
    If R11 is well-conditioned, then 
    σᵢ(R11) ≤ σᵢ(kernel) for 1≤i≤k, and
    σⱼ(kernel) ≤ σⱼ₋ₖ(R22) for k+1≤j≤N
    See Page 487 of the book: Golub, G.H. and Van Loan, C.F., 2013. Matrix computations. 4th. Johns Hopkins.
    Thus, the effective rank is defined as the minimal k that satisfy rtol≤ σ₁(R22)/σ₁(kernel)
    """
    u, σ, v = svd(kernel)
    rank = 1
    err = 0.0
    for (si, s) in enumerate(σ)
        # println(si, " => ", s / σ[1])
        if s / σ[1] < rtol
            rank = si - 1
            err = s[1] / σ[1]
            break
        end
    end
    println("Kernel ϵ-rank = ", rank, ", rtol ≈ ", err)

    for idx in rank:min(Nτ, Nω) 
        R22 = R[idx:Nτ, idx:Nω]
        u2, s2, v2 = svd(R22)
        # println(idx, " => ", s2[1] / σ[1])
        if s2[1] / σ[1] < rtol
            rank = idx
            err = s2[1] / σ[1]
            break
        end
    end
    println("DLR rank      = ", rank,  ", rtol ≈ ", err)

    ###########  dlr grid for ω  ###################
    ωGridDLR = sort(ωGrid[p[1:rank]])

    ###########  dlr grid for τ  ###################
    τkernel = zeros(Float64, (rank, Nτ))
    for τi in 1:Nτ
        for r in 1:rank
            τkernel[r, τi] = kernel[τi, p[r]]
        end
    end
    τqr = qr(τkernel, Val(true)) # julia qr has a strange, Val(true) will do a pivot QR
    τGridDLR = sort(τGrid[τqr.p[1:rank]])
    
    ##########  dlr grid for ωn  ###################
    Nωn = Int(para.Λ) # expect Nω ~ para.Λ/2π, drop 2π on the safe side
    ωnkernel = zeros(Complex{Float64}, (rank, Nωn + 1))
    ωnGrid = [w for w in 0:Nωn] # fermionic Matsubara frequency ωn=(2n+1)π
    for (ni, n) in enumerate(ωnGrid)
        for r in 1:rank
            ωnkernel[r, ni] = Spectral.kernelΩ(:fermi, n, ωGridDLR[r])
        end
    end
    nqr = qr(ωnkernel, Val(true)) # julia qr has a strange, Val(true) will do a pivot QR
    nGridDLR = sort(ωnGrid[nqr.p[1:rank]])

    ########### output  ############################
    @printf("%5s  %32s  %32s  %8s\n", "index", "real freq", "tau", "ωn")
    for r in 1:rank
        @printf("%5i  %32.17g  %32.17g  %8i\n", r, ωGridDLR[r], τGridDLR[r], nGridDLR[r])
    end

    dlr = Dict([(:ω, ωGridDLR), (:τ, τGridDLR), (:ωn, nGridDLR)])
    return dlr
end

end