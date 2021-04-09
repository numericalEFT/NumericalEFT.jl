using Distributed

const Ncpu = 1
const totalStep = 1e7
const Repeat = 1

addprocs(Ncpu)

@everywhere using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, Statistics, BenchmarkTools, InteractiveUtils, Parameters

@everywhere struct Para
    β::Float64
    kF::Float64
    m::Float64
    extQ::Vector{SVector{3,Float64}}
end

@everywhere function integrand(config)
    if config.curr.id == 1
        return 1.0
    elseif config.curr.id == 2
            return eval2(config)
        else
            return 0.0
    end
end

@everywhere function eval2(config)
    para = config.para
    kF, β, m = para.kF, para.β, para.m
    T, K, Ext = config.var[1], config.var[2], config.var[3]
        # In case the compiler is too stupid, it is a good idea to explicitly specify the type here
    k = K[1]::SVector{3,Float64}
    Tin, Tout = T[1]::Float64, T[2]::Float64 
    extidx = Ext[1]::Int
    q = para.extQ[extidx]::SVector{3,Float64} # external momentum
    # q = [0.0, 0.0, 0.0] # external momentum
    kq = k + q
    τ = (Tout - Tin) / β
    ω1 = (dot(k, k) - kF^2) * β
    g1 = Spectral.kernelFermiT(τ, ω1)
    ω2 = (dot(kq, kq) - kF^2) * β
    g2 = Spectral.kernelFermiT(-τ, ω2)
    spin = 2
    phase = 1.0 / (2π)^3
    return g1 * g2 * spin * phase
end

@everywhere function measure(config)
    diag = config.curr
    factor = 1.0 / diag.reWeightFactor
    extidx = config.var[3][1]
    if diag.id == 1
        diag.obs[extidx] += factor
    elseif diag.id == 2
        weight = integrand(config)
        diag.obs[extidx] += weight / abs(weight) * factor
    else
        return
    end
end

@everywhere function MC(totalStep, pid, para)
    rng = MersenneTwister(pid)
    β, kF, m = para.β, para.kF, para.m

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.Discrete(1, length(para.extQ)) # external variable is specified
    diag1 = MonteCarlo.Diagram(1, 0, [1, 0, 1], zeros(Float64, Ext.size))
    diag2 = MonteCarlo.Diagram(2, 1, [2, 1, 1], zeros(Float64, Ext.size))

    # _obs1 = 0.0 # diag1 is a constant for normalization
    # _obs2 = zeros(Float64, Ext.size) # diag2 measures the bubble for different external q

    config = MonteCarlo.Configuration(totalStep, (diag1, diag2), [T, K, Ext], para; pid=pid, rng=rng)

    # @code_warntype MonteCarlo.Configuration(totalStep, (diag1, diag2), [T, K, Ext], para; pid=pid, rng=rng)
    # @code_warntype MonteCarlo.Diagram(2, 1, [2, 1, 1], zeros(Float64, Ext.size))
    # @code_warntype MonteCarlo.increaseOrder(config, integrand)
    # @code_warntype integrand(config)
    # @code_warntype eval2(config)
    # @code_warntype measure(config)
    # exit()

    MonteCarlo.montecarlo(config, integrand, measure)

    return diag2.obs / sum(diag1.obs) * Ext.size
end

function run(repeat, totalStep)
    kF = 1.919
    β = 25.0 / kF^2
    m = 0.5
    QSize = 16
    extQ = [@SVector [q, 0.0, 0.0] for q in LinRange(0.0, 3.0 * kF, QSize)]
    para = Para(β, kF, m, extQ)

    if Ncpu > 1
        result = pmap((x) -> MC(totalStep, rand(1:10000), para), 1:repeat)
    else
        result = map((x) -> MC(totalStep, rand(1:10000), para), 1:repeat)
    end

    # observable = []
    # for r in result
    #     push!(observable, r[1])
    # end
    observable = result

    obs = mean(observable)
    obserr = std(observable) / sqrt(length(observable))

    for (idx, q) in enumerate(extQ)
        q = q[1]
        p, err = Diagram.bubble(q, 0.0im, 3, kF, β, m)
        p, err = real(p) * 2.0, real(err) * 2.0
        @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, obs[idx], obserr[idx], p, err)
    end
end

# @btime run(1, 10)
run(Repeat, totalStep)