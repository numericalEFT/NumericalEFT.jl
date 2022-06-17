@testset "Spectral functions" begin
    function testKernelT(isFermi, symmetry, sign, β, τ, ε)
        println("Testing $symmetry  for β=$β, τ=$τ, ε=$ε")
        n(ε, β) = 1.0 / (exp(ε * β) - sign * 1.0)
        @test Spectral.density(isFermi, ε, β) ≈ n(ε, β)
        @test Spectral.kernelT(Val(isFermi), Val(symmetry), eps(0.0), ε, β) ≈ 1.0 + sign * n(ε, β)

        # @test Spectral.kernelT(type, 0.0, ε, β) ≈ sign * n(ε, β) # τ=0.0 should be treated as the 0⁻
        @test Spectral.kernelT(Val(isFermi), Val(symmetry), -eps(0.0), ε, β) ≈ sign * n(ε, β)

        @test Spectral.kernelT(Val(isFermi), Val(symmetry), -τ, ε, β) ≈ sign * Spectral.kernelT(Val(isFermi), Val(symmetry), β - τ, ε, β)
        @test Spectral.kernelT(Val(isFermi), Val(symmetry), -eps(0.0), 1000.0, 1.0) ≈ 0.0
        @test Spectral.kernelT(Val(isFermi), Val(symmetry), -eps(0.0), -1000.0, 1.0) ≈ -1.0
        if isFermi
            # ω can not be zero for boson
            @test Spectral.kernelT(Val(true), Val(:none), τ, 0.0, β) ≈ n(0.0, β)
        end
    end

    testKernelT(true, :none, -1.0, 10.0, 1.0, 1.0)
    testKernelT(false, :none, 1.0, 10.0, 1.0, 1.0)

    testKernelT(true, :none, -1.0, 10.0, 1e-10, 1.0)
    testKernelT(false, :none, 1.0, 10.0, 1e-10, 1.0)

    testKernelT(true, :none, -1.0, 10.0, 1.0, 1.0e-6)
    testKernelT(false, :none, 1.0, 10.0, 1.0, 1.0e-6) # small ϵ for bosonic case is particularly dangerous because the kernal diverges ~1/ϵ

    function testAccuracy(isFermi, symmetry, τGrid, ωGrid, β)
        setprecision(128)
        maxErr = BigFloat(0.0)
        τ0, ω0, macheps = 0.0, 0.0, 0.0
        for (τi, τ) in enumerate(τGrid)
            for (ωi, ω) in enumerate(ωGrid)
                ker1 = Spectral.kernelT(Val(isFermi), Val(symmetry), τ, ω, β)
                ker2 = Spectral.kernelT(Val(isFermi), Val(symmetry), BigFloat(τ), BigFloat(ω), BigFloat(β))
                if abs(ker1 - ker2) > maxErr
                    maxErr = abs(ker1 - ker2)
                    τ0, ω0, macheps = τ, ω, eps(ker1) # get the machine accuracy for the float number ker1
                end
            end
        end
        @test maxErr < 2macheps
        return maxErr, τ0, ω0, macheps
    end

    println("testing kernel accuracy")
    β, Euv = 10000.0, 100.0
    τGrid = [t for t in LinRange(-β + 1e-10, β, 100)]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(true, :none, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(false, :none, τGrid, ωGrid, β)

    τGrid = [t for t in LinRange(-β + 1e-6, β, 100)]
    ωGrid = [-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    maxErr, τ0, ω0, macheps = testAccuracy(true, :none, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(false, :none, τGrid, ωGrid, β)

    τGrid = [-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(true, :none, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(false, :none, τGrid, ωGrid, β)

    τGrid = [β - 1e-6, β - 1e-8, β - 1e-10, -β + 1e-10, -β + 1e-8, -β + 1e-6]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(true, :none, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(false, :none, τGrid, ωGrid, β)
    println("kernel accuracy tested")
end