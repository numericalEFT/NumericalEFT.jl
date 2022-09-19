SemiCircle(dlr, grid, type) = Sample.SemiCircle(dlr.Euv, dlr.β, dlr.isFermi, grid, type, dlr.symmetry; rtol = dlr.rtol, degree = 24, regularized = true)

@testset "GreenFunc" begin
    # @testset "Green2" begin
    #     tgrid = [0.0,1.0]
    #     sgrid = [0.0,1.0]
    #     color_n = [0.0,1.0]
    #     beta = 20.0
    #     green_simple = GreenBasic.Green2{Float64}(:freq,:mom,true,:ph,nothing,beta,color_n,tgrid,sgrid)
    #     println(green_simple.dynamic)        
    # end

    @testset "Green2DLR" begin
        sgrid = [0.0, 1.0]
        color_n = [0.0]
        β = 10.0
        isFermi = true
        Euv = 1000.0

        green_freq = Green2DLR{ComplexF64}(:green, GreenFunc.IMFREQ, β, isFermi, Euv, sgrid)
        rtol = green_freq.dlrGrid.rtol
        println(green_freq.timeType)
        Gτ = SemiCircle(green_freq.dlrGrid, green_freq.dlrGrid.τ, :τ)
        Gn = SemiCircle(green_freq.dlrGrid, green_freq.dlrGrid.n, :n)
        green_dum = zeros(ComplexF64, (green_freq.color, green_freq.color, green_freq.spaceGrid.size, green_freq.timeGrid.size))
        for (ti, t) in enumerate(green_freq.timeGrid)
            for (qi, q) in enumerate(green_freq.spaceGrid)
                for (c1i, c1) in enumerate(color_n)
                    for (c2i, c2) in enumerate(color_n)
                        green_dum[c1i, c2i, qi, ti] = Gn[ti]
                    end
                end
            end
        end
        green_freq.dynamic = green_dum
        green_tau = toTau(green_freq)
        err = maximum(abs.(green_tau.dynamic[1, 1, 1, :] .- Gτ))
        printstyled("SemiCircle Fourier ωn->τ $err\n", color = :white)
        @test err < 50 * rtol

        green_freq_compare = toMatFreq(green_tau)
        err = maximum(abs.(green_freq_compare.dynamic[1, 1, 1, :] .- Gn))
        printstyled("SemiCircle Fourier τ->ωn $err\n", color = :white)
        @test err < 50 * rtol

        green_dlr = toDLR(green_freq)
        green_tau = toTau(green_dlr)
        err = maximum(abs.(green_tau.dynamic[1, 1, 1, :] .- Gτ))
        printstyled("SemiCircle Fourier ωn->dlr->τ $err\n", color = :white)
        @test err < 50 * rtol

        #test JLD2
        ############# FileIO API #################
        save("example.jld2", Dict("green" => green_freq), compress = true)
        d = load("example.jld2")
        green_read = d["green"]
        @test green_read.dynamic == green_freq.dynamic
        #deeptest(green_read, green_freq)
        rm("example.jld2")
    end

    @testset "find" begin
        sgrid = [0.0, 1.0]
        color_n = [0.0]
        β = 10.0
        isFermi = true
        Euv = 1000.0

        green_linear = Green2DLR{ComplexF64}(:green, GreenFunc.IMFREQ, β, isFermi, Euv, sgrid)
        rtol = green_linear.dlrGrid.rtol
        green_dum = zeros(Float64, (green_linear.color, green_linear.color, green_linear.spaceGrid.size, green_linear.timeGrid.size))
        for (ti, t) in enumerate(green_linear.timeGrid)
            for (qi, q) in enumerate(green_linear.spaceGrid)
                for (c1i, c1) in enumerate(color_n)
                    for (c2i, c2) in enumerate(color_n)
                        green_dum[c1i, c2i, qi, ti] = (2*t+1)*π/β * q
                    end
                end
            end
        end

        green_linear.dynamic = green_dum
        green_dum_ins = zeros(Float64, (green_linear.color, green_linear.color, green_linear.spaceGrid.size))
        for (qi, q) in enumerate(green_linear.spaceGrid)
            for (c1i, c1) in enumerate(color_n)
                for (c2i, c2) in enumerate(color_n)
                    green_dum_ins[c1i, c2i, qi] = q
                end
            end
        end

        green_linear.instant = green_dum_ins
        τ = 5
        x = 0.3
        interp_dym = dynamic(green_linear, τ, x)
        @test abs(interp_dym -  (2*τ+1)*π/β* x)< 5e-8
        interp_ins = instant(green_linear, x)
        @test abs(interp_ins - x) < 5e-8
        interp_dym = dynamic(green_linear, τ, x, GreenFunc.DEFAULTINTERP, GreenFunc.DEFAULTINTERP)
        @test abs(interp_dym - (2*τ+1)*π/β * x) < 5e-8
        interp_dym = dynamic(green_linear, τ, x, GreenFunc.DLRINTERP, GreenFunc.DEFAULTINTERP)
        @test abs(interp_dym - (2*τ+1)*π/β * x) < 5e-6
    end
end

