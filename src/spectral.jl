"""
Spectral representation related functions
"""
module Spectral
    export kernelFermiT, kernelFermiW, kernelBoseT, kernelBoseW, fermiDirac

"""
    kernelFermiT(τ, ω)

Compute the imaginary-time fermionic kernel. Assume ``k_B T/\\hbar=1``
```math
g(τ>0) = e^{-ωτ}/(1+e^{-ω}), g(τ≤0) = -e^{-ωτ}/(1+e^{ω})
```

# Arguments
- `τ`: the imaginary time, must be (-1, 1]
- `ω`: frequency
"""
@inline function kernelFermiT(τ::T, ω::T) where {T <: AbstractFloat}
    (-T(1.0) < τ <= T(1.0)) || error("τ must be (-β, β]")
    if τ == T(0.0)
        τ = -eps(T)
    end
    G = sign(τ)
        if τ < T(0.0)
    τ += 1.0
    end
    x = ω / 2
    y = 2τ - 1
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
fermiDirac(ω)

Compute the Fermi Dirac function. Assume ``k_B T/\\hbar=1``
```math
f(ω) = 1/(1+e^{-ω})
```

# Arguments
- `ω`: frequency
"""
@inline function fermiDirac(ω::T) where {T <: AbstractFloat}
    if -T(50.0) < ω < T(50.0)
        return 1.0 / (1.0 + exp(ω))
    elseif ω >= T(50.0)
        return exp(-ω)
    else # x<=-50.0
    return 1.0 - exp(ω)
    end
end

end