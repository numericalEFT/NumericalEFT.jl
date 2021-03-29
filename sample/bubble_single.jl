using QuantumStatistics, Statistics, LinearAlgebra, Random
using BenchmarkTools
using InteractiveUtils

const kF = 1.919
const β = 25.0 / kF^2

function MC(block, x)
    rng = MersenneTwister(x)

    function eval1(X, K, ext, step)
        T = X[1]
        return 1.0
    end

    function eval2(X, K, ext, step)
        k = K[1]
        Tin = X[1]
        Tout = X[2]
        # println(Tout, ", ", Tin)
        τ = (Tout - Tin) / β
        ω = (dot(k, k) - kF^2) * β
        g1 = Spectral.kernelFermiT(τ, ω)
        g2 = Spectral.kernelFermiT(-τ, ω)
        spin = 2
        phase = 1.0 / (2π)^3
        return g1 * g2 * spin * phase
    end

    function integrand(id, X, K, ext, step)
        if id == 1
            return eval1(X, K, ext, step)
        elseif id == 2
            return eval2(X, K, ext, step)
        else
            return 0.0
        end
    end

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.External([1]) # external variable is specified
    group1 = MonteCarlo.Group(1, 0, 1, 0, zeros(Float64, Ext.size...))
    group2 = MonteCarlo.Group(2, 1, 2, 1, zeros(Float64, Ext.size...))

    # @benchmark eval2(c) setup=(c=$config)

    # @code_warntype MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)
    # @code_llvm MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype eval2(config)

    MonteCarlo.montecarlo(block, integrand, (group1, group2), T, K, Ext; pid = x, rng = rng)

    w1 = group1.observable[1]
    w2 = group2.observable[1]
    println(group1.visitedSteps, " vs ", group2.visitedSteps)
    println(group1.reWeightFactor, " vs ", group2.reWeightFactor)
    return w2 / w1
end

function run()
    # println(procs())
    block = 10
    # result=zeros(Float64, N)
    # kF, β = 1.919, 25.0

    result = MC(block, 1)
    println(result)

    println(mean(result), " ± ", std(result) / sqrt(length(result)))
    p, err = Diagram.bubble(0.0, 0.0im, 3, kF, β)
    println(real(p) * 2, " ± ", real(err) * 2)
end

@btime run()
# run()
