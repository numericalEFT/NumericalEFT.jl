@testset "GreenFunc" begin
    @testset "MeshProduct" begin

        N1, N2 = 5, 7
        mesh1 = SimpleGrid.Uniform{Float64}([0.0, 1.0], N1)
        mesh2 = SimpleGrid.Uniform{Float64}([0.0, 1.0], N2)
        X = 3
        Y = 4
        I = 18
        X0 = 1
        Y0 = 1
        I0 = 1
        meshprod = MeshProduct(mesh1, mesh2)

        println("test meshprod: ", meshprod)

        @test meshprod isa MeshProduct{Tuple{typeof(mesh1),typeof(mesh2)}}
        # @test GreenFunc.rank(meshprod) == 2
        @test size(meshprod, 1) == N1
        @test size(meshprod, 2) == N2
        @test size(meshprod) == (N1, N2)
        @test length(meshprod) == N1 * N2
        # @inferred meshprod[1]
        # @inferred meshprod[2]

        @test eltype(typeof(meshprod)) == (eltype(typeof(mesh1)), eltype(typeof(mesh2)))

        function test_linear_index(mp::MeshProduct, x, y, i)
            #make sure return type is stable
            @inferred MeshGrids.index_to_linear(mp, x, y)
            @inferred MeshGrids.linear_to_index(mp, i) == (x, y)
            @inferred mp[x, y]
            @inferred mp[i]

            @test MeshGrids.index_to_linear(mp, x, y) == i
            @test MeshGrids.linear_to_index(mp, i) == (x, y)
            @test mp[x, y] == mp[i]
            @test mp[x, y] == (mesh1[x], mesh2[y])
            # println(mp[i])

            xp, yp = mp[x, y]
            @inferred locate(mp, xp / 3, yp / 3)
            @inferred volume(mp, x, y)
            @inferred volume(mp, i)
            @test locate(mp, xp / 3, yp / 3) == (locate(mp.meshes[1], xp / 3), locate(mp.meshes[2], yp / 3))

            @test volume(mp, x, y) == volume(mesh1, x) * volume(mesh2, y)
            @test volume(mp, x, y) == volume(mp, i)
        end

        test_linear_index(meshprod, X, Y, I)
        test_linear_index(meshprod, X0, Y0, I0)
        test_linear_index(meshprod, N1, N2, N1 * N2)
        #test iterator
        @test (meshprod[I] in meshprod) == true
        for item in meshprod
            # println(item)
        end

        # io = open("test_meshprod.txt", "w")
        # show(io,meshprod)
        # close(io)
        # println(typeof(meshprod))
        # println("rank of meshprod is $(GreenFunc.rank(meshprod))")
        # println("size of mesh1 is $(size(meshprod,1)), size of mesh2 is $(size(meshprod,2))")
        # println("size of meshprod is $(size(meshprod)), length of meshprod is $(length(meshprod))")
        # println("test index_to_linear for index(($x),($y)):$(GreenFunc.index_to_linear(meshprod,x,y))")
        # println("test linear_to_index for I=$(I):$(GreenFunc.index_to_linear(meshprod,I))")
        # println("test getindex with index input:\n", meshprod[x, y])
        # println("test getindex with linearindex input:\n", meshprod[I])

        # io = open("myfile.txt", "r")
    end
end

