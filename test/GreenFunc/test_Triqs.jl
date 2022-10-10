using PythonCall

@testset "Triqs mesh interface" begin
    gf = pyimport("triqs.gf")
    np = pyimport("numpy")

    mt = gf.MeshImTime(beta=1.0, S="Fermion", n_max=3)
    mjt = from_triqs(mt)
    for (i, x) in enumerate([p for p in mt.values()])
        @test mjt[i] ≈ pyconvert(Float64, x)
    end

    mw = gf.MeshImFreq(beta=1.0, S="Fermion", n_max=3)
    mjw = from_triqs(mw)
    for (i, x) in enumerate([p for p in mw.values()])
        @test mjw[i] ≈ imag(pyconvert(ComplexF64, x))
    end

    #add tests for MeshBrZone
    lat = pyimport("triqs.lattice")
    BL = lat.BravaisLattice(units=((2, 0, 0), (1, sqrt(3), 0))) # testing with a triangular lattice so that exchanged index makes a difference
    BZ = lat.BrillouinZone(BL)
    nk = 4
    mk = gf.MeshBrillouinZone(BZ, nk)
    mjk = from_triqs(mk)
    for p in mk
        ilin = pyconvert(Int, p.linear_index) + 1
        inds = pyconvert(Array, p.index)[1:2] .+ 1
        pval = pyconvert(Array, p.value)
        # note that while linear_index is kept, the index reversed
        @test pval[1:2] ≈ mjk[ilin]
        @test pval[1:2] ≈ mjk[reverse(inds)...]
    end

    # tests for MeshProduct
    mprod = gf.MeshProduct(mt, mw)
    mjprod = from_triqs(mprod)
    # println(mjprod)
    for (t, w) in mprod
        # println(t, w)
        ti, wi = pyconvert(Int, t.linear_index) + 1, pyconvert(Int, w.linear_index) + 1
        points = mjprod[wi, ti] # triqs mesh order is reversed
        @test points[1] ≈ imag(pyconvert(ComplexF64, w.value))
        @test points[2] ≈ pyconvert(Float64, t.value)
    end

    # test multilayer MeshProduct
    mprod2 = gf.MeshProduct(mk, mprod)
    mjprod2 = from_triqs(mprod2)
    for (k, tw) in mprod2
        # println(t, w)
        ki, ti, wi = pyconvert(Int, k.linear_index) + 1, pyconvert(Int, tw[0].linear_index) + 1, pyconvert(Int, tw[1].linear_index) + 1
        points = (mjprod2.meshes[1][wi, ti], mjprod2.meshes[2][ki])
        @test points[1][1] ≈ imag(pyconvert(ComplexF64, tw[1].value))
        @test points[1][2] ≈ pyconvert(Float64, tw[0].value)
        @test points[2] ≈ pyconvert(Array, k.value)[1:2]
    end

end

@testset "Triqs Gf interface" begin
    gf = pyimport("triqs.gf")
    np = pyimport("numpy")

    ############ test imaginary-time mesh #############
    mt = gf.MeshImTime(beta=1.0, S="Fermion", n_max=100)
    l = @py len(mt)
    lj = pyconvert(Int, l)

    ############ test MeshArray constructor #############
    G_t = gf.GfImTime(mesh=mt, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]
    gt = MeshArray(G_t)
    @test size(gt) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test gt[i1, i2, t] ≈ pyconvert(Float64, G_t.data[t-1, i2-1, i1-1])
    ######### test << ###############################
    G_t = gf.GfImTime(mesh=mt, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]
    gt << G_t
    @test size(gt) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test gt[i1, i2, t] ≈ pyconvert(Float64, G_t.data[t-1, i2-1, i1-1])

    ############ test Matsubara frequency mesh #############
    miw = gf.MeshImFreq(beta=1.0, S="Fermion", n_max=100)
    l = @py len(miw)
    lj = pyconvert(Int, l)

    ############ test MeshArray constructor #########
    G_w = gf.GfImFreq(mesh=miw, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]
    gw = MeshArray(G_w)
    @test size(gw) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test gw[i1, i2, t] ≈ pyconvert(Float64, G_w.data[t-1, i2-1, i1-1])
    ########### test << ###########################
    G_w = gf.GfImFreq(mesh=miw, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]
    gw << G_w
    @test size(gw) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test gw[i1, i2, t] ≈ pyconvert(Float64, G_w.data[t-1, i2-1, i1-1])

    ########## test MeshBrZone #######################
    lat = pyimport("triqs.lattice")
    BL = lat.BravaisLattice(units=((2, 0, 0), (1, sqrt(3), 0))) # testing with a triangular lattice so that exchanged index makes a difference
    BZ = lat.BrillouinZone(BL)
    nk = 8
    mk = gf.MeshBrillouinZone(BZ, nk)
    mprod = gf.MeshProduct(mk, miw)
    G_k_w = gf.GfImFreq(mesh=mprod, target_shape=[1, 1]) #G_k_w.data.shape will be [nk^2, lj, 1, 1]
    gkw = MeshArray(G_k_w)
    #gkw.mesh: [1:1, 1:1, miw, mk]
    @test size(gkw) == (1, 1, lj, nk^2)
    ik, iw = 12, 10
    @test gkw[1, 1, iw, ik] ≈ pyconvert(Float64, G_k_w.data[ik-1, iw-1, 0, 0])
    ########### test << ###########################
    G_k_w = gf.GfImFreq(mesh=mprod, data=np.random.rand(nk^2, lj, 1, 1)) #G_k_w.data.shape will be [nk^2, lj, 1, 1]
    gkw << G_k_w
    ik, iw = 12, 10
    @test gkw[1, 1, iw, ik] ≈ pyconvert(Float64, G_k_w.data[ik-1, iw-1, 0, 0])

end

@testset "Triqs BlockGf interface" begin
    gf = pyimport("triqs.gf")
    np = pyimport("numpy")

    mt = gf.MeshImTime(beta=1.0, S="Fermion", n_max=3)
    lj = pyconvert(Int, @py len(mt))
    G_t = gf.GfImTime(mesh=mt, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]
    G_w = gf.GfImTime(mesh=mt, data=np.random.rand(lj, 2, 3)) #target_shape = [2, 3] --> innerstate = [3, 2]

    blockG = gf.BlockGf(name_list=["1", "2"], block_list=[G_t, G_w], make_copies=false)

    jblockG = from_triqs(blockG)

    # gt = MeshArray(G_t)
    # @test size(gt) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test jblockG["1"][i1, i2, t] ≈ pyconvert(Float64, G_t.data[t-1, i2-1, i1-1])

    # ############ test Matsubara frequency mesh #############
    # gw = MeshArray(G_w)
    # @test size(gw) == (3, 2, lj)
    i1, i2, t = 1, 2, 3
    @test jblockG["2"][i1, i2, t] ≈ pyconvert(Float64, G_w.data[t-1, i2-1, i1-1])
end