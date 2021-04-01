@testset "Two-point correlator" begin
    β, τ, ε = 10.0, 1.0, 1.0
    n(ε) = 1.0 / (1.0 + exp(β * ε))
    @test Spectral.fermiDirac(β * ε) ≈ n(ε)

    @test TwoPoint.fermiT(τ, 0.0, β) ≈ n(0.0)
    @test TwoPoint.fermiT(eps(0.0), ε, β) ≈ 1.0 - n(ε)

    @test TwoPoint.fermiT(0.0, ε, β) ≈ -n(ε) # τ=0.0 should be treated as the 0⁻
    @test TwoPoint.fermiT(-eps(0.0), ε, β) ≈ -n(ε)

    @test TwoPoint.fermiT(-τ, ε, β) ≈ -TwoPoint.fermiT(β - τ, ε, β)
    @test TwoPoint.fermiT(-eps(0.0), 1000.0, β) ≈ 0.0
    @test TwoPoint.fermiT(-eps(0.0), -1000.0, β) ≈ -1.0
end


@testset "Spectral functions" begin
    function testKernelT(type, sign, β, τ, ε)
        println("Testing $type for β=$β, τ=$τ, ε=$ε")
    n(ε, β) = 1.0 / (exp(ε*β) - sign * 1.0)
    @test Spectral.density(type, ε, β) ≈ n(ε, β)
    @test Spectral.kernelT(type, eps(0.0), ε, β) ≈ 1.0 + sign * n(ε, β)

    @test Spectral.kernelT(type, 0.0, ε, β) ≈ sign * n(ε, β) # τ=0.0 should be treated as the 0⁻
    @test Spectral.kernelT(type, -eps(0.0), ε, β) ≈ sign * n(ε, β)

    @test Spectral.kernelT(type, -τ, ε, β) ≈ sign * Spectral.kernelT(type, β - τ, ε, β)
    @test Spectral.kernelT(type, -eps(0.0), 1000.0, 1.0) ≈ 0.0
    @test Spectral.kernelT(type, -eps(0.0), -1000.0, 1.0) ≈ -1.0
    if type == :fermi
        # ω can not be zero for boson
        @test Spectral.kernelT(:fermi, τ, 0.0, β) ≈ n(0.0, β)
    end
end
    testKernelT(:fermi, -1.0, 10.0, 1.0, 1.0)
    testKernelT(:bose, 1.0, 10.0, 1.0, 1.0)

    testKernelT(:fermi, -1.0, 10.0, 1e-10, 1.0)
    testKernelT(:bose, 1.0, 10.0, 1e-10, 1.0)

    testKernelT(:fermi, -1.0, 10.0, 1.0, 1.0e-6)
    testKernelT(:bose, 1.0, 10.0, 1.0, 1.0e-6) #small ϵ for bosonic case is particularly dangerous because the kernal diverges ~1/ϵ
end

@testset "Correlator Representation" begin
    S(ω) = sqrt(1.0 - ω^2) # semicircle -1<ω<1
    Euv = 1.0
    β = 1000.0
    eps = 1e-10
    dlr = Basis.dlrGrid(:fermi, Euv, β, eps)
    G, err = Spectral.freq2Tau(:fermi, S, dlr[:τ], β, -1.0, 1.0, eps)
    @test all(err .< eps) # make sure the Green's function is sufficiently accurate 

    coeff = Basis.tau2dlr(:fermi, G, dlr, β, rtol=eps)
    Gp = Basis.dlr2tau(:fermi, coeff, dlr, β)
    @test all(abs.(G - Gp) .< 10eps) # dlr should represent the Green's function up to accuracy of the order eps
end