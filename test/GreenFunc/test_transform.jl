SemiCircle(dlr, grid, type) = Sample.SemiCircle(dlr.Euv, dlr.β, dlr.isFermi, grid, type, dlr.symmetry; rtol=dlr.rtol, degree=24, regularized=false)

@testset "Transform" begin
    function test_fourier(N1, beta, statistics)
        mesh1 = SimpleGrid.Uniform{Float64}([0.0, 1.0], N1)
        mesh2 = MeshGrids.DLRFreq(beta, statistics)
        g = MeshArray(mesh1, mesh2; data=zeros(N1, length(mesh2)))

        @test GreenFunc._find_mesh(typeof(g.mesh), DLRFreq) == 2
        @test GreenFunc._find_mesh(typeof(g.mesh), Int) == 0 # not found

        moremeshes = (mesh1, mesh2, mesh1)
        replacedmeshes = GreenFunc._replace_mesh(moremeshes, mesh2, mesh1)
        # println(typeof(replacedmeshes))
        @test replacedmeshes == (mesh1, mesh1, mesh1)

        g_freq = dlr_to_imfreq(g)
        Gτ = SemiCircle(mesh2.dlr, mesh2.dlr.τ, :τ)
        Gn = SemiCircle(mesh2.dlr, mesh2.dlr.n, :n)

        # iterate over the last dimension of g_freq.data
        # for (ni, n) in enumerate(mesh2.dlr.n)
        #     g_freq.data[:, ni] .= Gn[ni]
        # end

        GreenFunc.multipole!(g_freq, [1.0, 2.0]) #check if it runs or not

        GreenFunc.semicircle!(g_freq)

        @test g_freq[1, :] ≈ Gn

        g_dlr = imfreq_to_dlr(g_freq)
        @time g_dlr = imfreq_to_dlr(g_freq)
        rtol = mesh2.dlr.rtol

        g_time = dlr_to_imtime(g_dlr)
        @time g_time = dlr_to_imtime(g_dlr)
        err = maximum(abs.(g_time.data[1, :] .- Gτ))
        printstyled("test dlr_to_imtime dlr->τ $err\n", color=:white)

        @test err < 50 * rtol
        g_freq1 = dlr_to_imfreq(g_dlr)
        @time g_freq1 = dlr_to_imfreq(g_dlr)
        err = maximum(abs.(g_freq1.data[1, :] .- Gn))
        printstyled("test dlr_to_imfreq $err\n", color=:white)
        @test err < 50 * rtol

        ########### test pipe operation #############
        g_freq2 = g_freq |> to_dlr |> to_imfreq
        @time g_freq2 = g_freq |> to_dlr |> to_imfreq
        err = maximum(abs.(g_freq2.data[1, :] .- Gn))
        printstyled("test dlr_to_imfreq with pipe $err\n", color=:white)
        @test err < 50 * rtol

        g_time2 = g_freq |> to_dlr |> to_imtime
        @time g_time2 = g_freq |> to_dlr |> to_imtime
        err = maximum(abs.(g_time2.data[1, :] .- Gτ))
        printstyled("test dlr_to_imtime with pipe $err\n", color=:white)
        @test err < 50 * rtol


        g_freq1 << g_dlr
        @time g_freq1 << g_dlr
        err = maximum(abs.(g_freq1.data[1, :] .- Gn))
        printstyled("test  imfreq<<dlr $err\n", color=:white)
        @test err < 50 * rtol
        g_time << g_dlr
        @time g_time << g_dlr
        err = maximum(abs.(g_time.data[1, :] .- Gτ))
        printstyled("test  imtime<<dlr $err\n", color=:white)

        @test err < 50 * rtol


    end
    test_fourier(5, 100.0, FERMION)
    test_fourier(5, 100.0, BOSON)
end