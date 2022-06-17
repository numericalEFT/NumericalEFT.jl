@testset "Interpolate" begin
    @testset "Linear1D" begin
        β = π
        tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 33)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t) = t
        data = zeros(tgrid.size)

        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end

        for ti = 1:tgrid.size - 1
            t = tgrid[ti] + 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) < fbar
            @test f(tgrid[ti + 1]) > fbar
        end
        for ti = 2:tgrid.size
            t = tgrid[ti] - 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) > fbar
            @test f(tgrid[ti - 1]) < fbar
        end

        t = tgrid[1] + eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        t = tgrid[tgrid.size] - eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        tlist = rand(10) * β
        # println(tlist)

        for (ti, t) in enumerate(tlist)
            fbar = Interp.interp1D(data, tgrid, t)
            # println("$k, $t, $fbar, ", f(k, t))
            @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
        end

        data2 = zeros((2,tgrid.size))

        for i in 1:2
            for (ti, t) in enumerate(tgrid.grid)
                data2[i,ti] = f(t)
            end
        end
        tlist = rand(10) * β

        fbars=Interp.interp1DGrid(data2, tgrid, tlist, axis=2)
        for (ti, t) in enumerate(tlist)
            fbar = Interp.interp1D(data2, tgrid, t, axis=2)
            @test abs(f(t) - fbar[1]) < 3.e-6 # linear interpolation, so error is δK+δt
            @test abs(f(t) - fbar[2]) < 3.e-6 # linear interpolation, so error is δK+δt
            @test abs(f(t) - fbars[1, ti]) < 3.e-6 # linear interpolation, so error is δK+δt
            @test abs(f(t) - fbars[2, ti]) < 3.e-6 # linear interpolation, so error is δK+δt
        end

    end

    @testset "LinearND" begin
        β = π
        tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 22)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t1,t2) = 1.0+t1+t2+t1*t2
        data = zeros((tgrid.size,tgrid.size))

        for (ti, t) in enumerate(tgrid.grid)
            for (di, d) in enumerate(tgrid.grid)
                data[ti, di] = f(t,d)
            end
        end

        for ti = 1:tgrid.size - 1
            for di = 1:tgrid.size - 1
                t = tgrid[ti] + 1.e-6
                d = tgrid[di] + 1.e-6
                fbar = Interp.linearND(data, [tgrid,tgrid], [t,d])
                @test abs(f(tgrid[ti],tgrid[di]) - fbar) < 9.e-6 # linear interpolation, so error is dδt+tδd+δt+δd
                @test f(tgrid[ti],tgrid[di]) < fbar
                @test f(tgrid[ti + 1],tgrid[di+1]) > fbar
            end
        end
        for ti = 2:tgrid.size
            for di = 2:tgrid.size
                t = tgrid[ti] - 1.e-6
                d = tgrid[di] - 1.e-6
                fbar = Interp.linearND(data, [tgrid,tgrid], [t,d])
                @test abs(f(tgrid[ti],tgrid[di]) - fbar) < 9.e-6 # linear interpolation, so error is dδt+tδd+δt+δd
                @test f(tgrid[ti],tgrid[di]) > fbar
                @test f(tgrid[ti - 1],tgrid[di - 1]) < fbar
            end
        end

        tlist = rand(10) * β
        dlist = rand(10) * β
        # println(tlist)

        for (ti, t) in enumerate(tlist)
            for (di, d) in enumerate(tlist)
                fbar = Interp.linearND(data, [tgrid,tgrid], [t,d])
                @test abs(f(t,d) - fbar) < 9.e-6 # linear interpolation, so error is δK+δt
            end
        end
    end

    @testset "interpND" begin
        β = π
        tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 22)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t1,t2) = 1.0+t1+t2+t1*t2
        data = zeros((tgrid.size,tgrid.size))

        for (ti, t) in enumerate(tgrid.grid)
            for (di, d) in enumerate(tgrid.grid)
                data[ti, di] = f(t,d)
            end
        end

        for ti = 1:tgrid.size - 1
            for di = 1:tgrid.size - 1
                t = tgrid[ti] + 1.e-6
                d = tgrid[di] + 1.e-6
                fbar = Interp.interpND(data, [tgrid,tgrid], [t,d])
                @test abs(f(tgrid[ti],tgrid[di]) - fbar) < 9.e-6 # linear interpolation, so error is dδt+tδd+δt+δd
                @test f(tgrid[ti],tgrid[di]) < fbar
                @test f(tgrid[ti + 1],tgrid[di+1]) > fbar
            end
        end
        for ti = 2:tgrid.size
            for di = 2:tgrid.size
                t = tgrid[ti] - 1.e-6
                d = tgrid[di] - 1.e-6
                fbar = Interp.interpND(data, [tgrid,tgrid], [t,d])
                @test abs(f(tgrid[ti],tgrid[di]) - fbar) < 9.e-6 # linear interpolation, so error is dδt+tδd+δt+δd
                @test f(tgrid[ti],tgrid[di]) > fbar
                @test f(tgrid[ti - 1],tgrid[di - 1]) < fbar
            end
        end

        tlist = rand(10) * β
        dlist = rand(10) * β
        # println(tlist)

        for (ti, t) in enumerate(tlist)
            for (di, d) in enumerate(tlist)
                fbar = Interp.interpND(data, [tgrid,tgrid], [t,d])
                @test abs(f(t,d) - fbar) < 9.e-6 # linear interpolation, so error is δK+δt
            end
        end
    end

    @testset "BaryCheb" begin
        β = π
        tgrid = SimpleGrid.BaryCheb{Float64}([0.0, β], 16)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t) = t
        data = zeros(tgrid.size)

        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end

        for ti = 1:tgrid.size - 1
            t = tgrid[ti] + 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) < fbar
            @test f(tgrid[ti + 1]) > fbar
        end
        for ti = 2:tgrid.size
            t = tgrid[ti] - 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) > fbar
            @test f(tgrid[ti - 1]) < fbar
        end

        t = tgrid[1] + eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        t = tgrid[tgrid.size] - eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        tlist = rand(10) * β
        # println(tlist)

        for (ti, t) in enumerate(tlist)
            fbar = Interp.interp1D(data, tgrid, t)
            # println("$k, $t, $fbar, ", f(k, t))
            @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
        end
    end

    @testset "DensedLog" begin
        β = 4
        tgrid = CompositeGrid.LogDensedGrid(:cheb, [0.0, β], [0.0, 0.5β, β], 4, 0.001, 4)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t) = t
        data = zeros(tgrid.size)

        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end

        for ti = 1:tgrid.size - 1
            t = tgrid[ti] + 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) < fbar
            @test f(tgrid[ti + 1]) > fbar
        end
        for ti = 2:tgrid.size
            t = tgrid[ti] - 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) > fbar
            @test f(tgrid[ti - 1]) < fbar
        end

        t = tgrid[1] + eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        t = tgrid[tgrid.size] - eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        tlist = rand(10) * β
        tlist = sort(tlist)
        println(tlist)
        ff = Interp.interp1DGrid(data, tgrid, tlist)
        ff_c = similar(ff)

        for (ti, t) in enumerate(tlist)
            fbar = Interp.interp1D(data, tgrid, t)
            ff_c[ti] = fbar
            # println("$k, $t, $fbar, ", f(k, t))
            @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test abs(f(t) - ff[ti]) < 3.e-6 # linear interpolation, so error is δK+δt
        end
        # println(tgrid.grid)
        # println("ff_c:",ff_c)
        # println("ff:",ff)
    end

    @testset "Integrate" begin
        β = 1.0
        tgrid = SimpleGrid.GaussLegendre{Float64}([0.0, β], 4)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t) = t
        data = zeros(tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end
        println(tgrid.grid)
        println(data)
        println(tgrid.weight)
        println(sum(data.*tgrid.weight))
        int_result = Interp.integrate1D(data, tgrid)
        @test abs(int_result - 0.5) < 3.e-6

        β = 1.0
        tgrid = SimpleGrid.BaryCheb{Float64}([0.0, β], 4)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        data = zeros(tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end
        println(tgrid.grid)
        println(data)
        println(tgrid.weight)
        println(sum(data.*tgrid.weight))
        int_result = Interp.integrate1D(data, tgrid)
        @test abs(int_result - 0.5) < 3.e-6
        int_result = Interp.integrate1D(data, tgrid, [0.3, 0.5])
        @test abs(int_result - 0.08) < 3.e-6

        β = 1.0
        tgrid = CompositeGrid.LogDensedGrid(:gauss, [0.0, β], [0.0, 0.5β, β], 2, 0.001, 3)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        data = zeros(2, tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[1,ti] = f(t)
            data[2,ti] = f(t)
        end
        println(tgrid.grid)
        println(data)
        int_result = Interp.integrate1D(data, tgrid; axis=2)
        @test abs(int_result[1] - 0.5) < 3.e-6
        @test abs(int_result[2] - 0.5) < 3.e-6

        β = 1.0
        tgrid = CompositeGrid.LogDensedGrid(:cheb, [0.0, β], [0.0, 0.5β, β], 8, 0.001, 4)
        # tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 11)
        # println(tgrid.grid)
        g(t) = t^2
        G(t) = t^3/3.0
        # g(t) = cos(t)
        # G(t) = sin(t)
        data = zeros(tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = g(t)
        end

        N=10
        testpoints = rand(N,2)*β
        for i in 1:N
            int_result = Interp.integrate1D(data, tgrid, testpoints[i,:])
            analytic = G(testpoints[i,2])-G(testpoints[i,1])
            # println(testpoints[i,:])
            # println(int_result, ",", analytic)
            @test abs(int_result - analytic) < 3.e-6
        end
    end

    @testset "Find Neighbor and Interp Sliced" begin
        β = 4
        tgrid = CompositeGrid.LogDensedGrid(:cheb, [0.0, β], [0.0, 0.5β, β], 4, 0.001, 4)
        # tugrid = Grid.Uniform{Float64,33}(0.0, β, (true, true))
        # kugrid = Grid.Uniform{Float64,33}(0.0, maxK, (true, true))
        f(t) = t
        data = zeros(tgrid.size)

        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = f(t)
        end

        for ti = 1:tgrid.size - 1
            t = tgrid[ti] + 1.e-6
            neighbor = Interp.findneighbor(Interp.LinearInterp(),tgrid, t)
            @test neighbor.index[1] == floor(tgrid, t)
            @test neighbor.index[2] == floor(tgrid, t) + 1

            neighbor = Interp.findneighbor(tgrid, t)
            data_slice = Interp.dataslice(data,neighbor.index)#data[neighbor.index[1]:neighbor.index[2]]
            fbar = Interp.interpsliced(neighbor, data_slice)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) < fbar
            @test f(tgrid[ti + 1]) > fbar
        end
        for ti = 2:tgrid.size
            t = tgrid[ti] - 1.e-6
            fbar = Interp.interp1D(data, tgrid, t)
            neighbor = Interp.findneighbor(Interp.LinearInterp(),tgrid, t)
            @test neighbor.index[1] == floor(tgrid, t)
            @test neighbor.index[2] == floor(tgrid, t) + 1

            neighbor = Interp.findneighbor(tgrid, t)
            data_slice = Interp.dataslice(data,neighbor.index)#data[neighbor.index[1]:neighbor.index[2]]
            fbar = Interp.interpsliced(neighbor, data_slice)
            @test abs(f(tgrid[ti]) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt
            @test f(tgrid[ti]) > fbar
            @test f(tgrid[ti - 1]) < fbar
        end

        t = tgrid[1] + eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

        t = tgrid[tgrid.size] - eps(Float64)*1e3
        fbar = Interp.interp1D(data, tgrid, t)
        @test abs(f(t) - fbar) < 3.e-6 # linear interpolation, so error is δK+δt

    end

    @testset "Differentiate" begin
        β = 1.0
        tgrid = CompositeGrid.LogDensedGrid(:uniform, [0.0, β], [0.0, 0.5β, β], 20, 0.0001, 20)
        # tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 11)
        # println(tgrid.grid)
        # g(t) = t^2
        # G(t) = t^3/3.0
        g(t) = cos(t)
        G(t) = sin(t)
        data = zeros(tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = G(t)
        end

        N=2
        testpoints = rand(N)*β
        for i in 1:N
            int_result = Interp.differentiate1D(data, tgrid, testpoints[i])
            analytic = g(testpoints[i])
            # println(testpoints[i,:])
            println(int_result, ",", analytic)
            # @test abs(int_result - analytic) < 3.e-6
        end

        β = 1.0
        tgrid = CompositeGrid.LogDensedGrid(:cheb, [0.0, β], [0.0, 0.5β, β], 4, 0.0001, 8)
        # tgrid = SimpleGrid.Uniform{Float64}([0.0, β], 11)
        # println(tgrid.grid)
        # g(t) = t^2
        # G(t) = t^3/3.0
        # g(t) = cos(t)
        # G(t) = sin(t)
        data = zeros(tgrid.size)
        for (ti, t) in enumerate(tgrid.grid)
            data[ti] = G(t)
        end

        N=100
        testpoints = rand(N)*β
        for i in 1:N
            int_result = Interp.differentiate1D(data, tgrid, testpoints[i])
            analytic = g(testpoints[i])
            # println(testpoints[i,:])
            # println(int_result, ",", analytic)
            @test isapprox(int_result, analytic, rtol = 1e-6, atol = 1e-6)
        end
    end

end

