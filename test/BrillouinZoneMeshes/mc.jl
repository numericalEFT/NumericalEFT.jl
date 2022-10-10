@testset "MC" begin
    # testing locate and volume functions provided for monte carlo
    rng = MersenneTwister(1234)

    function test_mc_histogram(mesh::AbstractMesh{DIM};
                               Npp=1e5, f = (x -> sum(x))) where {DIM}
        Ng = length(mesh)
        Nmc = Npp * Ng

        hist = zeros(Ng)
        for i in 1:Nmc
            pos = rand(rng, DIM)
            x = Vector(mesh.origin) .+ sum(mesh.latvec[:, i] .* pos[i] for i in 1:DIM)
            ind = locate(mesh, x)
            vol = volume(mesh, ind)
            hist[ind] += f(x) / vol * volume(mesh)
        end

        for (pi, p) in enumerate(mesh)
            vi = volume(mesh, pi)
            vall = volume(mesh)

            @test isapprox(hist[pi] / Nmc, f(p),
                          rtol = 5/sqrt(Nmc*vi/vall), atol=5*(vi/vall)^(1/DIM))
        end
    end

    @testset "UniformMesh Centered" begin
        # test 2d
        δ = 1e-5
        N, DIM = 4, 2
        origin = [0.0 0.0]
        latvec = [2 0; 1 sqrt(3)]'

        umesh = UniformMesh{DIM,N}(origin, latvec)

        for i in 1:length(umesh)
            p = umesh[i]
            @test i == locate(umesh, p)
            @test i == locate(umesh, [p[1], p[2] + δ])
            @test i == locate(umesh, [p[1], p[2] - δ])
            @test i == locate(umesh, [p[1] + δ, p[2]])
            @test i == locate(umesh, [p[1] - δ, p[2]])
        end

        @test volume(umesh) ≈ sum(volume(umesh, i) for i in 1:length(umesh))

        test_mc_histogram(umesh)
    end

    @testset "UniformMesh Edged" begin
        # test 2d
        δ = 1e-5
        N, DIM = 4, 2
        origin = [0.0 0.0]
        latvec = [2 0; 1 sqrt(3)]'

        umesh = UniformMesh{DIM,N,EdgedMesh}(origin, latvec)

        for i in 1:length(umesh)
            p = umesh[i]
            @test i == locate(umesh, p)
            @test i == locate(umesh, [p[1], p[2] + δ])
            @test i == locate(umesh, [p[1], p[2] - δ])
            @test i == locate(umesh, [p[1] + δ, p[2]])
            @test i == locate(umesh, [p[1] - δ, p[2]])
        end

        @test volume(umesh) ≈ sum(volume(umesh, i) for i in 1:length(umesh))

        test_mc_histogram(umesh)
    end

    @testset "BaryChebMesh" begin
        # test 2d
        δ = 1e-5
        N, DIM = 4, 2
        origin = [0.0 0.0]
        latvec = [2 0; 1 sqrt(3)]'

        umesh = BaryChebMesh(origin, latvec, DIM, N)

        for i in 1:length(umesh)
            p = umesh[i]
            @test i == locate(umesh, p)
            @test i == locate(umesh, [p[1], p[2] + δ])
            @test i == locate(umesh, [p[1], p[2] - δ])
            @test i == locate(umesh, [p[1] + δ, p[2]])
            @test i == locate(umesh, [p[1] - δ, p[2]])
        end

        @test volume(umesh) ≈ sum(volume(umesh, i) for i in 1:length(umesh))

        test_mc_histogram(umesh)
    end

    @testset "Tree Grid" begin
        # test 2d
        δ = 1e-5
        N, DIM = 2, 2
        origin = [0.0 0.0]
        latvec = [2 0; 1 sqrt(3)]'

        isfine(depth, pos) = false
        umesh = uniformtreegrid(isfine, latvec;maxdepth=2,mindepth=2,DIM=DIM,N=N)

        for i in 1:length(umesh)
            p = umesh[i]
            @test i == locate(umesh, p)
            @test i == locate(umesh, [p[1], p[2] + δ])
            @test i == locate(umesh, [p[1], p[2] - δ])
            @test i == locate(umesh, [p[1] + δ, p[2]])
            @test i == locate(umesh, [p[1] - δ, p[2]])
        end

        @test volume(umesh) ≈ sum(volume(umesh, i) for i in 1:length(umesh))

        test_mc_histogram(umesh)
    end


end
