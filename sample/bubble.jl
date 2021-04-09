using Distributed

const Ncpu = 1
const totalStep = 1e7
const Repeat = 1

addprocs(Ncpu)

@everywhere using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, Statistics, BenchmarkTools, InteractiveUtils

@everywhere function MC(totalStep, pid, kF, β)
    rng = MersenneTwister(pid)

    function eval1(config)
        return 1.0
    end

    function eval2(config)
        k = config.var[2][1]
        Tin = config.var[1][1]
        Tout = config.var[1][2]
        q = extQ[config.ext.idx[1]] # external momentum
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

    function integrand(config)
        if config.curr.id == 1
            return eval1(config)
        elseif config.curr.id == 2
            return eval2(config)
        else
            return 0.0
        end
    end

    function measure(config)
        diag = config.curr
        factor = 1.0 / diag.reWeightFactor
        if diag.id == 1
            obs1 += factor
        elseif diag.id == 2
            weight = integrand(config)
            obs2[config.ext.idx[1]] += weight / abs(weight) * factor
        else
            return
        end
    end

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.External([16]) # external variable is specified
    extQ = [@SVector [q, 0.0, 0.0] for q in range(0.0, stop=3.0 * kF, length=Ext.size[1])]
    obs1 = 0.0 # diag1 is a constant for normalization
    obs2 = zeros(Float64, Ext.size...) # diag2 measures the bubble for different external q
    diag1 = MonteCarlo.Diagram(1, 0, [1, 0])
    diag2 = MonteCarlo.Diagram(2, 1, [2, 1])
    config = MonteCarlo.Configuration(totalStep, (diag1, diag2), [T, K], Ext; pid=pid, rng=rng)

    # @benchmark eval2(c) setup=(c=$config)

    # @code_warntype MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)
    # @code_llvm MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype eval2(config)

    MonteCarlo.montecarlo(config, integrand, measure)

    return obs2 / obs1 * Ext.size[1], extQ
end

function run(repeat, totalStep)
    kF = 1.919
    β = 25.0 / kF^2
    m = 0.5
    if Ncpu > 1
        result = pmap((x) -> MC(totalStep, rand(1:10000), kF, β), 1:repeat)
    else
        result = map((x) -> MC(totalStep, rand(1:10000), kF, β), 1:repeat)
    end

    extQ = result[1][2]

    observable = []
    for r in result
        push!(observable, r[1])
    end

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