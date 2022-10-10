using Random

@testset "MeshArray" begin
    function test_shape(N1, N2, innermesh)
        ############# basic test ################
        mesh1 = SimpleGrid.Uniform{Float64}([0.0, 1.0], N1)
        mesh2 = SimpleGrid.Uniform{Float64}([0.0, 1.0], N2)
        if isempty(innermesh)
            g = MeshArray(mesh1, mesh2)
            @test length(g) == N1 * N2
        else
            g = MeshArray(innermesh..., mesh1, mesh2)
            @test length(g) == N1 * N2 * reduce(*, length.(innermesh))
        end
        show(g)
        @test size(g) == (length.(innermesh)..., N1, N2)
        @test eltype(typeof(g)) == Float64
        gc = similar(g, ComplexF64)
        @test eltype(typeof(gc)) == ComplexF64

        ############ broadcast test ###################
        rand!(g.data)
        if isempty(innermesh)
            g2 = MeshArray(mesh1, mesh2; data=rand(g.dims...))
        else
            g2 = MeshArray(innermesh..., mesh1, mesh2; data=rand(g.dims...))
        end

        MeshArrays._check(g, g2) #check if the two GreenFuncs have the same shape

        # sum/minus/mul/div
        g3 = g .+ g2
        @test g3.data ≈ g.data .+ g2.data
        @time g3 = g .+ g2 #call similar(broadcaststyle...) to create one copy
        @time g3 = g .+ g2.data #call similar(broadcaststyle...) to create one copy

        g4 = g .- g2
        @test g4.data ≈ g.data .- g2.data

        g5 = g .* g2
        @test g5.data ≈ g.data .* g2.data

        g6 = g ./ g2
        @test g6.data ≈ g.data ./ g2.data

        # inplace operation
        _g = deepcopy(g) # store a copy of g first

        g = deepcopy(_g)
        g .+= g2
        @test _g.data .+ g2.data ≈ g.data

        g = deepcopy(_g)
        g .+= g2
        println(".+= test time")
        @time g .+= g2
        @time g .+= g2.data

        g = deepcopy(_g)
        g .-= g2
        @test _g.data .- g2.data ≈ g.data

        g = deepcopy(_g)
        g .*= 2.0
        @test _g.data .* 2.0 ≈ g.data

        g = deepcopy(_g)
        g .*= g2
        @test _g.data .* g2.data ≈ g.data

        g = deepcopy(_g)
        g ./= 2.0
        @test _g.data ./ 2.0 ≈ g.data

    end

    test_shape(5, 7, ())
    test_shape(5, 7, (1:2, 1:3))
    test_shape(10, 140, (1:2, 1:3))
end
