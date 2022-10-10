@testset "BaryCheb" begin
    println("Testing BaryCheb")

    @testset "1D BaryCheb Tools" begin
        n = 4
        x, w = BaryCheb.barychebinit(n)
        println(x)
        println(w)

        f(t) = t
        F(t) = 0.5*t^2

        data = f.(x)

        # test interp
        @test BaryCheb.barycheb(n, 0.5, data, w, x) ≈ f(0.5)

        # test integrate
        vmat = BaryCheb.vandermonde(x)
        println("vandermonde:",vmat)
        invmat = inv(transpose(vmat))
        println("invmat:",invmat)
        x1, x2 = -0.4, 0.0
        b = BaryCheb.weightcoef(x2, 1, n) - BaryCheb.weightcoef(x1, 1, n)
        println("b:",b)
        intw = BaryCheb.calcweight(invmat, b)
        println("intw:",intw)
        @test sum(intw .* data) ≈ F(x2) - F(x1)
        @test BaryCheb.chebint(n, x1, x2, data, invmat) ≈ F(x2) - F(x1)
    end

    @testset "1D BaryCheb Wrapper" begin
        n = 4
        bc = BaryCheb.BaryCheb1D(n)

        f(t) = t
        F(t) = 0.5*t^2

        data = f.(bc.x)

        # test interp
        @test BaryCheb.interp1D(data, bc, 0.5) ≈ f(0.5)

        # test integrate
        x1, x2 = -0.4, 0.0
        @test BaryCheb.integrate1D(data, bc, x1=x1, x2=x2) ≈ F(x2) - F(x1)
    end

    @testset "Testing 1D BaryCheb Integral convergence" begin
        f(t) = cos(t)
        F(t) = sin(t)

        x0 = 1.5
        g(t) = log(t + x0)
        G(t) = (t + x0) * log(t + x0) - t

        Ni, Nf = 3, 12
        for n in Ni:Nf
            bc = BaryCheb.BaryCheb1D(n)

            data1 = f.(bc.x)
            analytic1 = F(1) - F(-1)
            numeric1 = BaryCheb.integrate1D(data1, bc)

            data2 = g.(bc.x)
            analytic2 = G(1) - G(-1)
            numeric2 = BaryCheb.integrate1D(data2, bc)

            # println("n=$n")
            # println("f(x)=cos(x), $analytic1 <-> $numeric1, diff:$(abs(analytic1-numeric1)); f(x)=ln(x+x0), $analytic2 <-> $numeric2, diff:$(abs(analytic2-numeric2))")
            # println("f(x)=ln(x+1), $analytic2 <-> $numeric2, diff:$(abs(analytic2-numeric2))")
            @test isapprox(analytic1, numeric1, rtol = 10^(- 0.5n))
        end

    end

    @testset "ND BaryCheb Tools" begin
        DIM = 2
        n = 4

        x, w = BaryCheb.barychebinit(n)

        f(x1, x2) = x1 + x2
        F(x1, x2) = 0.5 * (x1 + x1) * x1 * x2

        data = zeros(Float64, (n, n))
        for i1 in 1:n
            for i2 in 1:n
                data[i1, i2] = f(x[i1], x[i2])
            end
        end

        @test isapprox(BaryCheb.barychebND(n, [0.5, 0.5], data, w, x, DIM), f(0.5, 0.5), rtol = 1e-10)
        @test isapprox(BaryCheb.barychebND(n, [x[2], 0.5], data, w, x, DIM), f(x[2], 0.5), rtol = 1e-10)
        @test isapprox(BaryCheb.barychebND(n, [x[2], x[1]], data, w, x, DIM), f(x[2], x[1]), rtol = 1e-10)
    end

    @testset "ND BaryCheb" begin
        DIM = 2
        n = 4
        bc = BaryCheb.BaryCheb1D(n)

        f(x1, x2) = x1 + x2
        F(x1, x2) = 0.5 * (x1 + x2) * x1 * x2

        data = zeros(Float64, (n, n))
        Data = zeros(Float64, (n, n))
        for i1 in 1:n
            for i2 in 1:n
                data[i1, i2] = f(bc.x[i1], bc.x[i2])
                Data[i1, i2] = F(bc.x[i1], bc.x[i2])
            end
        end

        @test isapprox(BaryCheb.interpND(data, bc, [0.4, 0.7]), f(0.4, 0.7), rtol = 1e-10)
        @test isapprox(BaryCheb.interpND(data, bc, [bc.x[2], 0.5]), f(bc.x[2], 0.5), rtol = 1e-10)
        @test isapprox(BaryCheb.interpND(data, bc, [bc.x[2], bc.x[1]]), f(bc.x[2], bc.x[1]), rtol = 1e-10)

        x1s = [0.0, 0.0]
        x2s = [0.4, 0.7]
        @test isapprox(BaryCheb.integrateND(data, bc, x1s, x2s), F(x2s...), rtol = 1e-6)
    end

end
