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

    ####### Test fermi kernel accuracy for BigFloat #############
    τ, ω, β=BigFloat(0.5), BigFloat(1), BigFloat(1)
    s=Spectral.kernelT(:fermi, τ, ω, β)
    sext=exp(-ω*τ)/(BigFloat(1)+exp(-ω*β))
    @test abs(s-sext)<BigFloat(1e-64)
    ####### Test bose kernel accuracy for BigFloat #############
    τ, ω, β=BigFloat(0.5), BigFloat(1), BigFloat(1)
    s=Spectral.kernelT(:bose, τ, ω, β)
    sext=exp(-ω*τ)/(BigFloat(1)-exp(-ω*β))
    @test abs(s-sext)<BigFloat(1e-64)


    function testAccuracy(type, τGrid, ωGrid, β)
        maxErr=BigFloat(0.0)
        τ0, ω0, macheps = 0.0, 0.0, 0.0
        for (τi, τ) in enumerate(τGrid)
            for (ωi, ω) in enumerate(ωGrid)
                ker1=Spectral.kernelT(type, τ, ω, β)
                ker2=Spectral.kernelT(type, BigFloat(τ), BigFloat(ω), BigFloat(β))
                if abs(ker1-ker2)>maxErr
                    maxErr=abs(ker1-ker2)
                    τ0, ω0, macheps=τ, ω, eps(ker1) #get the machine accuracy for the float number ker1
                end
            end
        end
        @test maxErr<2macheps
        return maxErr, τ0, ω0, macheps
    end

    β, Euv=10000.0, 100.0
    τGrid=[t for t in LinRange(-β+1e-10, β, 100)]
    ωGrid=[w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps=testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps=testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid=[t for t in LinRange(-β+1e-6, β, 100)]
    ωGrid=[-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    maxErr, τ0, ω0, macheps=testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps=testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid=[-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    ωGrid=[w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps=testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps=testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid=[β-1e-6, β-1e-8, β-1e-10, -β+1e-10,-β+1e-8,-β+1e-6]
    ωGrid=[w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps=testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps=testAccuracy(:bose, τGrid, ωGrid, β)
end

@testset "Correlator Representation" begin
    Euv = 1.0
    β = 10000.0
    eps = 1e-8
    epsintegral=1e-15
    S1(ω) = sqrt(1.0 - (ω/Euv)^2)/Euv # semicircle -1<ω<1
    S2(ω) = sqrt(1.0/(((ω/Euv)-0.5)^2+1.0)+1.0/(((ω/Euv)+0.5)^2+1.0))/Euv

    function Case(τGrid)
        g=zeros(Float64, length(τGrid))
        for (τi, τ) in enumerate(τGrid)
            g[τi]=Spectral.kernelT(:fermi, τ, -0.5, β)+Spectral.kernelT(:fermi, τ, 0.5, β)
            g[τi]+=Spectral.kernelT(:fermi, τ, -0.2, β)+Spectral.kernelT(:fermi, τ, 0.8, β)
        end
        return g
    end

    dlr = Basis.dlrGrid(:fermi, Euv, β, eps)

    #get imaginary-time Green's function
    G1, err1 = Spectral.freq2tau(:fermi, S1, dlr[:τ], β, -Euv, Euv, epsintegral)
    @test all(err1 .< epsintegral) # make sure the Green's function is sufficiently accurate 
    G2, err2 = Spectral.freq2tau(:fermi, S2, dlr[:τ], β, -Euv, Euv, epsintegral)
    @test all(err2 .< epsintegral) # make sure the Green's function is sufficiently accurate 
    G=zeros(Float64, (2, length(G1)))
    G[1, :]=G1
    # G[2, :]=G2
    G[2, :] .= Case(dlr[:τ])

    #get imaginary-time Green's function for τ sample
    τSample=[t for t in LinRange(0.0, β, 100)]
    G1, err1 = Spectral.freq2tau(:fermi, S1, τSample, β, -Euv, Euv, epsintegral)

    # for (gi, g) in enumerate(G1)
    #     println(τSample[gi]/β, "  ", g, "  ", err1[gi])
    # end

    @test all(err1 .< epsintegral) # make sure the Green's function is sufficiently accurate 
    G2, err2 = Spectral.freq2tau(:fermi, S2, τSample, β, -Euv, Euv, epsintegral)
    @test all(err2 .< epsintegral) # make sure the Green's function is sufficiently accurate 
    Gsample=zeros(Float64, (2, length(G1)))
    Gsample[1, :]=G1
    # Gsample[2, :]=G2
    Gsample[2, :] .= Case(τSample)

    #imaginary-time to dlr
    coeff = Basis.tau2dlr(:fermi, G, dlr, β, axis=2, rtol=eps)
    # Gp = Basis.dlr2tau(:fermi, coeff, dlr, β, axis=2)
    # @test all(abs.(G - Gp) .< 10eps) # dlr should represent the Green's function up to accuracy of the order eps

    Gsamplep = Basis.dlr2tau(:fermi, coeff, dlr, τSample, β, axis=2)
    println(maximum(abs.(Gsample[1, :])), ", ", maximum(abs.(Gsample[1, :]-Gsamplep[1, :])))
    println(maximum(abs.(Gsample[2, :])), ", ", maximum(abs.(Gsample[2, :]-Gsamplep[2, :])))
    # for (ti, t) in enumerate(τSample)
    #     println(t, " ", Gsamplep[1, ti]-Gsample[1, ti])
    # end
    @test all(abs.(Gsample - Gsamplep) .< 10eps) # dlr should represent the Green's function up to accuracy of the order eps

    # #get Matsubara-frequency Green's function
    # Gn1, errn1 = Spectral.freq2matfreq(:fermi, S1, dlr[:ωn], β, -Euv, Euv, eps)
    # @test all(abs.(errn1) .< eps) # make sure the Green's function is sufficiently accurate 
    # Gn2, errn2 = Spectral.freq2matfreq(:fermi, S2, dlr[:ωn], β, -Euv, Euv, eps)
    # @test all(abs.(errn2) .< eps) # make sure the Green's function is sufficiently accurate 
    # Gn=zeros(Complex{Float64}, (2, length(Gn1)))
    # Gn[1, :]=Gn1
    # Gn[2, :]=Gn2

    # #Matsubara frequency to dlr
    # coeffn = Basis.matfreq2dlr(:fermi, Gn, dlr, β, axis=2, rtol=eps)
    # Gnp = Basis.dlr2matfreq(:fermi, coeffn, dlr, β, axis=2)
    # @test all(abs.(Gn - Gnp) .< 10eps) # dlr should represent the Green's function up to accuracy of the order eps

    # #imaginary-time to Matsubar-frequency
    # Gnpp = Basis.tau2matfreq(:fermi, G, dlr, β, axis=2, rtol=eps)
    # # for i in 1:length(Gn[1, :])
    # #     println(Gn[1, i]*β, "   ", Gnpp[1, i])
    # # end
    # # @test all(abs.(Gn - Gnpp/β) .< 100eps) # dlr should represent the Green's function up to accuracy of the order eps
    # @test all(abs.(Gn - Gnpp) .< 100eps) # dlr should represent the Green's function up to accuracy of the order eps

    # #Matsubara-freqeuncy to imaginary-time
    # Gpp = Basis.matfreq2tau(:fermi, Gn, dlr, β, axis=2, rtol=eps)
    # # for i in 1:length(G[1, :])
    # #     println(G[1, i], "   ", Gpp[1, i]*β)
    # # end
    # # @test all(abs.(G - Gpp*β) .< 100eps) # dlr should represent the Green's function up to accuracy of the order eps
    # println(maximum(abs.(G-Gpp)))
    # @test all(abs.(G - Gpp) .< 100eps) # dlr should represent the Green's function up to accuracy of the order eps
end