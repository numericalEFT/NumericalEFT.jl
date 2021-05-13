"""
    createIdx!(newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to generate new index (uniformly) randomly in [1, size]

# Arguments
- `newIdx`:  index ∈ [1, size]
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline function create!(d::Discrete, idx::Int, config)
    (idx >= length(d.data) - 1) && error("$idx overflow!")
    d[idx] = rand(config.rng, d.lower:d.upper)
    return Float64(d.upper - d.lower + 1) # lower:upper has upper-lower+1 elements!
end

@inline createRollback!(d::Discrete, idx::Int, config) = nothing

"""
    removeIdx!(newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to remove the old index in [1, size]

# Arguments
- `oldIdx`:  index ∈ [1, size]
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline function remove!(d::Discrete, idx::Int, config) 
    (idx >= length(d.data) - 1) && error("$idx overflow!")
    return 1.0 / Float64(d.upper - d.lower + 1)
end

@inline removeRollback!(d::Discrete, idx::Int, config) = nothing

"""
    shiftIdx!(oldIdx::Int, newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to shift the old index in [1, size] to a new index

# Arguments
- `oldIdx`:  old index ∈ [1, size]
- `newIdx`:  new index ∈ [1, size], will be modified!
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline function shift!(d::Discrete, idx::Int, config)
    (idx >= length(d.data) - 1) && error("$idx overflow!")
    d[end] = d[idx] # save the current variable
    d[idx] = rand(config.rng, d.lower:d.upper)
    return 1.0
end

@inline function shiftRollback!(d::Discrete, idx::Int, config)
    (idx >= length(d.data) - 1) && error("$idx overflow!")
    d[idx] = d[end]
end

"""
    create!(K::FermiK{D}, idx::Int, rng=GLOBAL_RNG)

Propose to generate new Fermi K in [Kf-δK, Kf+δK)

# Arguments
- `newK`:  vector of dimension of d=2 or 3
"""
function create!(K::FermiK{D}, idx::Int, config) where {D}
    (idx >= length(K.data) - 1) && error("$idx overflow!")
    rng = config.rng
    ############ Simple Way ########################
    # for i in 1:DIM
    #     newK[i] = Kf * (rand(rng) - 0.5) * 2.0
    # end
    # return (2.0 * Kf)^DIM
    ################################################

    Kamp = K.kF + (rand(rng) - 0.5) * 2.0 * K.δk
    (Kamp <= 0.0) && return 0.0
    # Kf-dK<Kamp<Kf+dK 
    ϕ = 2π * rand(rng)
    if D == 3 # dimension 3
        θ = π * rand(rng)
        # newK .= Kamp .* Mom(cos(ϕ) * sin(θ), sin(ϕ) * sin(θ), cos(θ))
        K[idx] = @SVector [Kamp * cos(ϕ) * sin(θ), Kamp * sin(ϕ) * sin(θ), Kamp * cos(θ)]
        return 2 * K.δk * 2π * π * (sin(θ) * Kamp^2)
        # prop density of KAmp in [Kf-dK, Kf+dK), prop density of Phi
        # prop density of Theta, Jacobian
    else  # DIM==2
        K[idx] = @SVector [Kamp * cos(ϕ), Kamp * sin(ϕ)]
        return 2 * K.δk * 2π * Kamp
        # prop density of KAmp in [Kf-dK, Kf+dK), prop density of Phi, Jacobian
    end
end
createRollback!(K::FermiK{D}, idx::Int, config) where {D} = nothing

"""
    removeFermiK!(oldK, Kf=1.0, δK=0.5, rng=GLOBAL_RNG)

Propose to remove an existing Fermi K in [Kf-δK, Kf+δK)

# Arguments
- `oldK`:  vector of dimension of d=2 or 3
"""
function remove!(K::FermiK{D}, idx::Int, config) where {D}
    (idx >= length(K.data) - 1) && error("$idx overflow!")
    ############## Simple Way #########################
    # for i in 1:DIM
    #     if abs(oldK[i]) > Kf
    #         return 0.0
    #     end
    # end
    # return 1.0 / (2.0 * Kf)^DIM
    ####################################################

    oldK = K[idx]
    Kamp = sqrt(dot(oldK, oldK))
    if !(K.kF - K.δk < Kamp < K.kF + K.δk)
        return 0.0
    end
    # (Kamp < Kf - dK || Kamp > Kf + dK) && return 0.0
    if D == 3 # dimension 3
        sinθ = sqrt(oldK[1]^2 + oldK[2]^2) / Kamp
        sinθ < 1.0e-15 && return 0.0
        return 1.0 / (2 * K.δk * 2π * π * sinθ * Kamp^2)
    else  # DIM==2
        return 1.0 / (2 * K.δk * 2π * Kamp)
end
end

removeRollback!(K::FermiK{D}, idx::Int, config) where {D} = nothing

"""
    shiftK!(oldK, newK, step, rng=GLOBAL_RNG)

Propose to shift oldK to newK. Work for generic momentum vector
"""
function shift!(K::FermiK{D}, idx::Int, config) where {D}
    (idx >= length(K.data) - 1) && error("$idx overflow!")
    K[end] = K[idx]  # save current K

    rng = config.rng
    x = rand(rng)
    if x < 1.0 / 3
        λ = 1.5
        ratio = 1.0 / λ + rand(rng) * (λ - 1.0 / λ)
        K[idx] = K[idx] * ratio
        return (D == 2) ? 1.0 : ratio
    elseif x < 2.0 / 3
        ϕ = rand(rng) * 2π
        if (D == 3)
            # sample uniformly on sphere, check http://corysimon.github.io/articles/uniformdistn-on-sphere/ 
            θ = acos(1 - 2 * rand(rng))
            Kamp = sqrt(K[idx][1]^2 + K[idx][2]^2 + K[idx][3]^2)
            K[idx] = @SVector [Kamp * cos(ϕ) * sin(θ), Kamp * sin(ϕ) * sin(θ), Kamp * cos(θ)]
            return 1.0 
        else # D=2
            Kamp = sqrt(K[idx][1]^2 + K[idx][2]^2)
            K = @SVector [Kamp * cos(ϕ), Kamp * sin(ϕ)]
            return 1.0
        end
    else
        Kc, dk = K[idx], K.δk
        if (D == 3)
            K[idx] = @SVector [Kc[1] + (rand(rng) - 0.5) * dk, Kc[2] + (rand(rng) - 0.5) * dk, Kc[3] + (rand(rng) - 0.5) * dk]
        else # D=2
            K[idx] = @SVector [Kc[1] + (rand(rng) - 0.5) * dk, Kc[2] + (rand(rng) - 0.5) * dk]
        end
        # K[idx] += (rand(rng, D) .- 0.5) .* K.δk
        return 1.0
    end
end

function shiftRollback!(K::FermiK{D}, idx::Int, config) where {D}
    (idx >= length(K.data) - 1) && error("$idx overflow!")
    K[idx] = K[end]
end

"""
    create!(T::Tau, idx::Int, rng=GLOBAL_RNG)

Propose to generate new tau (uniformly) randomly in [0, β), return proposal probability

# Arguments
- `T`:  Tau variable
- `idx`: T.data[idx] will be updated
"""
@inline function create!(T::Tau, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    T[idx] = rand(config.rng) * T.β
    return T.β
end
@inline createRollback!(T::Tau, idx::Int, config) = nothing

"""
    remove(T::Tau, idx::Int, rng=GLOBAL_RNG)

Propose to remove old tau in [0, β), return proposal probability

# Arguments
- `T`:  Tau variable
- `idx`: T.data[idx] will be updated
"""
@inline function remove!(T::Tau, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    return 1.0 / T.β
end
@inline removeRollback!(T::Tau, idx::Int, config) = nothing

"""
    shift!(T::Tau, idx::Int, rng=GLOBAL_RNG)

Propose to shift an existing tau to a new tau, both in [0, β), return proposal probability

# Arguments
- `T`:  Tau variable
- `idx`: T.data[idx] will be updated
"""
@inline function shift!(T::Tau, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    T[end] = T[idx]
    rng = config.rng
    x = rand(rng)
    if x < 1.0 / 3
        T[idx] = T[idx] + 2 * T.λ * (rand(rng) - 0.5)
    elseif x < 2.0 / 3
        T[idx] = T.β - T[idx]
    else
    T[idx] = rand(rng) * T.β
    end

    if T[idx] < 0.0
        T[idx] += T.β
    elseif T[idx] > T.β
        T[idx] -= T.β
end

return 1.0
end

@inline function shiftRollback!(T::Tau, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
T[idx] = T[end]
end

"""
    create!(T::TauPair, idx::Int, rng=GLOBAL_RNG)

Propose to generate a new pair of tau (uniformly) randomly in [0, β), return proposal probability

# Arguments
- `T`:  TauPair variable
- `idx`: T.data[idx] will be updated
"""
@inline function create!(T::TauPair, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    rng = config.rng
    T[idx][1] = rand(rng) * T.β
    T[idx][2] = rand(rng) * T.β
    return T.β * T.β
end

@inline createRollback!(T::TauPair, idx::Int, config) = nothing

"""
    remove(T::TauPair, idx::Int, rng=GLOBAL_RNG)

Propose to remove an existing pair of tau in [0, β), return proposal probability

# Arguments
- `T`:  Tau variable
- `idx`: T.data[idx] will be updated
"""
@inline function remove!(T::TauPair, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    return 1.0 / T.β / T.β
end
@inline removeRollback!(T::TauPair, idx::Int, config) = nothing

"""
    shift!(T::TauPair, idx::Int, rng=GLOBAL_RNG)

Propose to shift an existing tau pair to a new tau pair, both in [0, β), return proposal probability

# Arguments
- `T`:  Tau variable
- `idx`: T.t[idx] will be updated
"""
@inline function shift!(T::TauPair, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    T[end] .= T[idx]
    rng = config.rng
    x = rand(rng)
    if x < 1.0 / 3
        T[idx][1] += 2 * T.λ * (rand(rng) - 0.5)
        T[idx][2] += 2 * T.λ * (rand(rng) - 0.5)
    elseif x < 2.0 / 3
        T[idx][1] = T.β - T[idx][1]
        T[idx][2] = T.β - T[idx][2]
    else
        T[idx][1] = rand(rng) * T.β
    T[idx][2] = rand(rng) * T.β
    end

    if T[idx][1] < 0.0
    T[idx][1] += T.β
    elseif T[idx][1] > T.β
    T[idx][1] -= T.β
    end

    if T[idx][2] < 0.0
    T[idx][2] += T.β
    elseif T[idx][2] > T.β
    T[idx][2] -= T.β
end

    return 1.0
# return 0.0
end

@inline function shiftRollback!(T::TauPair, idx::Int, config)
    (idx >= length(T.data) - 1) && error("$idx overflow!")
    T[idx] .= T[end]
end

"""
    create!(theta::Angle, idx::Int, rng=GLOBAL_RNG)

Propose to generate new angle (uniformly) randomly in [0, 2π), return proposal probability

# Arguments
- `theta`:  angle variable
- `idx`: theta.t[idx] will be updated
"""
@inline function create!(theta::Angle, idx::Int, config)
    (idx >= length(theta.data) - 1) && error("$idx overflow!")
    theta[idx] = rand(config.rng) * 2π
return 2π
end
@inline createRollback!(theta::Angle, idx::Int, config) = nothing


"""
    remove(theta::Angle, idx::Int, rng=GLOBAL_RNG)

Propose to remove old theta in [0, 2π), return proposal probability

# Arguments
    - `theta`:  Tau variable
- `idx`: theta.t[idx] will be updated
"""
@inline function remove!(theta::Angle, idx::Int, config)
    (idx >= length(theta.data) - 1) && error("$idx overflow!")
    return 1.0 / 2.0 / π
end
@inline removeRollback!(theta::Angle, idx::Int, config) = nothing

"""
    shift!(theta::Angle, idx::Int, rng=GLOBAL_RNG)

Propose to shift the old theta to new theta, both in [0, 2π), return proposal probability

# Arguments
- `theta`:  angle variable
- `idx`: theta.t[idx] will be updated
"""
@inline function shift!(theta::Angle, idx::Int, config)
    (idx >= length(theta.data) - 1) && error("$idx overflow!")
    theta[end] = theta[idx]
    rng = config.rng
    x = rand(rng)
    if x < 1.0 / 3
        theta[idx] = theta[idx] + 2 * theta.λ * (rand(rng) - 0.5)
    elseif x < 2.0 / 3
        theta[idx] = 2π - theta[idx]
    else
        theta[idx] = rand(rng) * 2π
    end

    if theta[idx] < 0.0
    theta[idx] += 2π
    elseif theta[idx] > 2π
    theta[idx] -= 2π
    end

    return 1.0
end

@inline function shiftRollback!(theta::Angle, idx::Int, config)
    (idx >= length(theta.data) - 1) && error("$idx overflow!")
    theta[idx] = theta[end]
end
