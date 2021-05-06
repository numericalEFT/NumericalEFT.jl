# This example demonstrated how to calculate the bubble diagram of free electrons using the Monte Carlo module

using Distributed
using QuantumStatistics, LinearAlgebra, Random, Printf, BenchmarkTools, InteractiveUtils, Parameters

const Ncpu = 1 
const totalStep = 1e8

addprocs(Ncpu)

@everywhere include("parameter.jl")

@everywhere using QuantumStatistics, Parameters, Random, LinearAlgebra

@everywhere @with_kw struct Para
    n::Int = 0 # external Matsubara frequency
    Qsize::Int = 16
    extQ::Vector{SVector{3,Float64}} = [@SVector [q, 0.0, 0.0] for q in LinRange(0.0, 3.0 * kF, Qsize)]
end

@everywhere function integrand(config)
    if config.curr != 1
        error("impossible")
    end
    para = config.para

    T, K, Ext = config.var[1], config.var[2], config.var[3]
    k = K[1]
    Tin, Tout = 0.0, T[1] 
    extidx = Ext[1]
    q = para.extQ[extidx] # external momentum
    kq = k + q
    τ = (Tout - Tin) / β
    ω1 = (dot(k, k) - kF^2) / (2me) * β
    g1 = Spectral.kernelFermiT(τ, ω1)
    ω2 = (dot(kq, kq) - kF^2) / (2me) * β
    g2 = Spectral.kernelFermiT(-τ, ω2)
    phase = 1.0 / (2π)^3
    return g1 * g2 * spin * phase * cos(2π * para.n * τ)
end

@everywhere function measure(config)
    obs = config.observable
    curr = config.curr
    factor = 1.0 / config.reweight[curr]
    extidx = config.var[3][1]
    weight = integrand(config)
    obs[curr, extidx] += weight / abs(weight) * factor
end

function run(totalStep)

    para = Para()
    @unpack extQ, Qsize = para 

    T = MonteCarlo.Tau(β, β / 2.0)
    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    Ext = MonteCarlo.Discrete(1, length(extQ)) # external variable is specified

    dof = [[1, 1, 1],] # degrees of freedom of the normalization diagram and the bubble
    obs = zeros(Float64, (1, Qsize)) # observable for the normalization diagram and the bubble

    avg, std = MonteCarlo.sample(totalStep, (T, K, Ext), dof, obs, integrand, measure; para=para, print=1)


    @unpack n, extQ = Para()

    for (idx, q) in enumerate(extQ)
        q = q[1]
        p, err = TwoPoint.LindhardΩnFiniteTemperature(dim, q, n, kF, β, me, spin)
        @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, avg[idx], std[idx], p, err)
    end
end

run(totalStep)
# @time run(totalStep)