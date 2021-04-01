"""
Spectral representation related functions
"""
module Spectral
export kernelT, kernelΩ, density, freq2Tau, freq2MatFreq
export kernelFermiT, kernelFermiΩ, kernelBoseT, kernelBoseΩ, fermiDirac, boseEinstein
using QuadGK

"""
    kernelT(type, τ, ω, β=1.0)

Compute the imaginary-time kernel of different type. Assume ``k_B T/\\hbar=1``

# Arguments
- `type`: symbol :fermi, :bose, :corr
- `τ`: the imaginary time, must be (-1, 1]
- `ω`: frequency
- `β = 1.0`: the inverse temperature 
"""
@inline function kernelT(type::Symbol, τ::T, ω::T, β::T=1.0) where {T <: AbstractFloat}
    if type == :fermi
        return kernelFermiT(τ, ω, β)
    elseif type == :bose
        return kernelBoseT(τ, ω, β)
    else
        @error "Type $type  is not implemented!"
    end
end

"""
    kernelFermiT(τ, ω, β=1.0)

Compute the imaginary-time fermionic kernel. Assume ``k_B T/\\hbar=1``
```math
g(τ>0) = e^{-ωτ}/(1+e^{-ω}), g(τ≤0) = -e^{-ωτ}/(1+e^{ω})
```

# Arguments
- `τ`: the imaginary time, must be (-1, 1]
- `ω`: frequency
- `β = 1.0`: the inverse temperature 
"""
@inline function kernelFermiT(τ::T, ω::T, β::T=1.0) where {T <: AbstractFloat}
    (-β < τ <= β) || error("τ must be (-β, β]")
        if τ == T(0.0)
        τ = -eps(T)
    end
    G = sign(τ)
    if τ < T(0.0)
        τ += 1.0
    end
    x = ω * β / 2
    y = 2τ / β - 1
    if -T(100.0) < x < T(100.0)
        G *= exp(-x * y) / (2 * cosh(x))
    elseif x >= T(100.0)
        G *= exp(-x * (y + 1))
    else # x<=-100.0
        G *= exp(x * (1 - y))
    end
    return G
end

"""
    kernelBoseT(τ, ω, β=1.0)

Compute the imaginary-time bosonic kernel. Assume ``k_B T/\\hbar=1``
```math
g(τ>0) = e^{-ωτ}/(1+e^{-ω}), g(τ≤0) = -e^{-ωτ}/(1+e^{ω})
```

# Arguments
- `τ`: the imaginary time, must be (-1, 1]
- `ω`: frequency
- `β = 1.0`: the inverse temperature 
"""
@inline function kernelBoseT(τ::T, ω::T, β::T=1.0) where {T <: AbstractFloat}
    (-β < τ <= β) || error("τ must be (-β, β]")
    if τ == T(0.0)
        τ = -eps(T)
    end
    if τ < T(0.0)
        τ += 1.0
    end
    # if -eps(T) < ω <eps(T) #ω->0 makes the kernel diverge
    #     return 0.0
    # end
    x = ω * β / 2
    y = 2τ / β - 1
    if -T(100.0) < x < T(100.0)
        G = exp(-x * y) / (2 * sinh(x))
    elseif x >= T(100.0)
        G = exp(-x * (y + 1))
    else # x<=-100.0
        G = exp(x * (1 - y))
    end
    @assert isfinite(G)
    return G
end

"""
    kernelΩ(type, n, ω, β=1.0)

Compute the imaginary-time kernel of different type. Assume ``k_B T/\\hbar=1``

# Arguments
- `type`: symbol :fermi, :bose, :corr
- `n`: index of the Matsubara frequency
- `ω`: energy 
- `β`: the inverse temperature 
"""
@inline function kernelΩ(type::Symbol, n::Int, ω::T, β::T=1.0) where {T <: AbstractFloat}
    if type == :fermi
        return kernelFermiΩ(n, ω, β)
    elseif type == :bose
        return kernelBoseΩ(n, ω, β)
    else
        @error "Type $type  is not implemented!"
    end
end

"""
    kernelFermiΩ(n::Int, ω::T, β::T) where {T <: AbstractFloat}

Compute the fermionic kernel with Matsubara frequency.
```math
g(iω_n) = -1/(iω_n-ω),
```
where ``ω_n=(2n+1)π/β``. The convention here is consist with the book "Quantum Many-particle Systems" by J. Negele and H. Orland, Page 95

# Arguments
- `n`: index of the Matsubara frequency
- `ω`: energy 
- `β`: the inverse temperature 
"""
@inline function kernelFermiΩ(n::Int, ω::T, β::T=1.0) where {T <: AbstractFloat}
    # fermionic Matsurbara frequency
    ω_n = (2 * n + 1) * π / β
    G = -1.0 / (ω_n * im - ε)
    return T(G)
end

"""
    kernelBoseΩ(n::Int, ω::T, β::T) where {T <: AbstractFloat}

Compute the bosonic kernel with Matsubara frequency.
```math
g(iω_n) = -1/(iω_n-ω),
```
where ``ω_n=2nπ/β``. The convention here is consist with the book "Quantum Many-particle Systems" by J. Negele and H. Orland, Page 95

# Arguments
- `n`: index of the Matsubara frequency
- `ω`: energy 
- `β`: the inverse temperature 
"""
@inline function kernelBoseΩ(n::Int, ω::T, β::T=1.0) where {T <: AbstractFloat}
    # fermionic Matsurbara frequency
    ω_n = (2 * n) * π / β
    G = -1.0 / (ω_n * im - ε)
    return T(G)
end

"""
    density(type, ω, β=1.0)

Compute the imaginary-time kernel of different type. Assume ``k_B T/\\hbar=1``

# Arguments
- `type`: symbol :fermi, :bose
- `ω`: energy 
- `β`: the inverse temperature 
"""
@inline function density(type::Symbol, ω::T, β::T=1.0) where {T <: AbstractFloat}
    if type == :fermi
        return fermiDirac(ω, β)
    elseif type == :bose
        return boseEinstein(ω, β)
    else
        @error "Type $type  is not implemented!"
    end
end

"""
fermiDirac(ω)

Compute the Fermi Dirac function. Assume ``k_B T/\\hbar=1``
```math
f(ω) = 1/(1+e^{-ω})
```

# Arguments
- `ω`: frequency
- `β`: the inverse temperature 
"""
@inline function fermiDirac(ω::T, β::T=1.0) where {T <: AbstractFloat}
    x=ω*β
    if -T(50.0) < x < T(50.0)
    return 1.0 / (1.0 + exp(x))
    elseif x >= T(50.0)
        return exp(-x)
    else # x<=-50.0
        return 1.0 - exp(x)
    end
end

"""
boseEinstein(ω)

Compute the Fermi Dirac function. Assume ``k_B T/\\hbar=1``
```math
f(ω) = 1/(1-e^{-ω})
```

# Arguments
- `ω`: frequency
- `β`: the inverse temperature 
"""
@inline function boseEinstein(ω::T, β::T=1.0) where {T <: AbstractFloat}
    # if -eps(T)<ω<eps(T)
    #     return 0.0
    # end
    n=0.0
    x=ω*β
    if -T(50.0) < x < T(50.0)
        n=1.0 / (1.0 - exp(x))
    elseif x >= T(50.0)
        n=exp(-x)
    else # x<=-50.0
        n=1.0 - exp(x)
    end
    @assert isfinite(n)
    return n
end

"""
freq2Tau(type, spectral, τGrid, β=1.0, Emin=-Inf, Emax=Inf, rtol=1e-12)

Compute imaginary-time Green's function from a spectral density``
```math
G(τ>0) = ∫dω e^{-ωτ}/(1±e^{-ωβ}) S(ω)
G(τ≤0) = -∫dω e^{-ωτ}/(1±e^{ωβ}) S(ω)
```

# Arguments
- `ω`: frequency
"""
function freq2Tau(type, spectral, τGrid, β=1.0, Emin=-Inf, Emax=Inf, rtol=1e-12)
    G = similar(τGrid)
    err = similar(τGrid)
        for (τi, τ) in enumerate(τGrid)
        f(ω) = Spectral.kernelT(type, τ / β, ω * β) * spectral(ω)
        G[τi], err[τi] = QuadGK.quadgk(f, Emin, Emax, rtol=rtol)
    end
    return G, err
end

"""
freq2MatFreq(type, spectral, τGrid, β=1.0, Emin=-Inf, Emax=Inf, rtol=1e-12)

Compute Matubra frequency Green's function from a spectral density``
```math
G(iωn) = ∫dω -1/(iωn-ω) S(ω)
```

# Arguments
- `ω`: frequency
"""
function freq2MatFreq(type, spectral, τGrid, β=1.0, Emin=-Inf, Emax=Inf, rtol=1e-12)
    G = similar(τGrid)
    err = similar(τGrid)
        for (τi, τ) in enumerate(τGrid)
        f(ω) = Spectral.kernelΩ(type, τ / β, ω * β) * spectral(ω)
        G[τi], err[τi] = QuadGK.quadgk(f, Emin, Emax, rtol=rtol)
    end
    return G, err
end

end
