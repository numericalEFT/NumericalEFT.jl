"""
    createIdx!(newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to generate new index (uniformly) randomly in [1, size]

# Arguments
- `newIdx`:  index ∈ [1, size]
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline function createIdx!(newIdx::Int, size::Int, rng = RNG)
    newIdx = rand(rng, 1:size)
    return size * 1.0
end

"""
    removeIdx!(newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to remove the old index in [1, size]

# Arguments
- `oldIdx`:  index ∈ [1, size]
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline removeIdx(oldIdx::Int, size::Int, rng = RNG) = 1.0 / size

"""
    shiftIdx!(oldIdx::Int, newIdx::Int, size::Int, rng=GLOBAL_RNG)

Propose to shift the old index in [1, size] to a new index

# Arguments
- `oldIdx`:  old index ∈ [1, size]
- `newIdx`:  new index ∈ [1, size], will be modified!
- `size` : up limit of the index
- `rng=GLOBAL_RNG` : random number generator
"""
@inline function shiftIdx!(oldIdx::Int, newIdx::Int, size::Int, rng = RNG)
    newIdx = rand(rng, 1:size)
    return 1.0
end

"""
    createT!(newT, β=1.0, rng=GLOBAL_RNG)

Propose to generate new tau (uniformly) randomly in [0, β)

# Arguments
- `newT`:  index ∈ [0, β)
- `β=1.0`:  inverse temperature
"""
@inline function createT!(newT::T, β::T = T(1), rng = RNG) where {T<:AbstractFloat}
    newT = rand(rng) * β
    return β
end

"""
    removeT(oldT, β=1.0, rng=GLOBAL_RNG)

Propose to remove old tau in [0, β)
"""
@inline function removeT(oldT::T, β::T = T(1), rng = RNG) where {T<:AbstractFloat}
    return T(1) / β
end

"""
    shiftT!(oldT, newT, β=1.0, rng=GLOBAL_RNG)

Propose to shift the old tau to new tau, both in [0, β)

# Arguments
- `newT`:  will be modified!
"""
@inline function shiftT!(
    oldT::T,
    newT::T,
    step::T,
    β::T = T(1),
    rng = RNG,
) where {T<:AbstractFloat}
    newT = oldT + 2 * step * (rand(rng) - T(0.5))
    if newT < T(0.0)
        newT += β
    elseif newT > β
        newT -= β
    end
    return T(1.0)
end

"""
    shiftT_flip!(oldT, newT, β=1.0, rng=GLOBAL_RNG)

Propose to flip the old tau to a new tau, both in [0, β)

# Arguments
- `newT`:  will be modified!
"""
@inline function shiftT_flip!(
    oldT::T,
    newT::T,
    β::T = T(1),
    rng = RNG,
) where {T<:AbstractFloat}
    newT = β - oldT
    return T(1.0)
end

"""
    createFermiK!(newK, Kf=1.0, δK=0.5, rng=GLOBAL_RNG)

Propose to generate new Fermi K in [Kf-δK, Kf+δK)

# Arguments
- `newK`:  vector of dimension of d=2 or 3
"""
@inline function createFermiK!(newK, Kf = 1.0, δK = 0.5, rng = RNG)
    ############ Simple Way ########################
    # for i in 1:DIM
    #     newK[i] = Kf * (rand(rng) - 0.5) * 2.0
    # end
    # return (2.0 * Kf)^DIM
    ################################################

    Kamp = Kf + (rand(rng) - 0.5) * 2.0 * δK
    Kamp <= 0.0 && return 0.0
    # Kf-dK<Kamp<Kf+dK 
    ϕ = 2π * rand(rng)
    if length(newK) == 3 #dimension 3
        θ = π * rand(rng)
        # newK .= Kamp .* Mom(cos(ϕ) * sin(θ), sin(ϕ) * sin(θ), cos(θ))
        newK[1] = Kamp * cos(ϕ) * sin(θ)
        newK[2] = Kamp * sin(ϕ) * sin(θ)
        newK[3] = Kamp * cos(θ)
        return 2δK * 2π * π * (sin(θ) * Kamp^2)
        # prop density of KAmp in [Kf-dK, Kf+dK), prop density of Phi
        # prop density of Theta, Jacobian
    else  # DIM==2
        newK[1] = Kamp * cos(ϕ)
        newK[2] = Kamp * sin(ϕ)
        return 2δK * 2π * Kamp
        # prop density of KAmp in [Kf-dK, Kf+dK), prop density of Phi, Jacobian
    end
end

"""
    removeFermiK!(oldK, Kf=1.0, δK=0.5, rng=GLOBAL_RNG)

Propose to remove an existing Fermi K in [Kf-δK, Kf+δK)

# Arguments
- `oldK`:  vector of dimension of d=2 or 3
"""
@inline function removeFermiK(oldK, Kf = 1.0, δK = 0.5, rng = RNG)
    ############## Simple Way #########################
    # for i in 1:DIM
    #     if abs(oldK[i]) > Kf
    #         return 0.0
    #     end
    # end
    # return 1.0 / (2.0 * Kf)^DIM
    ####################################################

    δK = Kf / 2.0
    Kamp = sqrt(dot(oldK, oldK))
    if !(Kf - δK < Kamp < Kf + δK)
        return 0.0
    end
    # (Kamp < Kf - dK || Kamp > Kf + dK) && return 0.0
    if length(oldK) == 3 #dimension 3
        sinθ = sqrt(oldK[1]^2 + oldK[2]^2) / Kamp
        sinθ < 1.0e-15 && return 0.0
        return 1.0 / (2δK * 2π * π * sinθ * Kamp^2)
    else  # DIM==2
        return 1.0 / (2δK * 2π * Kamp)
    end
end

"""
    shiftK_radial!(oldK, newK, λ=1.5, rng=GLOBAL_RNG)

Propose to shift oldK to newK. Work for generic momentum vector
# Arguments
- `newK`:  randomly proposed in [oldK/λ, oldK*λ)
"""
@inline function shiftK_radial!(oldK, newK, λ = 1.5, rng = RNG)
    ratio = 1.0 / λ + rand(rng) * (λ - 1.0 / λ)
    newK .= oldK .* ratio
    DIM = length(oldK)
    return (DIM == 2) ? 1.0 : ratio
end

"""
    shiftK!(oldK, newK, step, rng=GLOBAL_RNG)

Propose to shift oldK to newK. Work for generic momentum vector
"""
@inline function shiftK!(oldK, newK, step, rng = RNG)
    DIM = length(oldK)
    newK .= oldK .+ (rand(rng, DIM) .- 0.5) .* step
    return 1.0
end

"""
    shiftK_flip!(oldK, newK)

Propose to flip oldK to newK. Work for generic momentum vector
"""
@inline function shiftK_flip!(oldK, newK)
    newK .= oldK .* (-1.0)
    return 1.0
end

"""
    create!(new::Tau, rng=GLOBAL_RNG)

Propose to generate new tau (uniformly) randomly in [0, β), return proposal probability
"""
@inline function create!(T::Tau, idx::Int, rng = RNG)
    T.t[idx] = rand(rng) * T.β
    return β
end

"""
    remove(old::Tau, rng=GLOBAL_RNG)

Propose to remove old tau in [0, β), return proposal probability
"""
@inline function remove(T::Tau, idx::Int, ng = RNG)
    return T(1) / β
end

"""
    shiftT!(oldT, newT, β=1.0, rng=GLOBAL_RNG)

Propose to shift the old tau to new tau, both in [0, β)

# Arguments
- `newT`:  will be modified!
"""
@inline function shift!(T::Tau, idx::Int, rng = RNG)
    x=rand(rng)
    if x<1.0/3
        T.t[idx] = T.t[idx] + 2 * T.λ * (rand(rng) - T(0.5))
    elseif x<2.0/3
        T.t[idx]= T.β - T.t[idx]
    else
        T.t[idx] = rand(rng) * T.β
    end

    if T.t[idx] < T(0.0)
        T.t[idx] += β
    elseif newT > β
        T.t[idx] -= β
    end

    return T(1.0)
end

"""
    shiftT_flip!(oldT, newT, β=1.0, rng=GLOBAL_RNG)

Propose to flip the old tau to a new tau, both in [0, β)

# Arguments
- `newT`:  will be modified!
"""
@inline function shiftT_flip!(
    oldT::T,
    newT::T,
    β::T = T(1),
    rng = RNG,
) where {T<:AbstractFloat}
    newT = β - oldT
    return T(1.0)
end