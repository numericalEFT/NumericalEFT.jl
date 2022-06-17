"""
(1-exp(-Λ*x)/x
"""
function kernel(ω::Float)
    if ω < 1e-5
        return 1 - ω / 2 + ω^2 / 6 - ω^3 / 24 + ω^4 / 120 - ω^5 / 720
    else
        # return (1 - exp(-ω)) / ω
        return -expm1(-ω) / ω
    end
end

##################### Particle-hole symmetric kernel #############################

"""
particle-hole symmetric kernel: K(ω, τ)=e^{-ω*τ}+e^{-ω*(β-τ)}

KK=int_0^{1/2} dτ K(ω1,τ)*K(ω2,τ)=(1-e^{ω1+ω2})/(ω1+ω2)+(e^{-ω2}-e^{-ω1})/(ω1-ω2)
"""
function projPH_ω(Λ::Float, ω1::Float, ω2::Float)
        if ω1 > ω2
        return kernel(ω1 + ω2) + exp(-ω2) * kernel(ω1 - ω2)
    else
        return kernel(ω1 + ω2) + exp(-ω1) * kernel(ω2 - ω1)
    end
end

"""
particle-hole symmetric kernel: K(ω, τ)=e^{-ω*τ}+e^{-ω*(β-τ)}

KK=int_0^{Λ} dτ K(ω,t1)*K(ω2,t2)=(1-e^{t1+t2})/(t1+t2)+(1-e^{2β-t1-t2})/(2β-t1-t2)+(1-e^{β+t1-t2})/(β+t1-t2)+(1-e^{β-t1+t2})/(β-t1+t2)
"""
function projPH_τ(Λ::Float, t1::Float, t2::Float)
    # note that here Λ = \beta/2
    return kernel(t1 + t2) + kernel(4 * Λ - t1 - t2) + kernel(2 * Λ - t1 + t2) + kernel(2 * Λ + t1 - t2)
end

##################### Particle-hole asymmetric kernel #############################
"""
particle=hole asymmetric kernel: K(ω, τ)=e^{-ω*τ}-e^{-ω*(β-τ)}

KK=int_0^{1/2} dτ K(ω1,τ)*K(ω2,τ)=(1-e^{ω1+ω2})/(ω1+ω2)-(e^{-ω2}-e^{-ω1})/(ω1-ω2)
"""
function projPHA_ω(Λ::Float, ω1::Float, ω2::Float)
    if ω1 > ω2
    return kernel(ω1 + ω2) - exp(-ω2) * kernel(ω1 - ω2)
    else
    return kernel(ω1 + ω2) - exp(-ω1) * kernel(ω2 - ω1)
    end
end

"""
particle-hole asymmetric kernel: K(ω, τ)=e^{-ω*τ}-e^{-ω*(β-τ)}

KK=int_0^{Λ} dτ K(ω,t1)*K(ω2,t2)=(1-e^{t1+t2})/(t1+t2)+(1-e^{2β-t1-t2})/(2β-t1-t2)-(1-e^{β+t1-t2})/(β+t1-t2)-(1-e^{β-t1+t2})/(β-t1+t2)
"""
function projPHA_τ(Λ::Float, t1::Float, t2::Float)
    return kernel(t1 + t2) + kernel(4 * Λ - t1 - t2) - kernel(2 * Λ - t1 + t2) - kernel(2 * Λ + t1 - t2)
end