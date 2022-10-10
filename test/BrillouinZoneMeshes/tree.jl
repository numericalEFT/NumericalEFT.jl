@testset "Tree Grid" begin
    rng = MersenneTwister(1234)

    function dispersion(k)
        me = 0.5
        return dot(k, k) / 2me
    end

    function density(k)
        T = 0.01
        μ = 1.0

        ϵ = dispersion(k) - μ

        # return 1 / (exp((ϵ) / T) + 1.0)
        return (π * T)^2 / ((π * T)^2 + ϵ^2)
        # return (exp((ϵ) / T) / T)/(exp((ϵ) / T) + 1.0)^2
    end

    latvec = [2 0; 1 sqrt(3)]' .* (2π)
    tg = treegridfromdensity(k -> density(k), latvec; atol=1 / 2^10, maxdepth=5, mindepth=1, N=2)

    println("size:$(size(tg)),\t length:$(length(tg)),\t efficiency:$(efficiency(tg))")

    @testset "TreeGrid" begin

        # test node.index
        for node in PostOrderDFS(tg.root)
            if node.index[1] != 0
                origin1 = GridTree._calc_origin(node, tg.latvec)
                origin2 = tg.subgrids[node.index[1]].origin
                @test origin1 == origin2
            end
        end

        # test floor of GridNode
        for node in PostOrderDFS(tg.root)
            if node.index[1] != 0
                x = (node.pos .* 2.0 .+ 1.0) ./ 2^(node.depth + 1)
                index = floor(tg.root, x)
                @test index == node.index[1]
            end
        end

        # test floor of TreeGrid
        for i in 1:length(tg)
            p = tg[i]
            # this works only for non-boundary points of subgrids now
            # @test i == floor(tg, p)
        end

    end

    @testset "SymMap" begin
        atol = 1e-6
        smap = SymMap{Float64}(tg, k -> density(k); atol=atol)
        println(smap.map)
        println("compress:$(smap.reduced_length/length(smap.map))")

        vals = smap._vals

        # test map
        for i in 1:length(tg)
            @test isapprox(density(tg[i]), vals[smap.map[i]], atol=2atol)
        end

        # test inv_map
        for i in 1:length(smap.inv_map)
            val = density(tg[smap.inv_map[i][1]])
            for j in 1:length(smap.inv_map[i])
                @test isapprox(val, density(tg[smap.inv_map[i][j]]), atol=2atol)
            end
        end
    end

    @testset "Interp and Integrate" begin
        latvec = [1 0; 0 1]'
        tg = treegridfromdensity(k -> density(k), latvec; atol=1 / 2^10, maxdepth=5, mindepth=1, N=2)

        f(k) = k[1] + k[2]

        data = zeros(Float64, length(tg))
        for i in 1:length(tg)
            data[i] = f(tg[i])
        end

        Ntest = 5
        xlist = rand(rng, Ntest) .- 0.5
        ylist = rand(rng, Ntest) .- 0.5

        for x in xlist
            for y in ylist
                @test isapprox(f([x, y]), interp(data, tg, [x, y]), rtol=1e-2)
            end
        end

        # @test isapprox(0.0, integrate(data, tg), rtol=1e-2)

        tg = treegridfromdensity(k -> density(k), latvec;
            atol=1 / 2^10, maxdepth=5, mindepth=1, N=2, type=:barycheb)

        data = zeros(Float64, length(tg))
        for i in 1:length(tg)
            data[i] = f(tg[i])
        end

        Ntest = 5
        xlist = rand(rng, Ntest) .- 0.5
        ylist = rand(rng, Ntest) .- 0.5

        for x in xlist
            for y in ylist
                @test isapprox(f([x, y]), interp(data, tg, [x, y]), rtol=1e-2)
            end
        end

        # @test isapprox(0.0, integrate(data, tg), rtol=1e-2)
    end

end
