@testset "Fast Math" begin
    x = 3.0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 1.0 / 3.0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 3.0f0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 1.0f0 / 3.0f0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5

    # semicircle -1<ω<1
#     function S(ω)
#     if -1.0 < ω < 1.0
#         return sqrt(1.0 - ω^2)
#     else
#         return 0.0
#     end
# end

#     result, err = FastMath.integrate(S, -1.0, 1.0, :cuhre)
#     @test abs(result - π / 2) < 3err
#     result, err = FastMath.integrate(S, -Inf, 1.0, :cuhre)
#     @test abs(result - π / 2) < 3err
#     result, err = FastMath.integrate(S, -1.0, Inf, :cuhre)
#     @test abs(result - π / 2) < 3err
#     result, err = FastMath.integrate(S, -Inf, Inf, :cuhre)
#     @test abs(result - π / 2) < 3err

    # k = MVector{3,Float64}([1.0, 2.0, 3.0])
    # q = MVector{3,Float64}([3.0, 1.0, 4.0])
    # println(FastMath.dot(k, q))
    # @test FastMath.dot(k, q) ≈ LinearAlgebra.dot(k, q)
    # @test FastMath.norm(k) ≈ LinearAlgebra.norm(k)
    # @test FastMath.squaredNorm(k) ≈ LinearAlgebra.dot(k, k)
end
