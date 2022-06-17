@testset "Chebyshev" begin
    @testset "BaryCheb Interp" begin
        n = 4
        x, w = SimpleGrid.barychebinit(n)
        println(x)
        println(w)

        f(t) = t
        F(t) = 0.5 * t^2

        data = f.(x)

        # test interp
        @test SimpleGrid.barycheb(n, 0.5, data, w, x) ≈ f(0.5)

        # test integrate
        vmat = SimpleGrid.vandermonde(x)
        println("vandermonde:", vmat)
        invmat = inv(transpose(vmat))
        println("invmat:", invmat)
        x1, x2 = -0.4, 0.0
        b = SimpleGrid.weightcoef(x2, 1, n) - SimpleGrid.weightcoef(x1, 1, n)
        println("b:", b)
        intw = SimpleGrid.calcweight(invmat, b)
        println("intw:", intw)
        @test sum(intw .* data) ≈ F(x2) - F(x1)
        @test SimpleGrid.chebint(n, x1, x2, data, invmat) ≈ F(x2) - F(x1)

        Data = F.(x)
        x1 = 0.4
        @test isapprox(SimpleGrid.chebdiff(n, x1, Data, invmat), f(x1), rtol=1e-6)
    end

end
