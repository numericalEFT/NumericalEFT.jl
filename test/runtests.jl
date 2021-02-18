using QuantumStatistics, Test
# import Test: @test, @testset

@testset "Green's functions" begin
    β, τ, ε = 10.0, 1.0, 1.0
    n(ε) = 1.0 / (1.0 + exp(β * ε))
    @test Green.FermiDirac(β, ε) ≈ n(ε)

    @test Green.bareFermi(β, τ, 0.0) ≈ n(0.0)
    @test Green.bareFermi(β, eps(0.0), ε) ≈ 1.0 - n(ε)

    @test Green.bareFermi(β, 0.0, ε) ≈ -n(ε) # τ=0.0 should be treated as the 0⁻
    @test Green.bareFermi(β, -eps(0.0), ε) ≈ -n(ε)

    @test Green.bareFermi(β, -τ, ε) ≈ -Green.bareFermi(β, β - τ, ε)
    @test Green.bareFermi(β, -eps(0.0), 1000.0) ≈ 0.0
    @test Green.bareFermi(β, -eps(0.0), -1000.0) ≈ -1.0
end

@testset "Grids" begin

    # with a shift to the grid element, check if produce the correct floored index
    function check(grid, range, shift, idx_shift)
        for i in range
            @test(floor(grid, grid[i] + shift) == i + idx_shift)
            # if floor(grid, grid[i] + shift) != i + idx_shift
            #     return false
            # end
        end
        return true
    end

    @testset "UniformGrid" begin
        uniform = Grid.Uniform{Float64,4}(0.0, 1.0, (true, true))
        @test floor(uniform, 0.0) == 1
        @test floor(uniform, uniform[1]) == 1

        δ = 1.0e-12
        check(uniform, 2:uniform.size - 1, δ, 0)
        check(uniform, 2:uniform.size - 1, -δ, -1)

        @test floor(uniform, uniform[end]) == uniform.size - 1
        @test floor(uniform, 1.0) == uniform.size - 1

        uniform = Grid.Uniform{Float64,2}(0.0, 1.0, (true, true))
        @test floor(uniform, 0.0) == 1
        @test floor(uniform, uniform[1]) == 1

        δ = 1.0e-12
        check(uniform, 2:uniform.size - 1, δ, 0)
        check(uniform, 2:uniform.size - 1, -δ, -1)

        @test floor(uniform, uniform[end]) == uniform.size - 1
        @test floor(uniform, 1.0) == uniform.size - 1
    end


    @testset "Log Grid for Tau" begin
        β = 10.0
        tau = Grid.tau(β, 0.2β, 8)
        @test floor(tau, 0.0) == 1
        @test floor(tau, tau[1]) == 1 # tau[1]=0^+ is special

        δ = 1.0e-12
        check(tau, 2:tau.size - 1, δ, 0)
        check(tau, 2:tau.size - 1, -δ, -1)

        @test floor(tau, tau[end]) == tau.size - 1
        @test floor(tau, β) == tau.size - 1
    end

    @testset "Log Grid for fermiK" begin
        kF, maxK, halfLife = 1.0, 3.0, 0.5
        K = Grid.fermiK(kF, maxK, halfLife, 16)
        # println(K.grid)
        @test floor(K, 0.0) == 1
        @test floor(K, K[1]) == 1

        δ = 1.0e-12
        check(K, 2:K.size - 1, δ, 0)
        check(K, 2:K.size - 1, -δ, -1)

        @test floor(K, K[end] - δ) == K.size - 1
        @test floor(K, K[end]) == K.size - 1
        @test floor(K, maxK) == K.size - 1
    end

    @testset "Log Grid for boseK" begin
        kF, maxK, halfLife = 1.0, 3.0, 0.5
        K = Grid.boseK(kF, maxK, halfLife, 16)
        # println(K.grid)
        @test floor(K, 0.0) == 1
        @test floor(K, K[1]) == 1

        δ = 1.0e-12
        check(K, 2:K.size - 1, δ, 0)
        check(K, 2:K.size - 1, -δ, -1)

        @test floor(K, K[end] - δ) == K.size - 1
        @test floor(K, K[end]) == K.size - 1
        @test floor(K, maxK) == K.size - 1
    end

end

include("yeppp.jl")

@testset "Fast Math" begin
    x = 3.0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 1.0 / 3.0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 3.0f0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
    x = 1.0f0 / 3.0f0
    @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5

    using StaticArrays, LinearAlgebra
    k = MVector{3,Float64}([1.0, 2.0, 3.0])
    q = MVector{3,Float64}([3.0, 1.0, 4.0])
    @test FastMath.dot(k, q) ≈ LinearAlgebra.dot(k, q)
    @test FastMath.norm(k) ≈ LinearAlgebra.norm(k)
    @test FastMath.squaredNorm(k) ≈ LinearAlgebra.dot(k, k)
end
