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
        check(uniform, 2:uniform.size-1, δ, 0)
        check(uniform, 2:uniform.size-1, -δ, -1)

        @test floor(uniform, uniform[end]) == uniform.size - 1
        @test floor(uniform, 1.0) == uniform.size - 1

        uniform = Grid.Uniform{Float64,2}(0.0, 1.0, (true, true))
        @test floor(uniform, 0.0) == 1
        @test floor(uniform, uniform[1]) == 1

        δ = 1.0e-12
        check(uniform, 2:uniform.size-1, δ, 0)
        check(uniform, 2:uniform.size-1, -δ, -1)

        @test floor(uniform, uniform[end]) == uniform.size - 1
        @test floor(uniform, 1.0) == uniform.size - 1
    end


    @testset "Log Grid for Tau" begin
        β = 10.0
        tau = Grid.tau(β, 0.2β, 8)
        @test floor(tau, 0.0) == 1
        @test floor(tau, tau[1]) == 1 # tau[1]=0^+ is special

        δ = 1.0e-12
        check(tau, 2:tau.size-1, δ, 0)
        check(tau, 2:tau.size-1, -δ, -1)

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
        check(K, 2:K.size-1, δ, 0)
        check(K, 2:K.size-1, -δ, -1)

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
        check(K, 2:K.size-1, δ, 0)
        check(K, 2:K.size-1, -δ, -1)

        @test floor(K, K[end] - δ) == K.size - 1
        @test floor(K, K[end]) == K.size - 1
        @test floor(K, maxK) == K.size - 1
    end

end


@testset "Interpolate" begin

    @testset "Linear2D" begin
        β, kF, maxK = 10.0, 1.0, 3.0
        Nt, Nk = 8, 7
        tgrid = Grid.tau(β, 0.2β, Nt)
        kgrid = Grid.boseK(kF, maxK, 0.2kF, Nk)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(k, t) = k + t * 1.1
        data = zeros((Nk, Nt))

        for (ti, t) in enumerate(tgrid.grid)
            for (ki, k) in enumerate(kgrid.grid)
                data[ki, ti] = f(k, t)
            end
        end

        for ti = 1:tgrid.size-1
            for ki = 1:kgrid.size-1
                t = tgrid[ti] + 1.e-6
                k = kgrid[ki] + 1.e-6
                fbar = Interpolate.linear2D(data, kgrid, tgrid, k, t)
                @test abs(f(kgrid[ki], tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
                @test f(kgrid[ki], tgrid[ti]) < fbar
                @test f(kgrid[ki], tgrid[ti+1]) > fbar
                @test f(kgrid[ki+1], tgrid[ti]) > fbar
                @test f(kgrid[ki+1], tgrid[ti+1]) > fbar
            end
        end

        for ti = 2:tgrid.size
            for ki = 2:kgrid.size
                t = tgrid[ti] - 1.e-6
                k = kgrid[ki] - 1.e-6
                fbar = Interpolate.linear2D(data, kgrid, tgrid, k, t)
                @test abs(f(kgrid[ki], tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
                @test f(kgrid[ki], tgrid[ti]) > fbar
                @test f(kgrid[ki], tgrid[ti-1]) < fbar
                @test f(kgrid[ki-1], tgrid[ti]) < fbar
                @test f(kgrid[ki-1], tgrid[ti-1]) < fbar
            end
        end

        tlist = rand(10) * β
        klist = rand(10) * maxK
        # println(tlist)

        for (ti, t) in enumerate(tlist)
            for (ki, k) in enumerate(klist)
                fbar = Interpolate.linear2D(data, kgrid, tgrid, k, t)
                # println("$k, $t, $fbar, ", f(k, t))
                @test abs(f(k, t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            end
        end
    end
end

include("montecarlo.jl")

# include("yeppp.jl")

# @testset "Fast Math" begin
#     x = 3.0
#     @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
#     x = 1.0 / 3.0
#     @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
#     x = 3.0f0
#     @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5
#     x = 1.0f0 / 3.0f0
#     @test FastMath.invsqrt(x) ≈ 1.0 / sqrt(x) rtol = 1.0e-5

#     using StaticArrays, LinearAlgebra
#     k = MVector{3,Float64}([1.0, 2.0, 3.0])
#     q = MVector{3,Float64}([3.0, 1.0, 4.0])
#     @test FastMath.dot(k, q) ≈ LinearAlgebra.dot(k, q)
#     @test FastMath.norm(k) ≈ LinearAlgebra.norm(k)
#     @test FastMath.squaredNorm(k) ≈ LinearAlgebra.dot(k, k)
# end
