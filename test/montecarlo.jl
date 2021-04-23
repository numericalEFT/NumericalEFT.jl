function Sphere1(totalstep)
    function integrand(config)
        if config.curr == 1
            return 1.0
        else
            X = config.var[1]
            if (X[1]^2 + X[2]^2 < 1.0) 
                return 1.0 
            else 
                return 0.0
            end
        end
    end

    function measure(config)
        curr = config.curr
        factor = 1.0 / config.reweight[curr]
        if curr == 1
            config.observable[1] += factor
        elseif curr == 2
            weight = integrand(config)
            config.observable[2] += weight / abs(weight) * factor
        else
            error("impossible!")
        end
    end

    normalize(config) = config.observable[2] / config.observable[1]

    T = MonteCarlo.Tau(1.0, 1.0 / 2.0)
    dof = ([0, ], [2, ]) # number of T variable for the normalization and the integrand
    avg, err = MonteCarlo.sample(totalstep, (T,), dof, [0.0, 0.0], integrand, measure, normalize; Nblock=64, print=-1)

    return avg, err
end

function Sphere2(totalstep)
    function integrand(config)
        if config.curr == 1
            return 1.0
        else
            X = config.var[1]
            if (X[1][1]^2 + X[1][2]^2 < 1.0) 
                return 1.0 
            else 
                return 0.0
            end
        end
    end

    function measure(config)
        curr = config.curr
        factor = 1.0 / config.reweight[curr]
        if curr == 1
            config.observable[1] += factor
        elseif curr == 2
            weight = integrand(config)
            config.observable[2] += weight / abs(weight) * factor
        else
            error("impossible!")
        end
    end

    normalize(config) = config.observable[2] / config.observable[1]

    T = MonteCarlo.TauPair(1.0, 1.0 / 2.0)
    dof = ([0, ], [1, ]) # number of T variable for the normalization and the integrand
    avg, err = MonteCarlo.sample(totalstep, (T,), dof, [0.0, 0.0], integrand, measure, normalize; Nblock=64, print=-1)

    return avg, err
end

@testset "MonteCarlo Sampler" begin
    totalStep = 1000_000

    avg, err = Sphere1(totalStep)
    println("MC integration 1: $avg ± $err (exact: $(π / 4.0))")
    @test abs(avg - π / 4.0) < 5.0 * err

    avg, err = Sphere2(totalStep)
    println("MC integration 2: $avg ± $err (exact: $(π / 4.0))")
    @test abs(avg - π / 4.0) < 5.0 * err
end
