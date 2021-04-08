using Distributed

const Ncpu = 4
const totalStep = 1e7
const Repeat = 4

addprocs(Ncpu)

@everywhere using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, Statistics, BenchmarkTools, InteractiveUtils


@everywhere function MC(totalStep, pid, kF, β)
    rng = MersenneTwister(pid)

    function WRPA(τ,q,β)
        # temporarily using a toy model
        g=1.0

        factor=1.0
        q2=dot(q,q)
        if τ<-β
            τ=τ+2*β
        elseif τ<0
            τ=τ+β
            factor=1.0
        end
        sq2g=sqrt(q2+g)
        return -factor*4*π/(q2+g)/sq2g*(exp(-sq2g*τ)+exp(-sq2g*(β-τ)))/(1-exp(-sq2g*β))
    end

    function eval1(config)
        return 1.0
    end

    function eval2(config)
        q = config.K[1]
        t = config.X[1]

        k1 = extQ[config.ext.idx[1]] # external momentum
        k2 = extQ[config.ext.idx[2]] # external momentum
        t1 = extT[config.ext.idx[3]] # external momentum
        t2 = extT[config.ext.idx[4]] # external momentum


        ω1 = (dot(q-k1, q-k1) - kF^2) * β
        τ1 = (t1-t)/β
        g1 = Spectral.kernelFermiT(τ1, ω1)

        ω2 = (dot(q-k2, q-k2) - kF^2) * β
        τ2 = (β-t1-t)/β
        g2 = Spectral.kernelFermiT(τ2, ω2)

        W1 = WRPA(t, q-k1-k2,β)
        W2 = WRPA(t-t1-t2,q,β)

        legendre=1.0
        factor = 1.0
        return g1 * g2 * W1 * W2 * factor * legendre
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
            if !isnan(weight) && abs(weight) > 1e-8 
                obs2[config.ext.idx[1]] += weight / abs(weight) * factor
            end
        else
            return
        end
    end

    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Nk = 8
    Nt = 8
    Ext = MonteCarlo.External([Nk,Nk,Nt,Nt]) # external variable is specified
    extQ = [@SVector [q, 0.0, 0.0] for q in range(0.0, stop=3.0 * kF, length=Nk)]
    extT = range(0.0, stop=β, length=Nt)
    obs1 = 0.0 # diag1 is a constant for normalization
    obs2 = zeros(Float64, Ext.size...) # diag2 measures the bubble for different external q
    diag1 = MonteCarlo.Diagram(1, 0, 1, 0)
    diag2 = MonteCarlo.Diagram(2, 1, 1, 1)
    config = MonteCarlo.Configuration(totalStep, (diag1, diag2), T, K, Ext; pid=pid, rng=rng)

    # @benchmark eval2(c) setup=(c=$config)

    # @code_warntype MonteCarlo.Configuration(block, (group1, group2), (K, T), Ext; pid = 1, rng=rng)
    # @code_llvm MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype MonteCarlo.increaseOrder(config, config.curr)
    # @code_warntype eval2(config)

    MonteCarlo.montecarlo(config, integrand, measure)

    return obs2 / obs1 * Ext.size[1]^2 * Ext.size[3]^2, extQ, extT
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
    extT = result[1][3]

    observable = []
    for r in result
        push!(observable, r[1])
    end

    obs = mean(observable)
    obserr = std(observable) / sqrt(length(observable))

    for (idx, t) in enumerate(extT)
        t = t[1]
        @printf("%10.6f  %10.6f ± %10.6f\n", t, obs[idx], obserr[idx])
    end
end

# @btime run(1, 10)
run(Repeat, totalStep)
