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
    # test bubble diagram of free electron
end
