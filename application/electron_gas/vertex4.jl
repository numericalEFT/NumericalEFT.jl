# This example demonstrated how to calculate the bubble diagram of free electrons using the Monte Carlo module

using Distributed

const Ncpu = 8 
const totalStep = 1e7
const Repeat = 8

addprocs(Ncpu)

@everywhere using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, Statistics, BenchmarkTools, InteractiveUtils, Parameters
@everywhere include("RPA.jl")

# claim all globals to be constant, otherwise, global variables could impact the efficiency
@everywhere const kF, m, e, AngSize = 1.919, 0.5, sqrt(2), 32
@everywhere const β = 25.0 / kF^2
@everywhere const mass2 = 0.001
@everywhere const n = 0 # external Matsubara frequency
@everywhere const extAngle = collect(LinRange(0.0, π, AngSize))
@everywhere const qgrid = Grid.boseK(kF, 3kF, 0.2kF, 32) 
@everywhere const τgrid = Grid.tau(β, EF, 128)
@everywhere const vqinv = [(q^2 + mass2) / (4π * e^2) for q in qgrid]
@everywhere const dW0 = dWRPA(vqinv, qgrid, τgrid, kF, β, 2, m) # dynamic part of the effective interaction
@everywhere const obs1, obs2 = zeros(Float64, AngSize), zeros(Float64, AngSize)
@everywhere const Weight = SVector{2,Float64}
@everywhere const KInL = [kF, 0.0, 0.0] # incoming momentum of the left particle
@everywhere const Qd = [0.0, 0.0, 0.0] # transfer momentum is zero in the forward scattering channel

@everywhere function integrand(config)
    if config.curr.id == 1
        return 1.0
    elseif config.curr.id == 2
            return eval2(config)
        else
            return 0.0
    end
end

@everywhere function interaction(qd, qe, τIn, τOut)
    dτ = abs(τOut - τIn)

    kDiQ = FastMath.norm(qd)
    vd = -4π * e^2 / (kDiQ^2 + mass2)
    wd = -vd * Grid.linear2D(dW0, qgrid, τgrid, kDiQ, dτ) # dynamic interaction, don't forget the singular factor vq

    kExQ = FastMath.norm(qe)
    ve = 4π * e^2 / (kEiQ^2 + mass2)
    we = ve * Grid.linear2D(dW0, qgrid, τgrid, kEiQ, dτ) # dynamic interaction, don't forget the singular factor vq

    return Weight(vd / β, ve / β), Weight(wd, we)
end

"""
      KInL                      KOutL
       |                         | 
       ↑                t2.L     ↑ 
       |-------------->----------|
       |       |    k1    |      |
       |   L   |          |  R   |
       |       |    k2    |      |
       |--------------<----------|
  t1.L ↑    t1.R                 ↑ t2.R
       |                         | 
      KInL                      KOutL
"""
@everywhere function eval2(config)
    T, K, Ang = config.var[1], config.var[2], config.var[3]
    # In case the compiler is too stupid, it is a good idea to explicitly specify the type here
    k1 = K[1]
    t1, t2 = T[1], T[2]
    θ = extAngle[Ang[1]] # external momentum
    KInR = [kF * cos(θ), kF * sin(θ), 0.0]

    k2 = k1 - Qd
    vld, vle, wld, wle = interaction(Qd, KInL - k1, t1.L, t1.R)
    vrd, vre, wrd, wre = interaction(Qd, KInR - k2, t2.L, t2.R)

    ϵ1, ϵ2 = (dot(k1, k1) - kF^2) / (2m), (dot(k2, k2) - kF^2) / (2m) 
    wd, we = 0.0, 0.0

    # possible green's function on the top
    gu1 = Spectral.kernelFermiT(t2.L - t1.L, ϵ1, β)
    gu2 = Spectral.kernelFermiT(t2.L - t1.R, ϵ1, β)

    # possible green's function on the down
    gd1 = Spectral.kernelFermiT(t1.L - t2.L, ϵ2, β)
    gd2 = Spectral.kernelFermiT(t1.L - t2.R, ϵ2, β)

    gd31 = Spectral.kernelFermiT(t[1] - t[3], ϵ2)
    gd41 = Spectral.kernelFermiT(t[1] - t[4], ϵ2)
    gd32 = Spectral.kernelFermiT(t[2] - t[3], ϵ2)
    gd42 = Spectral.kernelFermiT(t[2] - t[4], ϵ2)

    kq = k + q
    τ = (Tout - Tin) / β
    ω1 = (dot(k, k) - kF^2) * β
    g1 = Spectral.kernelFermiT(τ, ω1)
    ω2 = (dot(kq, kq) - kF^2) * β
    g2 = Spectral.kernelFermiT(-τ, ω2)
    spin = 2
    phase = 1.0 / (2π)^3
    return g1 * g2 * spin * phase * cos(2π * n * τ)
    # return g1 * g2 * spin * phase
end

@everywhere function measure(config)
    diag = config.curr
    factor = 1.0 / diag.reWeightFactor
    extidx = config.var[3][1]
    if diag.id == 1
        obs1[extidx] += factor
    elseif diag.id == 2
        weight = integrand(config)
        obs2[extidx] += weight / abs(weight) * factor
    else
        return
    end
end

@everywhere function MC(totalStep, pid)
    rng = MersenneTwister(pid)

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.Discrete(1, length(extQ)) # external variable is specified
    diag1 = MonteCarlo.Diagram(1, 0, [1, 0, 1])
    diag2 = MonteCarlo.Diagram(2, 1, [2, 1, 1])

    config = MonteCarlo.Configuration(totalStep, (diag1, diag2), (T, K, Ext); pid=pid, rng=rng)
    MonteCarlo.montecarlo(config, integrand, measure)

    return obs2 / sum(obs1) * Ext.size
end

function run(repeat, totalStep)
    if Ncpu > 1
        observable = pmap((x) -> MC(totalStep, rand(1:10000)), 1:repeat)
    else
        observable = map((x) -> MC(totalStep, rand(1:10000)), 1:repeat)
    end

    obs = mean(observable)
    obserr = std(observable) / sqrt(length(observable))

    for (idx, q) in enumerate(extQ)
        q = q[1]
        p, err = TwoPoint.LindhardΩnFiniteTemperature(3, q, n, kF, β, m, 2)
        @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, obs[idx], obserr[idx], p, err)
    end
end

# @btime run(1, 10)
run(Repeat, totalStep)
# @time run(Repeat, totalStep)