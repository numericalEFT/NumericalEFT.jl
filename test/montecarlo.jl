function Sphere1(totalstep)
    function integrand(config)
        X = config.var[1]
        if (X[1]^2 + X[2]^2 < 1.0) 
            return 1.0 
        else 
            return 0.0
        end
    end

    function measure(config)
        factor = 1.0 / config.reweight[config.curr]
        weight = integrand(config)
        config.observable += weight / abs(weight) * factor
    end

    T = MonteCarlo.Tau(1.0, 1.0 / 2.0)
    dof = [[2, ],] # number of T variable for the normalization and the integrand
    config = MonteCarlo.Configuration(totalstep, (T,), dof, 0.0)
    avg, err = MonteCarlo.sample(config, integrand, measure; Nblock=64, print=-1)
    # avg, err = MonteCarlo.sample(totalstep, (T,), dof, [0.0, ], integrand, measure; Nblock=64, print=-1)

    return avg, err
end

function Sphere2(totalstep)
    function integrand(config)
        X = config.var[1]
        if (X[1][1]^2 + X[1][2]^2 < 1.0) 
            return 1.0 
        else 
            return 0.0
        end
    end

    function measure(config)
        factor = 1.0 / config.reweight[config.curr]
        weight = integrand(config)
        config.observable += weight / abs(weight) * factor
    end

    T = MonteCarlo.TauPair(1.0, 1.0 / 2.0)
    dof = [[1, ],] # number of T variable for the normalization and the integrand
    config = MonteCarlo.Configuration(totalstep, (T,), dof, 0.0)
    avg, err = MonteCarlo.sample(config, integrand, measure; Nblock=64, print=-1)
    # avg, err = MonteCarlo.sample(totalstep, (T,), dof, [0.0, ], integrand, measure; Nblock=64, print=-1)

    return avg, err
end

function Sphere3(totalstep)
    function integrand(config)
        @assert config.curr == 1 || config.curr == 2
        X = config.var[1]
        if config.curr == 1
            if (X[1]^2 + X[2]^2 < 1.0) 
                return 1.0 
            else 
                return 0.0
            end
        else
            if (X[1]^2 + X[2]^2 + X[3]^2 < 1.0) 
                return 1.0 
            else 
                return 0.0
            end
        end
    end

    function measure(config)
        factor = 1.0 / config.reweight[config.curr]
        weight = integrand(config)
        config.observable[config.curr] += weight / abs(weight) * factor
    end

    T = MonteCarlo.Tau(1.0, 1.0 / 2.0)
    dof = [[2, ], [3, ]] # number of T variable for the normalization and the integrand
    config = MonteCarlo.Configuration(totalstep, (T,), dof, [0.0, 0.0])
    avg, err = MonteCarlo.sample(config, integrand, measure; Nblock=64, print=-1)
    # avg, err = MonteCarlo.sample(totalstep, (T,), dof, [0.0, ], integrand, measure; Nblock=64, print=-1)

    return avg, err
end

@testset "MonteCarlo Sampler" begin
    totalStep = 1000_000

    avg, err = Sphere1(totalStep)
    println("MC integration 1: $avg ± $err (exact: $(π / 4.0))")
    @test abs(avg - π / 4.0) < 5.0 * err
    # @test abs(avg[1] - π / 4.0) < 5.0 * err[1]

    avg, err = Sphere2(totalStep)
    println("MC integration 2: $avg ± $err (exact: $(π / 4.0))")
    @test abs(avg - π / 4.0) < 5.0 * err
    # @test abs(avg[1] - π / 4.0) < 5.0 * err[1]

    avg, err = Sphere3(totalStep)
    println("MC integration 3: $(avg[1]) ± $(err[1]) (exact: $(π / 4.0))")
    println("MC integration 3: $(avg[2]) ± $(err[2]) (exact: $(4.0 * π / 3.0 / 8))")
    @test abs(avg[1] - π / 4.0) < 5.0 * err[1]
    @test abs(avg[2] - π / 6.0) < 5.0 * err[2]
end
