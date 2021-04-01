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
    τ, ε = 0.1, 10.0
    n(ε) = 1.0 / (1.0 + exp(ε))
    @test Spectral.fermiDirac(ε) ≈ n(ε)

    @test Spectral.kernelFermiT(τ, 0.0) ≈ n(0.0)
    @test Spectral.kernelFermiT(eps(0.0), ε) ≈ 1.0 - n(ε)

    @test Spectral.kernelFermiT(0.0, ε) ≈ -n(ε) # τ=0.0 should be treated as the 0⁻
    @test Spectral.kernelFermiT(-eps(0.0), ε) ≈ -n(ε)

    @test Spectral.kernelFermiT(-τ, ε) ≈ -Spectral.kernelFermiT(1 - τ, ε)
    @test Spectral.kernelFermiT(-eps(0.0), 1000.0) ≈ 0.0
    @test Spectral.kernelFermiT(-eps(0.0), -1000.0) ≈ -1.0
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