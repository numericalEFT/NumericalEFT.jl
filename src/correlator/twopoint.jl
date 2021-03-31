"""
Provide N-body response and correlation functions
"""
module TwoPoint
export fermiT, fermiΩ
include("spectral.jl")
using .Spectral

"""
    fermiT(τ, ϵ, β = 1.0)

Compute the bare fermionic Green's function. Assume ``k_B=\\hbar=1``
```math
g(τ>0) = e^{-ϵτ}/(1+e^{-βϵ}), g(τ≤0) = -e^{-ϵτ}/(1+e^{βϵ})
```

# Arguments
- `τ`: the imaginary time, must be (-β, β]
- `ϵ`: dispersion minus chemical potential: ``E_k-μ``
       it could also be the real frequency ω if the bare Green's function is used as the kernel in the Lehmann representation 
- `β = 1.0`: the inverse temperature 
"""
@inline function fermiT(τ::T, ϵ::T, β::T=1.0) where {T <: AbstractFloat}
    if τ <= 0.0 || τ > eps() * β
        return kernelFermiT(τ / β, ϵ * β)
    else # 0<τ<=eps()*β
        return kernelFermiT(eps(), ϵ * β)
    end
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
@inline function fermiΩ(β::T, n::Int, ε::T) where {T <: AbstractFloat}
    # fermionic Matsurbara frequency
    ω_n = (2 * n + 1) * π / β
    G = -1.0 / (ω_n * im - ε)
    return T(G)
end

# """
#     FermiDirac(β, ε)

# Compute the Fermi Dirac function. Assume ``k_B=\\hbar=1``
# ```math
# f(ϵ) = 1/(1+e^{-βε})
# ```

# # Arguments
# - `β`: the inverse temperature 
# - `ε`: dispersion minus chemical potential: ``E_k-μ``
#        it could also be the real frequency ω if the bare Green's function is used as the kernel in the Lehmann representation 
# """
# @inline function FermiDirac(β::T, ε::T) where {T<:AbstractFloat}
#     x = β * ε
#     if -T(50.0) < x < T(50.0)
#         return 1.0 / (1.0 + exp(x))
#     elseif x >= T(50.0)
#         return exp(-x)
#     else # x<=-50.0
#         return 1.0 - exp(x)
#     end
# end


end
