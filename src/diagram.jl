"""
Calculator for some simple diagrams
"""
module Diagram
    include("green.jl")
    include("spectral.jl")
    using .Green
    using .Spectral
    using Cuba

"""
    Polar0(q, ω, dim, kF, β)

Compute the polarization function of free electrons at a given frequency. 

# Arguments
- `q`: external momentum, q<1e-4 will be treated as q=0 
- `ω`: externel frequency, make sure Im ω>0
- `dim`: dimension
- `kF=1.0`: the Fermi momentum 
- `β=1.0`: the inverse temperature
- `eps=1.0e-6`: the required absolute accuracy
"""
@inline function Polar0(q::T, ω, dim::Int, kF=T(1.0), β=T(1.0), eps=T(1.0e-6)) where {T <: AbstractFloat} 
    ω, q = ω * β, q / kF # make everything dimensionless 
    if (ω != 0.0 && imag(ω) < eps)
        println("Im ω>eps is expected unless ω=0!")
    end
    
    kp2(k, θ) = (q + k * cos(θ))^2 + (k * sin(θ))^2
    
    function polar(k, θ)
        ϵ1 = (k^2 - kF^2) * β
        ϵ2 = (kp2(k, θ) - kF^2) * β
        δϵ = ϵ1 - ϵ2
        Jacobi = (dim == 3 ? T(2π) * k^2 * sin(θ) : k^2 * sin(θ))
        Phase = T(1.0) / (2π)^dim

        if ((abs(ω) < abs(δϵ) && q < T(1.0e-4)) || (abs(ω) < eps && abs(δϵ) < eps))
            p = -(T(1) + δϵ / 2) * fermiDirac(ϵ1) * (T(1) - fermiDirac(ϵ2)) * β * Jacobi * Phase + 0.0 * 1im
        else
            p = (fermiDirac(ϵ1) - fermiDirac(ϵ2)) / (ω + δϵ)  * β * Jacobi * Phase
        end

        if isnan(p)
            println("ω=$ω, q=$q, k=$k leads to NaN!")
        end
        # println(p)
        return p
    end

    # if β*q is too small, replace it with (df/dq)/(dϵ/dq)
    # polar(k, θ) = (FermiDirac(β, k^2 - kF^2) - FermiDirac(β,  kp2(k, θ) - kF^2)) / (ω_n * 1im + k^2 - kp2(k, θ)) / (2.0 * π)^2 * sin(θ) * k^2 
    # function integrand(x, f)
    #     # x[1]:k, x[2]: θ
    #     f[1] = polar(x[1] / (1 - x[1]), x[2] * π) / (1 - x[1])^2 * π
    # end
    # println(polar(1.919, 0.0))
    
    function integrand1(x, f)
        # x[1]:k, x[2]: θ
        phase = (dim == 3 ? π : 2π)
        f[1], f[2] = reim(polar(kF + x[1] / (1 - x[1]), x[2] * phase) / (1 - x[1])^2 * phase)
    end
    
    function integrand2(x, f)
        # x[1]:k, x[2]: θ
        phase = (dim == 3 ? π : 2π)
        f[1], f[2] = reim(polar(x[1] * kF, x[2] * phase) * kF * phase)
    end
    
    result1, err1 = Cuba.cuhre(integrand1, 2, 2, atol=eps * 1e-3);
    result2, err2 = Cuba.cuhre(integrand2, 2, 2, atol=eps * 1e-3);

    # println(complex(result1[1], result1[2]))
    # println(complex(result2[1]...))
    result = result1 + result2
    err = err1 + err2
    
    return complex(result[1], result[2]), complex(err[1], err[2])
end
            
end
