@testset "BaryCheb" begin
    # println("Testing BaryCheb")

    @testset "1D BaryCheb Tools" begin
        n = 4
        x, w = BaryChebTools.barychebinit(n)
        # println(x)
        # println(w)

        f(t) = t
        F(t) = 0.5 * t^2

        data = f.(x)

        # test interp
        @test BaryChebTools.barycheb(n, 0.5, data, w, x) ≈ f(0.5)

        # test integrate
        vmat = BaryChebTools.vandermonde(x)
        # println("vandermonde:",vmat)
        invmat = inv(transpose(vmat))
        # println("invmat:",invmat)
        x1, x2 = -0.4, 0.0
        b = BaryChebTools.weightcoef(x2, 1, n) - BaryChebTools.weightcoef(x1, 1, n)
        # println("b:",b)
        intw = BaryChebTools.calcweight(invmat, b)
        # println("intw:",intw)
        @test sum(intw .* data) ≈ F(x2) - F(x1)
        @test BaryChebTools.chebint(n, x1, x2, data, invmat) ≈ F(x2) - F(x1)
    end

    @testset "1D BaryCheb Wrapper" begin
        n = 4
        bc = BaryChebTools.BaryCheb1D(n)

        f(t) = t
        F(t) = 0.5 * t^2

        data = f.(bc.x)

        # test interp
        @test BaryChebTools.interp1D(data, bc, 0.5) ≈ f(0.5)

        # test integrate
        x1, x2 = -0.4, 0.0
        @test BaryChebTools.integrate1D(data, bc, x1=x1, x2=x2) ≈ F(x2) - F(x1)
    end

    @testset "ND BaryCheb Tools" begin
        DIM = 2
        n = 4

        x, w = BaryChebTools.barychebinit(n)

        f(x1, x2) = x1 + x2
        F(x1, x2) = 0.5 * (x1 + x1) * x1 * x2

        data = zeros(Float64, (n, n))
        for i1 in 1:n
            for i2 in 1:n
                data[i1, i2] = f(x[i1], x[i2])
            end
        end

        @test isapprox(BaryChebTools.barychebND(n, [0.5, 0.5], data, w, x, DIM), f(0.5, 0.5), rtol=1e-10)
        @test isapprox(BaryChebTools.barychebND(n, [x[2], 0.5], data, w, x, DIM), f(x[2], 0.5), rtol=1e-10)
        @test isapprox(BaryChebTools.barychebND(n, [x[2], x[1]], data, w, x, DIM), f(x[2], x[1]), rtol=1e-10)
    end

    @testset "ND BaryCheb" begin
        DIM = 2
        n = 4
        bc = BaryChebTools.BaryCheb1D(n)

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

        @test isapprox(BaryChebTools.interpND(data, bc, [0.4, 0.7]), f(0.4, 0.7), rtol=1e-10)
        @test isapprox(BaryChebTools.interpND(data, bc, [bc.x[2], 0.5]), f(bc.x[2], 0.5), rtol=1e-10)
        @test isapprox(BaryChebTools.interpND(data, bc, [bc.x[2], bc.x[1]]), f(bc.x[2], bc.x[1]), rtol=1e-10)

        x1s = [0.0, 0.0]
        x2s = [0.4, 0.7]
        @test isapprox(BaryChebTools.integrateND(data, bc, x1s, x2s), F(x2s...), rtol=1e-6)
    end

end
