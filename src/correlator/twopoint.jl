"""
Provide N-body response and correlation functions
"""
module TwoPoint

export freePropagatorT, freePropagatorΩ
export freePolarizationT
using ..Spectral

"""
    freePropagatorT(type, τ, ω, β)

Imaginary-time propagator.

# Arguments
- `type`: symbol :fermi, :bose
- `τ`: the imaginary time, must be (-β, β]
- `ω`: dispersion ϵ_k-μ
- `β = 1.0`: the inverse temperature 
"""
@inline function freePropagatorT(type, τ, ω, β)
    return kernelT(type, τ, ω, β)
end

"""
    freePropagatorΩ(type, n, ω, β=1.0)

Matsubara-frequency kernel of different type

# Arguments
- `type`: symbol :fermi, :bose, :corr
- `n`: index of the Matsubara frequency
- `ω`: dispersion ϵ_k-μ
- `β`: the inverse temperature 
"""
@inline function freePropagatorΩ(type, n::Int, ω, β)
    return kernelΩ(type, n, ω, β)
end

@inline function freeFermiDoS(dim, kF, m, spin)
    if dim == 3
        return spin * m * kF / 2 / π^2
    else
        error("Dimension $dim not implemented!")
        # return spin/4/π
    end
end

function LindhardΩn(dim, q, n, β, kF, m, spin)
    q < 0.0 && (q = -q) # Lindhard function is q symmetric

    q2 = q^2
    kFq = 2kF * q
    ωn = 2π * n / β
    D = 1 / (8kF * q)
    NF = freeFermiDoS(dim, kF, m, spin)

    # if ωn<=20*(q2+kFq)/(2m)
        # careful for small q or large w
    iw = ωn * im
    wmq2 = iw * 2m - q^2
    wpq2 = iw * 2m + q^2
    C1 = log(wmq2 - kFq) - log(wmq2 + kFq)
    C2 = log(wpq2 - kFq) - log(wpq2 + kFq)
    res = real(-NF / 2 * (1 - D * (wmq2^2 / q^2 - 4 * kF^2) * C1 + D * (wpq2^2 / q^2 - 4 * kF^2) * C2))
    # else
    #     b2 = q2 * ( q2 + 12/5 * kF^2 )
    #     c = 2*EF*kF*q2/(3*pi**2)
    #     res = -c/(w**2 + b2)
    # end
    return res
end
end
