"""
Calculator for some simple diagrams
"""
module Diagram
export bubble
using ..Spectral
using Cuba

"""
    bubble(dim::Int, q::T, n::Int, kF::T, β::T, m::T; ϵk=(k) -> (k^2 - kF^2) / (2m), rtol=T(1.0e-6)) where {T <: AbstractFloat}

Compute the polarization function of free electrons at a given frequency. 

# Arguments
- `dim`: dimension
- `q`: external momentum, q<1e-4 will be treated as q=0 
- `n`: externel Matsubara frequency, ωn=2π*n/β
- `kF`: Fermi momentum 
- `β`: inverse temperature
- `m`: mass
- `dispersion': dispersion, default k^2/2m-kF^2/2m
- `rtol=1.0e-6`: relative accuracy goal
"""
@inline function bubble(dim::Int, q::T, n::Int, kF::T, β::T, m::T; ϵk=(k) -> (k^2 - kF^2) / (2m), rtol=T(1.0e-6)) where {T <: AbstractFloat}
    ω = 2π * n / β
    function polar(k)
        # ϵ1 = (k^2 - kF^2) * β
        # ϵ2 = (kp2(k, θ) - kF^2) * β
        # @assert ϵ1 ≈ dispersion(k) * β
        phase = T(1.0)
        if dim == 3
            phase *= k^2 / (4π^2)
        else
            error("not implemented")
        end
        ϵ = ϵk(k) * β
        p = phase * fermiDirac(ϵ) * m / k / q * log(((q^2 - 2k * q)^2 + 4m^2 * ω^2) / ((q^2 + 2k * q)^2 + 4m^2 * ω^2))

        if isnan(p)
            println("ω=$ω, q=$q, k=$k leads to NaN!")
        end
        # println(p)
    return p
    end

    function integrand1(x, f)
        # x[1]:k
        f[1] = polar(kF + x[1] / (1 - x[1])) / (1 - x[1])^2
    end

    function integrand2(x, f)
        # x[1]:k
        f[1] = polar(x[1] * kF) * kF
    end

    function integrand(x, f)
        # x[1]:k
        f[1] = polar(x[1] / (1 - x[1])) / (1 - x[1])^2
    end

    # result1, err1 = Cuba.cuhre(integrand1, 2, 2, atol=eps * 1e-3)
    # result2, err2 = Cuba.cuhre(integrand2, 2, 2, atol=eps * 1e-3)

    result, err = Cuba.cuhre(integrand, 2, 1, rtol=rtol)
    # result, err = Cuba.vegas(integrand, 2, 1, rtol=eps)

    # result1, err1 = Cuba.vegas(integrand1, 2, 2, atol=eps * 1e-3)
    # result2, err2 = Cuba.vegas(integrand2, 2, 2, atol=eps * 1e-3)

    # println(complex(result1[1], result1[2]))
    # println(complex(result2[1]...))
    # result = result1 + result2
    # err = err1 + err2

    return result[1], err[1]
end
# @inline function bubble(dim::Int, q::T, ω::Complex{T}, kF=T(1.0), β=T(1.0), m=T(0.5), ϵk=(k) -> (k^2 - kF^2) / (2m); eps=T(1.0e-6)) where {T <: AbstractFloat}
#     # ω, q = ω * β, q / kF # make everything dimensionless 
#     if (ω * β != 0.0 && imag(ω * β) < eps)
#         println("Im ω>eps is expected unless ω=0!")
#     end
    
#     kp2(k, θ) = (q + k * cos(θ))^2 + (k * sin(θ))^2
    
#     function polar(k, θ)
#         # ϵ1 = (k^2 - kF^2) * β
#         # ϵ2 = (kp2(k, θ) - kF^2) * β
#         # @assert ϵ1 ≈ dispersion(k) * β
#         ϵ1 = ϵk(k) * β
#         ϵ2 = ϵk(sqrt(kp2(k, θ))) * β
#         δϵ = ϵ1 - ϵ2
#         Jacobi = (dim == 3 ? T(2π) * k^2 * sin(θ) : k^2 * sin(θ))
#         Phase = T(1.0) / (2π)^dim

#         if ((abs(ω * β) < abs(δϵ) && q / kF < T(1.0e-4)) || (abs(ω * β) < eps && abs(δϵ) < eps))
#             p = -(T(1) + δϵ / 2) * fermiDirac(ϵ1) * (T(1) - fermiDirac(ϵ2)) * β * Jacobi * Phase + 0.0 * 1im
#         else
#             p = (fermiDirac(ϵ1) - fermiDirac(ϵ2)) / (ω + δϵ) * β * Jacobi * Phase
#         end

#         if isnan(p)
#             println("ω=$ω, q=$q, k=$k leads to NaN!")
#         end
#         # println(p)
#     return p
#     end

#     function integrand1(x, f)
#         # x[1]:k, x[2]: θ
#         phase = (dim == 3 ? π : 2π)
#         f[1], f[2] =
#             reim(polar(kF + x[1] / (1 - x[1]), x[2] * phase) / (1 - x[1])^2 * phase)
#     end

#     function integrand2(x, f)
#         # x[1]:k, x[2]: θ
#         phase = (dim == 3 ? π : 2π)
#         f[1], f[2] = reim(polar(x[1] * kF, x[2] * phase) * kF * phase)
#     end

#     # result1, err1 = Cuba.cuhre(integrand1, 2, 2, atol=eps * 1e-3)
#     # result2, err2 = Cuba.cuhre(integrand2, 2, 2, atol=eps * 1e-3)

#     result1, err1 = Cuba.vegas(integrand1, 2, 2, atol=eps * 1e-3)
#     result2, err2 = Cuba.vegas(integrand2, 2, 2, atol=eps * 1e-3)

#     # println(complex(result1[1], result1[2]))
#     # println(complex(result2[1]...))
#     result = result1 + result2
#     err = err1 + err2

#     return complex(result[1], result[2]), complex(err[1], err[2])
# end

end
