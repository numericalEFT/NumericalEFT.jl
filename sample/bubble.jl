using Distributed

const N=16
addprocs(N)
@everywhere using QuantumStatistics, Statistics, LinearAlgebra, Random

@everywhere function MC(block, kF, β, x)
    rng=MersenneTwister(x)
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
    # println(procs())
    block=10
    # result=zeros(Float64, N)
    kF, β = 1.919, 25.0

    result=pmap((x)->MC(block, kF, β, x), 1:N)

    println(mean(result)," ± ",std(result)/sqrt(length(result)))
    p, err=Diagram.bubble(0.0, 0.0im, 3, kF, β)
    println(real(p)*2, " ± ", real(err)*2)
end

run()


# println(group1.observable)
# println(group2.observable)