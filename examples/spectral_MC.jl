using Distributed

const Ncpu = 16
const totalStep = 1e9
const Repeat = 16

addprocs(Ncpu)

@everywhere using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, Statistics, BenchmarkTools, InteractiveUtils

@everywhere function MC(totalStep, pid, kF, Euv)
    rng = MersenneTwister(pid)

    function eval1(config)
        return 1.0
    end

    function eval2(config)
        ω = config.X[1] - Euv
        # println(ω)
        # τ = 0.080808080808080
        τ = 0.0909090909090909
        return Spectral.kernelT(:fermi, τ, ω) * sqrt(1.0 - (ω / Euv)^2) / Euv
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
    Ω = MonteCarlo.Tau(2Euv, 1.0)
    Ext = MonteCarlo.External([1]) # external variable is specified
    obs1 = 0.0 # diag1 is a constant for normalization
    obs2 = zeros(Float64, Ext.size...) # diag2 measures the bubble for different external q
    diag1 = MonteCarlo.Diagram(1, 0, 0, 0)
    diag2 = MonteCarlo.Diagram(2, 1, 1, 0)
    config = MonteCarlo.Configuration(totalStep, (diag1, diag2), Ω, K, Ext; pid=pid, rng=rng)

    # @benchmark eval2(c) setup=(c=$config)

    # @code_warntype MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)
    # @code_llvm MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype eval2(config)

    MonteCarlo.montecarlo(config, integrand, measure)

    return obs2 / obs1 * Ext.size[1]
end

function run(repeat, totalStep)
    kF = 1.919
    Euv = 10000.0
    if Ncpu > 1
        result = pmap((x) -> MC(totalStep, rand(1:10000), kF, Euv), 1:repeat)
    else
        result = map((x) -> MC(totalStep, rand(1:10000), kF, Euv), 1:repeat)
    end

    observable = []
    for r in result
        push!(observable, r)
    end

    obs = mean(observable)
    obserr = std(observable) / sqrt(length(observable))
    @printf("%10.6f ± %10.6f\n", obs[1], obserr[1])

    # for (idx, q) in enumerate(extQ)
    #     q = q[1]
        # p, err = Diagram.bubble(q, 0.0im, 3, kF, β, m)
        # p, err = real(p) * 2.0, real(err) * 2.0
        # @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, obs[idx], obserr[idx], p, err)
    #     @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, obs[idx], obserr[idx], p, err)
    # end
end

# @btime run(1, 10)
run(Repeat, totalStep)