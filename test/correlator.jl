using FastGaussQuadrature

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
    println("Testing $type  for β=$β, τ=$τ, ε=$ε")
    n(ε, β) = 1.0 / (exp(ε * β) - sign * 1.0)
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
    testKernelT(:bose, 1.0, 10.0, 1.0, 1.0e-6) # small ϵ for bosonic case is particularly dangerous because the kernal diverges ~1/ϵ

    function testAccuracy(type, τGrid, ωGrid, β)
    maxErr = BigFloat(0.0)
    τ0, ω0, macheps = 0.0, 0.0, 0.0
    for (τi, τ) in enumerate(τGrid)
        for (ωi, ω) in enumerate(ωGrid)
            ker1 = Spectral.kernelT(type, τ, ω, β)
            ker2 = Spectral.kernelT(type, BigFloat(τ), BigFloat(ω), BigFloat(β))
            if abs(ker1 - ker2) > maxErr
                maxErr = abs(ker1 - ker2)
                τ0, ω0, macheps = τ, ω, eps(ker1) # get the machine accuracy for the float number ker1
            end
        end
    end
    @test maxErr < 2macheps
    return maxErr, τ0, ω0, macheps
end

    β, Euv = 10000.0, 100.0
    τGrid = [t for t in LinRange(-β + 1e-10, β, 100)]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid = [t for t in LinRange(-β + 1e-6, β, 100)]
    ωGrid = [-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    maxErr, τ0, ω0, macheps = testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid = [-1e-6, -1e-8, -1e-10, 1e-10, 1e-8, 1e-6]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(:bose, τGrid, ωGrid, β)

    τGrid = [β - 1e-6, β - 1e-8, β - 1e-10, -β + 1e-10,-β + 1e-8,-β + 1e-6]
    ωGrid = [w for w in LinRange(-Euv, Euv, 100)]
    maxErr, τ0, ω0, macheps = testAccuracy(:fermi, τGrid, ωGrid, β)
    maxErr, τ0, ω0, macheps = testAccuracy(:bose, τGrid, ωGrid, β)
end

function SemiCircle(type, Grid, β, Euv; IsMatFreq=false)
    # calculate Green's function defined by the spectral density
    # S(ω) = sqrt(1 - (ω / Euv)^2) / Euv # semicircle -1<ω<1

    ##### Panels endpoints for composite quadrature rule ###
    npo = Int(ceil(log(β*Euv) / log(2.0)))
    pbp = zeros(Float64, 2npo + 1)
    pbp[npo + 1] = 0.0
    for i in 1:npo
        pbp[npo + i + 1] = 1.0 / 2^(npo - i)
    end
    pbp[1:npo] = -pbp[2npo + 1:-1:npo + 2]

    function Green(n, IsMatFreq)
        xl, wl = gausslegendre(n)
        xj, wj = gaussjacobi(n, 1 / 2, 0.0)

        G = IsMatFreq ? zeros(ComplexF64, length(Grid)) : zeros(Float64, length(Grid))
        err=zeros(Float64, length(Grid))
        for (τi, τ) in enumerate(Grid)
            for ii in 2:2npo-1
                a, b = pbp[ii], pbp[ii+1]
                for jj in 1:n
                    x = (a+b)/2+(b-a)/2*xl[jj]
                    ker = IsMatFreq ? Spectral.kernelΩ(type, τ, Euv*x, β) : Spectral.kernelT(type, τ, Euv*x, β)
                    G[τi] += (b-a)/2*wl[jj]*ker*sqrt(1-x^2)
                end
            end
        
            a, b = 1.0/2, 1.0
            for jj in 1:n
                x = (a+b)/2+(b-a)/2*xj[jj]
                ker = IsMatFreq ? Spectral.kernelΩ(type, τ, Euv*x, β) : Spectral.kernelT(type, τ, Euv*x, β)
                G[τi] += ((b-a)/2)^1.5*wj[jj]*ker*sqrt(1+x)
            end

            a, b = -1.0, -1.0/2
            for jj in 1:n
                x = (a+b)/2+(b-a)/2*(-xj[n-jj+1])
                ker = IsMatFreq ? Spectral.kernelΩ(type, τ, Euv*x, β) : Spectral.kernelT(type, τ, Euv*x, β)
                G[τi] += ((b-a)/2)^1.5*wj[n-jj+1]*ker*sqrt(1-x)
            end

        end
        return G
    end

    g1=Green(24, IsMatFreq)
    g2=Green(48, IsMatFreq)
    err=abs.(g1-g2)
    
    println("Semi-circle case erro = ", maximum(err))
    return g2, err
end    

function MultiPole(type, Grid, β, Euv; IsMatFreq=false)
    poles=[-Euv, -0.2*Euv, 0.0, 0.8*Euv, Euv]
    # poles=[-Euv, -0.2*Euv, 0.1, 0.8*Euv, Euv]
    # poles = [0.0]
    g = IsMatFreq ? zeros(ComplexF64, length(Grid)) : zeros(Float64, length(Grid))
    for (τi, τ) in enumerate(Grid)
        for ω in poles
            if IsMatFreq==false
                g[τi] += Spectral.kernelT(type, τ, ω, β)
            else
                g[τi] += Spectral.kernelΩ(type, τ, ω, β)
            end
        end
    end
    return g, zeros(Float64, length(Grid))
end

@testset "Correlator Representation" begin

    function test(type; Euv, β, eps)
        printstyled("Testing DLR for $type, Euv=$Euv, β=$β, eps=$eps\n", color=:green)

        dlr = DLR.DLRGrid(type, 1Euv, β, eps)
        printstyled("Testing imaginary-time dlr\n", color=:green)
        # get imaginary-time Green's function
        Gdlr = zeros(Float64, (2, dlr.size))
        Gdlr[1, :] = SemiCircle(type, dlr.τ, β, Euv)[1]
        Gdlr[2, :] = MultiPole(type, dlr.τ, β, Euv)[1]

        # get imaginary-time Green's function for τ sample
        τSample=[t for t in LinRange(eps, β-eps, 100)]
        Gsample = zeros(Float64, (2, length(τSample)))
        Gsample[1, :] = SemiCircle(type, τSample, β, Euv)[1]
        Gsample[2, :] = MultiPole(type, τSample, β, Euv)[1]

        # imaginary-time to dlr
        coeff = DLR.tau2dlr(type, Gdlr, dlr, axis=2, rtol=eps)
        Gfitted = DLR.dlr2tau(type, coeff, dlr, τSample, axis=2)
        println(maximum(abs.(Gsample[1, :])), ", ", maximum(abs.(Gsample[1, :] - Gfitted[1, :])))
        println(maximum(abs.(Gsample[2, :])), ", ", maximum(abs.(Gsample[2, :] - Gfitted[2, :])))
        for (ti, t) in enumerate(τSample)
        @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", t / β, Gsample[2, ti],  Gfitted[2, ti], Gsample[2, ti] - Gfitted[2, ti])
    end
        @test all(abs.(Gsample - Gfitted) .< 10eps) # dlr should represent the Green's function up to accuracy of the order eps

        printstyled("Testing Matsubara frequency dlr\n", color=:green)
        # #get Matsubara-frequency Green's function
        Gndlr=zeros(Complex{Float64}, (2, dlr.size))
        Gndlr[1, :]=SemiCircle(type, dlr.n, β, Euv, IsMatFreq=true)[1]
        Gndlr[2, :]=MultiPole(type, dlr.n, β, Euv, IsMatFreq=true)[1]

        δ=2Euv*β/100
        nSample=[Int(n*δ) for n in -100:100]
        # nSample=[Int(floor(n)) for n in LinRange(-Int(2Euv*β), Int(2Euv*β), 200)]
        Gnsample=zeros(Complex{Float64}, (2, length(nSample)))
        Gnsample[1, :]=SemiCircle(type, nSample, β, Euv, IsMatFreq=true)[1]
        Gnsample[2, :]=MultiPole(type, nSample, β, Euv, IsMatFreq=true)[1]

        # #Matsubara frequency to dlr
        coeffn = DLR.matfreq2dlr(type, Gndlr, dlr, axis=2, rtol=eps)
        Gnfitted = DLR.dlr2matfreq(type, coeffn, dlr, nSample, axis=2)
    #     for (ni, n) in enumerate(nSample)
    #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", n, imag(Gnsample[1, ni]),  imag(Gnfitted[1, ni]), abs(Gnsample[1, ni] - Gnfitted[1, ni]))
    # end
    #     for (ni, n) in enumerate(nSample)
    #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", n, imag(Gnsample[2, ni]),  imag(Gnfitted[2, ni]), abs(Gnsample[2, ni] - Gnfitted[2, ni]))
    # end
        println(maximum(abs.(Gnsample-Gnfitted)))
        @test all(abs.(Gnsample - Gnfitted) .< 50eps) # dlr should represent the Green's function up to accuracy of the order eps

        # #imaginary-time to Matsubar-frequency (fourier transform)
        printstyled("Testing fourier transfer based on DLR\n", color=:green)
        Gnfourier = DLR.tau2matfreq(type, Gdlr, dlr, nSample,axis=2, rtol=eps)
        println("fourier tau to Matfreq error =",  maximum(abs.(Gnsample - Gnfourier)))
        @test all(abs.(Gnsample - Gnfourier) .< 500eps) # dlr should represent the Green's function up to accuracy of the order eps

        Gfourier = DLR.matfreq2tau(type, Gndlr, dlr, τSample, axis=2, rtol=eps)
        # for (ti, t) in enumerate(τSample)
        #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", t / β, Gsample[2, ti],  real(Gfourier[2, ti]), abs(Gsample[2, ti] - Gfourier[2, ti]))
        # end

        println("fourier Matfreq to tau error =",  maximum(abs.(Gsample - Gfourier)))
        @test all(abs.(Gsample - Gfourier) .< 500eps) # dlr should represent the Green's function up to accuracy of the order eps
    end

    test(:fermi, Euv=10.0, β=1000.0, eps=1e-10)

end