"""
Define diagrams
"""
module Diagram
    include("green.jl")
    using .Green
    using Cuba

"""
    Polar0(q, n, kF, β=1)

Compute the polarization function of free electrons in the Matsubara frequency. Assume ``k_B T=1`` to the energy scale.

# Arguments
- `q`: external momentum, q<1e-4 will be treated as q=0 
- `n`: the externel Matsubara frequency
- `kF`: the Fermi momentum 

"""
# @inline function Polar0(q::T, n::T, kF::T, β::T, Dim::Int) where {T <: AbstractFloat}
@inline function Polar0(q, n, kF, β)
    ω_n = 2 * n * π / β
    kp2(k, θ) = (q + k * cos(θ))^2 + (k * sin(θ))^2

    # if β*q is too small, replace it with (df/dq)/(dϵ/dq)
    polar(k, θ) =  q >= 1e-4 ? (FermiDirac(β, k^2 - kF^2) - FermiDirac(β,  kp2(k, θ) - kF^2)) / (ω_n * 1im + k^2 - kp2(k, θ)) / (2.0 * π)^2 * sin(θ) * k^2 : (n == 0 ? 0.0 + 0.0im : (1.0 - FermiDirac(β, k^2 - kF^2)) * FermiDirac(β, k^2 - kF^2) * β * sin(θ) * k^2 / (2.0 * π)^2 + 0im)

    # if β*q is too small, replace it with (df/dq)/(dϵ/dq)
    # polar(k, θ) = (FermiDirac(β, k^2 - kF^2) - FermiDirac(β,  kp2(k, θ) - kF^2)) / (ω_n * 1im + k^2 - kp2(k, θ)) / (2.0 * π)^2 * sin(θ) * k^2 
    # function integrand(x, f)
    #     # x[1]:k, x[2]: θ
    #     f[1] = polar(x[1] / (1 - x[1]), x[2] * π) / (1 - x[1])^2 * π
    # end
    # println(polar(1.919, 0.0))

    function integrand1(x, f)
        # x[1]:k, x[2]: θ
        f[1], f[2] = reim(polar(kF + x[1] / (1 - x[1]), x[2] * π) / (1 - x[1])^2 * π)
        # f[1] = (polar(kF + x[1] / (1 - x[1]), x[2] * π) / (1 - x[1])^2 * π)
    end
    
    function integrand2(x, f)
        # x[1]:k, x[2]: θ
        f[1], f[2] = reim(polar(x[1] * kF, x[2] * π) * kF * π)
        # f[1] = (polar(x[1] * kF, x[2] * π) * kF * π)
    end

    result1, err1 = Cuba.cuhre(integrand1, 2, 2, atol=1e-12, rtol=1e-10);
    result2, err2 = Cuba.cuhre(integrand2, 2, 2, atol=1e-12, rtol=1e-10);

    return complex((result1 + result2)[1]...), complex((err1 + err2)[1]...)
end

end
