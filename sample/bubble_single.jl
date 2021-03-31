using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays
using BenchmarkTools
using InteractiveUtils

const kF = 1.919
const β = 25.0 / kF^2

function MC(block, x)
    rng = MersenneTwister(x)

    function eval1(X, K, ext, step)
        return 1.0
    end

    function eval2(X, K, ext, step)
        k = K[1]
        Tin = X[1]
        Tout = X[2]
        q = extQ[ext.idx[1]] # external momentum
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

    function integrand(diag, X, K, ext, step)
        if diag.id == 1
            return eval1(X, K, ext, step)
        elseif diag.id == 2
            return eval2(X, K, ext, step)
        else
            return 0.0
        end
    end

    function measure(diag, X, K, ext, step)
        factor = 1.0 / diag.reWeightFactor
        if diag.id == 1
            obs1 += factor
        elseif diag.id == 2
            weight = integrand(diag, X, K, ext, step)
            obs2[ext.idx[1]] += weight / abs(weight) * factor
        else
            return
        end
    end

# function measure(config, integrand)
#     curr = config.curr
#     # factor = 1.0 / config.absWeight / curr.reWeightFactor
#     weight = integrand(curr.id, config.X, config.K, config.ext, config.step)
#     obs = curr.observable
#     obs[config.ext.idx...] += weight / abs(weight) / curr.reWeightFactor
# end

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.External([16]) # external variable is specified
    extQ = [@SVector [q, 0.0, 0.0] for q in range(0.0, stop=3.0 * kF, length=Ext.size[1])]
    obs1 = 0.0 # group1 is a constant for normalization
    obs2 = zeros(Float64, Ext.size...) # group2 measures the bubble for different external q
    diag1 = MonteCarlo.Diagram(1, 0, 1, 0)
    diag2 = MonteCarlo.Diagram(2, 1, 2, 1)

    # @benchmark eval2(c) setup=(c=$config)

    # @code_warntype MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)
    # @code_llvm MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype eval2(config)

    MonteCarlo.montecarlo(block, (diag1, diag2), T, K, Ext, integrand, measure; pid=x, rng=rng)

    # println(group1.visitedSteps, " vs ", group2.visitedSteps)
    # println(group1.reWeightFactor, " vs ", group2.reWeightFactor)
    return obs2 / obs1 * Ext.size[1], extQ
end

function run()
    # println(procs())
    block = 10
    # result=zeros(Float64, N)
    # kF, β = 1.919, 25.0

    result, extQ = MC(block, rand(1:10000))

    for (idx, q) in enumerate(extQ)
        q = q[1]
        p, err = Diagram.bubble(q, 0.0im, 3, kF, β)
        p, err = real(p) * 2.0, real(err) * 2.0
        polar = result[idx]
        @printf("%10.6f  %10.6f ± %10.6f  %10.6f ± %10.6f\n", q / kF, polar, 0.0, p, err)
    end
end

# @btime run()
run()
