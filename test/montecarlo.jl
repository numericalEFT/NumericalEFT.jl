@testset "MonteCarlo Sampler" begin
    # test if the forward proposal probability is the inverse of the backward proposal probability
    N = 20

    # size = 100
    # oldIdx, newIdx = 1, Int(size / 2)
    # for i = 1:N
    #     @test MonteCarlo.createIdx!(newIdx, size) * MonteCarlo.removeIdx(newIdx, size) ≈ 1.0
    #     @test newIdx >= 1 && newIdx <= size

    #     @test MonteCarlo.shiftIdx!(oldIdx, newIdx, size) *
    #           MonteCarlo.shiftIdx!(newIdx, oldIdx, size) ≈ 1.0
    #     @test newIdx >= 1 && newIdx <= size
    # end

    # β, step = 1.0, 0.5
    # oldT, newT = 0.0, β / 2.0
    # for i = 1:N
    #     @test MonteCarlo.createT!(newT, β) * MonteCarlo.removeT(newT, β) ≈ 1.0
    #     @test newT >= 0.0 && newT < β
    #     @test MonteCarlo.shiftT!(oldT, newT, step, β) *
    #           MonteCarlo.shiftT!(newT, oldT, step, β) ≈ 1.0
    #     @test oldT >= 0.0 && oldT < β
    #     @test newT >= 0.0 && newT < β
    #     @test MonteCarlo.shiftT_flip!(oldT, newT, β) *
    #           MonteCarlo.shiftT_flip!(newT, oldT, β) ≈ 1.0
    #     @test oldT >= 0.0 && oldT < β
    #     @test newT >= 0.0 && newT < β
    # end

    # Kf, δK = 1.0, 0.5
    # oldK3 = [Kf / 3.0, Kf / 3.0, Kf / 3.0]
    # newK3 = oldK3 * 2.0
    # oldK2 = [Kf / 3.0, Kf / 3.0]
    # newK2 = oldK2 * 2.0
    # for i = 1:N
    #     @test MonteCarlo.createFermiK!(newK2, Kf, δK) *
    #           MonteCarlo.removeFermiK(newK2, Kf, δK) ≈ 1.0
    #     @test MonteCarlo.createFermiK!(newK3, Kf, δK) *
    #           MonteCarlo.removeFermiK(newK3, Kf, δK) ≈ 1.0
    # end

end

@testset "MonteCarlo Bubble" begin
    #test bubble diagram of free electron
    kF, β = 1.0, 25.0
    function eval1(configuration)
        return 1.0
    end

    function eval2(configuration)
        return 1.0
    end

    K = MonteCarlo.FermiK(3, kF, 0.5 * kF, 10.0 * kF)
    T = MonteCarlo.Tau(β, β / 2.0)
    Ext = MonteCarlo.External([1]) #external variable is specified
    group1 = MonteCarlo.Group(1, [0, 1], zeros(Float64, Ext.size...), eval1)
    group2 = MonteCarlo.Group(2, [1, 2], zeros(Float64, Ext.size...), eval2)
    config = MonteCarlo.Configuration(100, (group1, group2), (K, T), Ext; pid = 1)

    println("eval: ", group1.eval(config))

    println(K[1])
    println(MonteCarlo.create!(K, 1))
    println(MonteCarlo.remove(K, 1))
    println(K[1])

    MonteCarlo.montecarlo(config)
end
