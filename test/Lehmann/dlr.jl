using FastGaussQuadrature, Printf

rtol(x, y) = maximum(abs.(x - y)) / maximum(abs.(x))

# SemiCircle(dlr, grid, type) = Sample.SemiCircle(dlr.Euv, dlr.β, dlr.isFermi, grid, type, dlr.symmetry, rtol = dlr.rtol, degree = 24, regularized = true)
SemiCircle(dlr, grid, type) = Sample.SemiCircle(dlr, type, grid, degree = 24, regularized = true)

function MultiPole(dlr, grid, type)
    Euv = dlr.Euv
    poles = [-Euv, -0.2 * Euv, 0.0, 0.8 * Euv, Euv]
    # return Sample.MultiPole(dlr.β, dlr.isFermi, grid, type, poles, dlr.symmetry; regularized = true)
    return Sample.MultiPole(dlr, type, poles, grid; regularized = true)
end

function compare(case, a, b, eps, requiredratio, para = "")
    err = rtol(a, b)
    ratio = isfinite(err) ? Int(round(err / eps)) : 0
    if ratio > 50
        printstyled("$case, $para err: ", color = :white)
        printstyled("$(round(err, sigdigits=3)) = $ratio x rtol\n", color = :green)
    end
    @test rtol(a, b) .< requiredratio * eps # dlr should represent the Green's function up to accuracy of the order eps
end

function compare_atol(case, a, b, atol, para = "")
    err = rtol(a, b)
    err = isfinite(err) ? round(err, sigdigits = 3) : 0
    if err > 100 * atol
        printstyled("$case, $para err: ", color = :white)
        printstyled("$err = $(Int(round(err/atol))) x atol\n", color = :blue)
    end
    @test rtol(a, b) .< 5000 * atol # dlr should represent the Green's function up to accuracy of the order eps
end

@testset "Correlator Representation" begin

    function test(case, isFermi, symmetry, Euv, β, eps)
        # println("Test $case with isFermi=$isFermi, Symmetry = $symmetry, Euv=$Euv, β=$β, rtol=$eps")

        para = "fermi=$isFermi, sym=$symmetry, Euv=$Euv, β=$β, rtol=$eps"

        dlr = DLRGrid(Euv, β, eps, isFermi, symmetry) #construct dlr basis
        dlr10 = DLRGrid(10Euv, β, eps, isFermi, symmetry) #construct denser dlr basis for benchmark purpose

        #=========================================================================================#
        #                              Imaginary-time Test                                        #
        #=========================================================================================#
        # get imaginary-time Green's function 
        Gdlr = case(dlr, dlr.τ, :τ)
        # get imaginary-time Green's function for τ sample 
        τSample = dlr10.τ
        Gsample = case(dlr, τSample, :τ)

        ########################## imaginary-time to dlr #######################################
        coeff = tau2dlr(dlr, Gdlr)
        Gfitted = dlr2tau(dlr, coeff, τSample)
        compare("dlr τ → dlr → generic τ $case", Gsample, Gfitted, eps, 100, para)
        # for (ti, t) in enumerate(τSample)
        #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", t / β, Gsample[1, ti],  Gfitted[1, ti], Gsample[1, ti] - Gfitted[1, ti])
        # end

        compare("generic τ → dlr → τ $case", tau2tau(dlr, Gsample, dlr.τ, τSample), Gdlr, eps, 1000, para)
        #=========================================================================================#
        #                            Matsubara-frequency Test                                     #
        #=========================================================================================#
        # #get Matsubara-frequency Green's function
        Gndlr = case(dlr, dlr.n, :n)
        nSample = dlr10.n
        Gnsample = case(dlr, nSample, :n)

        # #Matsubara frequency to dlr
        coeffn = matfreq2dlr(dlr, Gndlr)
        Gnfitted = dlr2matfreq(dlr, coeffn, nSample)
        #     for (ni, n) in enumerate(nSample)
        #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", n, real(Gnsample[1, ni]),  real(Gnfitted[1, ni]), abs(Gnsample[1, ni] - Gnfitted[1, ni]))
        # end

        compare("dlr iω → dlr → generic iω $case ", Gnsample, Gnfitted, eps, 100, para)
        compare("generic iω → dlr → iω $case", matfreq2matfreq(dlr, Gnsample, dlr.n, nSample), Gndlr, eps, 1000, para)

        #=========================================================================================#
        #                            Fourier Transform Test                                     #
        #=========================================================================================#
        Gnfourier = tau2matfreq(dlr, Gdlr, nSample)
        compare("τ→dlr→iω $case", Gnsample, Gnfourier, eps, 1000, para)
        # for (ti, t) in enumerate(nSample)
        #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", t / β, imag(Gnsample[2, ti]), imag(Gnfourier[2, ti]), abs(Gnsample[2, ti] - Gnfourier[2, ti]))
        # end

        Gfourier = matfreq2tau(dlr, Gndlr, τSample)
        compare("iω→dlr→τ $case", Gsample, Gfourier, eps, 1000, para)
        # for (ti, t) in enumerate(τSample)
        #     @printf("%32.19g    %32.19g   %32.19g   %32.19g\n", t / β, Gsample[2, ti],  real(Gfourier[2, ti]), abs(Gsample[2, ti] - Gfourier[2, ti]))
        # end

        #=========================================================================================#
        #                            Noisey data Test                                             #
        #=========================================================================================#

        # err = 10 * eps
        atol = eps
        noise = atol * rand(eltype(Gsample), length(Gsample))
        GNoisy = Gsample .+ noise
        compare_atol("noisy generic τ → dlr → τ $case", tau2tau(dlr, GNoisy, dlr.τ, τSample; error = abs.(noise)), Gdlr, atol, para)

        noise = atol * rand(eltype(Gnsample), length(Gnsample))
        GnNoisy = Gnsample .+ noise
        compare_atol("noisy generic iω → dlr → iω $case", matfreq2matfreq(dlr, GnNoisy, dlr.n, nSample, error = abs.(noise)), Gndlr, atol, para)
    end

    # the accuracy greatly drops beyond Λ >= 1e8 and rtol<=1e-6
    cases = [SemiCircle, MultiPole]
    Λ = [1e3, 1e5, 1e7]
    rtol = [1e-8, 1e-10]
    for case in cases
        for l in Λ
            for r in rtol
                test(case, true, :none, 1.0, l, r)
                test(case, false, :none, 1.0, l, r)
                test(case, false, :ph, 1.0, l, r)
                test(case, true, :ph, 1.0, l, r)
                test(case, false, :pha, 1.0, l, r)
                test(case, true, :pha, 1.0, l, r)
            end
        end
    end

end

@testset "Tensor ↔ Matrix Mapping" begin
    a = rand(3)
    acopy = deepcopy(a)
    b, psize = Lehmann._tensor2matrix(a, 1)
    anew = Lehmann._matrix2tensor(b, psize, 1)
    @test acopy ≈ anew

    a = rand(3, 4)
    acopy = deepcopy(a)
    for axis = 1:2
        b, psize = Lehmann._tensor2matrix(a, axis)
        anew = Lehmann._matrix2tensor(b, psize, axis)
        @test acopy ≈ anew
    end

    a = rand(3, 4, 5)
    acopy = deepcopy(a)
    for axis = 1:3
        b, psize = Lehmann._tensor2matrix(a, axis)
        anew = Lehmann._matrix2tensor(b, psize, axis)
        @test acopy ≈ anew
    end
end

@testset "Tensor DLR" begin
    Euv, β = 1.0, 1000.0
    eps = 1e-10
    isFermi = true
    symmetry = :none
    # symmetry = :ph
    weight = π / 2 * Euv
    para = "fermi=$isFermi, sym=$symmetry, Euv=$Euv, β=$β, rtol=$eps"
    dlr = DLRGrid(Euv, β, eps, isFermi, symmetry) #construct dlr basis

    Gτ = SemiCircle(dlr, dlr.τ, :τ)
    Gn = SemiCircle(dlr, dlr.n, :n)
    Gτ_copy = deepcopy(Gτ)
    Gn_copy = deepcopy(Gn)

    n1, n2 = 1, 1
    a = rand(n1, n2)
    # a = ones(n1, n2)
    sumrule_τ = zeros(n1, n2)
    sumrule_n = zeros(n1, n2)
    tensorGτ = zeros(eltype(Gτ), (n1, n2, length(Gτ)))
    for (gi, g) in enumerate(Gτ)
        tensorGτ[:, :, gi] = a .* g
        sumrule_τ = a * weight
    end
    tensorGn = zeros(eltype(Gn), (n1, n2, length(Gn)))
    for (gi, g) in enumerate(Gn)
        tensorGn[:, :, gi] = a .* g
        sumrule_n = a * weight
    end
    tensorGτ_copy = tensorGτ
    tensorGn_copy = tensorGn

    compare("τ ↔ iω tensor", tau2matfreq(dlr, Gτ_copy), Gn, eps, 1000.0, para)
    @test Gτ ≈ Gτ_copy #make sure there is no side effect on G
    compare("iω ↔ τ tensor", matfreq2tau(dlr, Gn_copy), Gτ, eps, 1000.0, para)
    @test Gn ≈ Gn_copy #make sure there is no side effect on G

    compare("τ ↔ iω tensor", tau2matfreq(dlr, Gτ_copy; sumrule = weight), Gn, eps, 1000.0, para)
    @test Gτ ≈ Gτ_copy #make sure there is no side effect on G
    compare("iω ↔ τ tensor", matfreq2tau(dlr, Gn_copy; sumrule = weight), Gτ, eps, 1000.0, para)
    @test Gn ≈ Gn_copy #make sure there is no side effect on G

    compare("τ ↔ iω tensor", tau2matfreq(dlr, tensorGτ_copy; axis = 3), tensorGn, eps, 1000.0, para)
    @test tensorGτ ≈ tensorGτ_copy #make sure there is no side effect on G
    compare("iω ↔ τ tensor", matfreq2tau(dlr, tensorGn_copy; axis = 3), tensorGτ, eps, 1000.0, para)
    @test tensorGn ≈ tensorGn_copy #make sure there is no side effect on G

    compare("τ ↔ iω tensor", tau2matfreq(dlr, tensorGτ_copy; axis = 3, sumrule = sumrule_τ), tensorGn, eps, 1000.0, para)
    @test tensorGτ ≈ tensorGτ_copy #make sure there is no side effect on G
    compare("iω ↔ τ tensor", matfreq2tau(dlr, tensorGn_copy; axis = 3, sumrule = sumrule_n), tensorGτ, eps, 1000.0, para)
    @test tensorGn ≈ tensorGn_copy #make sure there is no side effect on G

end

@testset "Least square fitting" begin
    Gτ = [6.0, 5.0, 7.0, 10.0]
    kernel = zeros(4, 2)
    kernel[:, 1] = [1.0, 1.0, 1.0, 1.0]
    kernel[:, 2] = [1.0, 2.0, 3.0, 4.0]
    dlrGrid = DLRGrid(β = 10.0, isFermi = true)
    coeff = Lehmann._weightedLeastSqureFit(dlrGrid, Gτ, nothing, kernel, nothing)
    @test coeff ≈ [3.5, 1.4]
end

@testset "DLR io" begin
    function finddlr(folder, filename)
        searchdir(path, key) = filter(x -> occursin(key, x), readdir(path))
        file = searchdir(folder, filename)
        if length(file) == 1
            #dlr file found
            println("find dlr file: ", file[1])
            return joinpath(folder, file[1])
        end
        return nothing
    end

    folder = "./"
    Euv, β, rtol = 1.5, 110.0, 4.3e-8

    #save dlr to a local file
    dlr = Lehmann.DLRGrid(Euv, β, rtol, true; rebuild = true, folder = folder, verbose = false)
    file = finddlr(folder, ".dlr")
    @test isnothing(file) == false
    #load dlr from the local file
    dlr_load = Lehmann.DLRGrid(Euv, β, rtol, true; rebuild = false, folder = folder, verbose = false)
    @test dlr_load.τ ≈ dlr.τ
    @test dlr_load.n ≈ dlr.n
    @test dlr_load.ω ≈ dlr.ω
    @test dlr_load.ωn ≈ dlr.ωn
    @test dlr_load.Euv ≈ dlr.Euv
    @test dlr_load.β ≈ dlr.β
    @test dlr_load.rtol ≈ dlr.rtol
    @test dlr_load.Λ ≈ dlr.Λ
    rm(file)
end

@testset "JLD2 IO" begin
    dlr = DLRGrid(isFermi = true, beta = 10.0)
    save("test.jld2", Dict("dlr" => dlr), compress = true)
    dlr_new = load("test.jld2")["dlr"]
    println(dlr_new)
    @test dlr.isFermi == dlr_new.isFermi
    @test dlr.Euv ≈ dlr_new.Euv
    @test dlr.β ≈ dlr_new.β
    @test dlr.τ ≈ dlr_new.τ
    @test dlr.n ≈ dlr_new.n
    @test dlr.ωn ≈ dlr_new.ωn
    @test dlr.symmetry == dlr_new.symmetry
    rm("test.jld2")
end
