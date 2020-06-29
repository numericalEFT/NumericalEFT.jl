"""
Provide N-body response and correlation functions
"""
module Green
    export bareFermi, bareFermiMatsubara
    include("fastmath.jl")
    using .FastMath

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
       it could also be the real frequency ω if the bare Green's function is used as the kernel in the Lehmann representation 
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

"""
calcualte with a given momentum vector and the chemical potential μ, rotation symmetry is assumed.
"""
@inline function bareFermi(β::T, τ::T, k::AbstractVector{T}, μ::T) where {T<:AbstractFloat}
    return bareFermi(β, τ, FastMath.squaredNorm(k)-μ)
end

"""
    bareFermiMatsubara(β, n, ε, [, scale])

Compute the bare Green's function for a given Matsubara frequency.
```math
g(iω_n) = -1/(iω_n-ε),
```
where ``ω_n=(2n+1)π/β``. The convention here is consist with the book "Quantum Many-particle Systems" by J. Negele and H. Orland, Page 95

# Arguments
- `β`: the inverse temperature 
- `τ`: the imaginary time, must be (-β, β]
- `ε`: dispersion minus chemical potential: ``E_k-μ``; 
       it could also be the real frequency ω if the bare Green's function is used as the kernel in the Lehmann representation 
"""
@inline function bareFermiMatsubara(β::T, n::Int, ε::T) where {T<:AbstractFloat}
    #fermionic Matsurbara frequency
    ω_n = (2*n+1)*π/β 
    G = -1.0/(ω_n*im-ε)
    return T(G)
end


end