"""
Basis functions for the correlator
"""
module Basis
export tauGrid
using DelimitedFiles, LinearAlgebra
# include("spectral.jl")
using ..Spectral

function dlrGrid(type, Euv, β=1.0, eps=1e-10)
    Λ = Euv * β # dlr only depends on this dimensionless scale
    @assert eps > 0.0 "eps=$eps is not positive and nonzero!"
    @assert 0 < Λ < 1000000 "Energy scale $Λ must be in (0, 1000000)!"
    if Λ < 100 
        Λ = Int(100)
    else
        Λ = 10^(Int(ceil(log10(Λ)))) # get smallest n so that Λ<10^n
    end
    epspower = Int(floor(log10(eps))) # get the biggest n so that eps>1e-n
    if abs(epspower) < 4
        epspower = 4
    end

    if type == :fermi
        filename = string(@__DIR__, "/basis/dlr_fermi/dlr$(Λ)_1e$(epspower).dat")
        grid = readdlm(filename)

        ω = grid[:, 2] / β
        ωn = Int.(grid[:, 4])
        tgrid = [((t >= 0.0) ? t : 1.0 + t) for t in grid[:, 3]]
        τ = sort(tgrid * β)
        return Dict(:ω => ω, :τ => τ, :ωn => ωn)
    else
        @error "Not implemented!"
    end
end

function kernelT(type, τGrid, ωGrid, β)
    kernel = zeros(Float64, (length(τGrid), length(ωGrid)))
    for (τi, τ) in enumerate(τGrid)
        for (ωi, ω) in enumerate(ωGrid)
            if type == :fermi
                kernel[τi, ωi] = kernelFermiT(τ / β, ω * β)
            else
                @error "Not implemented"
            end
        end
    end
    return kernel
end

function tau2dlr(type, green, dlrGrid, β=1.0; axis=1, rtol=1e-12)
    @assert length(size(green)) >= axis "dimension of the Green's function should be larger than axis!"
    τGrid = dlrGrid[:τ]
    ωGrid = dlrGrid[:ω]
    kernel = kernelT(type, τGrid, ωGrid, β)
    kernel, ipiv, info = LAPACK.getrf!(Float64.(kernel)) # LU factorization

    if axis == 1
        g = copy(green)
    else
        g = permutedims(green, [axis, 1])
    end

    coeff = LAPACK.getrs!('N', kernel, ipiv, g) # LU linear solvor for coeff=kernel*green

    if axis == 1
        return coeff
    else
        return permutedims(coeff, [axis, 1])
    end
end

function dlr2tau(type, dlrcoeff, dlrGrid, β=1.0; axis=1)
    @assert length(size(dlrcoeff)) >= axis "dimension of the dlr coefficients should be larger than axis!"
    τGrid = dlrGrid[:τ]
    ωGrid = dlrGrid[:ω]
    kernel = kernelT(type, τGrid, ωGrid, β)
    if axis == 1
        coeff = dlrcoeff
    else
        coeff = permutedims(dlrcoeff, [axis, 1])
    end

    G = kernel * coeff # tensor dot product: \sum_i kernel[..., i]*coeff[i, ...]

    if axis == 1
        return G
    else
        return permutedims(G, [axis, 1])
    end
end

end