module Green
    export bareFermi

"""
    bareFermi(β, τ, ε, [, scale])

Compute the bare fermionic Green's function. Assume ``k_B=\\hbar=1``
```math
g(τ>0) = e^{-ετ}/(1+e^{-βε}), g(τ≤0) = -e^{-ετ}/(1+e^{βε})
```

# Arguments
- `β`: the inverse temperature 
- `τ`: the imaginary time, must be (-β, β]
- `ε`: dispersion minus chemical potential: ``E_k-μ``
"""
@inline function bareFermi(β::T, τ::T, ε::T) where {T<:AbstractFloat}
    (-β < τ <= β) || error("τ must be (-β, β]")
    if τ == T(0.0)
        τ = -eps(T)
    end
    G = sign(τ)
    if τ < T(0.0)
        τ += β
    end
    x = β * ε / 2
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
end