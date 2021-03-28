using QuantumStatistics
using LinearAlgebra
using Statistics
using Random

function MC(block, rng)
    kF, β = 1.919, 25.0
    function eval1(config)
        T=config.var[2][1]
        return 1.0
    end

    function eval2(config)
        K=config.var[1][1]
        Tin=config.var[2][1]
        Tout=config.var[2][2]
        # println(Tout, ", ", Tin)
        τ=(Tout-Tin)/β
        ω=(dot(K, K)-kF^2)*β
        g1=Spectral.kernelFermiT(τ, ω)
        g2=Spectral.kernelFermiT(-τ, ω)
        spin=2
        phase=1.0/(2π)^3
        return g1*g2*spin*phase
    end

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.External([1]) #external variable is specified
    group1 = MonteCarlo.Group(1, [0, 1], zeros(Float64, Ext.size...), eval1)
    group2 = MonteCarlo.Group(2, [1, 2], zeros(Float64, Ext.size...), eval2)
    config = MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)

    MonteCarlo.montecarlo(config)
    w1=group1.observable[1]
    w2=group2.observable[1]
    println(group1.visitedSteps, " vs ", group2.visitedSteps)
    println(group1.reWeightFactor, " vs ", group2.reWeightFactor)
    return w2/w1
end

function run()
    N=30
    block=10
    result=zeros(Float64, N)
    rngs = [MersenneTwister(i) for i in 1:Threads.nthreads()]

    Threads.@threads for i in 1:N
        result[i]=MC(block, rngs[i])
    end
    println(mean(result),"±",std(result)/sqrt(length(result)))
end

run()


# println(group1.observable)
# println(group2.observable)