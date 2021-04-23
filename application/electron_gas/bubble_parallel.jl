# This example demonstrated how to calculate the bubble diagram of free electrons using the Monte Carlo module

using Distributed
using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, BenchmarkTools, InteractiveUtils, Parameters

const Ncpu = 4
const totalStep = 1e7
const Repeat = 1

addprocs(Ncpu)

@everywhere using QuantumStatistics, Parameters, StaticArrays, Random, LinearAlgebra

@everywhere @with_kw struct Para
    kF::Float64 = 1.919
    m::Float64 = 0.5
    β::Float64 = 25.0 / kF^2
    n::Int = 0 # external Matsubara frequency
    Qsize::Int = 16
    extQ::Vector{SVector{3,Float64}} = [@SVector [q, 0.0, 0.0] for q in LinRange(0.0, 3.0 * kF, Qsize)]
end

@everywhere function integrand(config)
    if config.curr == 1
        return 1.0
    elseif config.curr == 2
            return eval2(config)
        else
            return 0.0
    end
end

@everywhere function eval2(config)
    para = config.para
    β, kF, m = para.β, para.kF, para.m

    T, K, Ext = config.var[1], config.var[2], config.var[3]
        # In case the compiler is too stupid, it is a good idea to explicitly specify the type here
    k = K[1]
    Tin, Tout = T[1], T[2] 
    extidx = Ext[1]
    q = para.extQ[extidx] # external momentum
    kq = k + q
    τ = (Tout - Tin) / β
    ω1 = (dot(k, k) - kF^2) / (2m) * β
    g1 = Spectral.kernelFermiT(τ, ω1)
    ω2 = (dot(kq, kq) - kF^2) / (2m) * β
    g2 = Spectral.kernelFermiT(-τ, ω2)
    spin = 2
    phase = 1.0 / (2π)^3
    return g1 * g2 * spin * phase * cos(2π * para.n * τ)
    # return g1 * g2 * spin * phase
end

@everywhere function measure(config)
    obs = config.obs
    factor = 1.0 / config.diagrams[config.curr].reWeightFactor
    extidx = config.var[3][1]
    if config.curr == 1
        obs[1][extidx] += factor
    elseif config.curr == 2
        weight = integrand(config)
        obs[2][extidx] += weight / abs(weight) * factor
    else
        return
    end
end

@everywhere normalize(config) = config.obs[2] / sum(config.obs[1]) * config.para.Qsize

function run(totalStep)

    para = Para()
    @unpack kF, β, extQ, Qsize = para 

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.Discrete(1, length(extQ)) # external variable is specified
    # diag1 = MonteCarlo.Diagram(1, 0, [1, 0, 1])
    # diag2 = MonteCarlo.Diagram(2, 1, [2, 1, 1])

    diag = ([1, 0, 1], [2, 1, 1]) # degrees of freedom of the normalization diagram and the bubble
    obs = (zeros(Float64, Qsize), zeros(Float64, Qsize)) # observable for the normalization diagram and the bubble

    avg, std = MonteCarlo.sample(totalStep, (T, K, Ext), diag, obs, integrand, measure, normalize;  Nblock=Ncpu, para=para, print=10)


    @unpack kF, β, m, n, extQ = Para()

    for (idx, q) in enumerate(extQ)
        q = q[1]
        p, err = TwoPoint.LindhardΩnFiniteTemperature(3, q, n, kF, β, m, 2)
        @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, avg[idx], std[idx], p, err)
    end
end

# @btime run(1, 10)
# @time run(Repeat, totalStep)
run(totalStep)
# @time run(Repeat, totalStep)