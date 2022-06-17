"""
discrete Lehmann representation for imaginary-time/Matsubara-freqeuncy correlator
"""
# include("spectral.jl")
# using ..Spectral
# include("./functional/builder.jl")
# include("./discrete/builder.jl")
# include("operation.jl")


"""
struct DLRGrid

    DLR grids for imaginary-time/Matsubara frequency correlators

#Members:
- `isFermi`: bool is fermionic or bosonic
- `symmetry`: particle-hole symmetric :ph, or particle-hole asymmetric :pha, or :none
- `Euv` : the UV energy scale of the spectral density 
- `β` or `beta` : inverse temeprature
- `Λ` or `lambda`: cutoff = UV Energy scale of the spectral density * inverse temperature
- `rtol`: tolerance absolute error
- `size` : number of DLR basis
- `ω` or `omega` : selected representative real-frequency grid
- `n` : selected representative Matsubara-frequency grid (integer)
- `ωn` or `omegaN` : (2n+1)π/β
- `τ` or `tau` : selected representative imaginary-time grid
"""
mutable struct DLRGrid
    isFermi::Bool
    symmetry::Symbol
    Euv::Float64
    β::Float64
    Λ::Float64
    rtol::Float64

    # dlr grids
    # size::Int # rank of the dlr representation
    ω::Vector{Float64}
    n::Vector{Int} # integers, (2n+1)π/β gives the Matsubara frequency
    ωn::Vector{Float64} # (2n+1)π/β
    τ::Vector{Float64}

    kernel_τ::Any
    kernel_n::Any

    """
    function DLRGrid(Euv, β, rtol, isFermi::Bool; symmetry::Symbol = :none, rebuild = false, folder = nothing, algorithm = :functional, verbose = false)
    function DLRGrid(; isFermi::Bool, β = -1.0, beta = -1.0, Euv = 1.0, symmetry::Symbol = :none, rtol = 1e-14, rebuild = false, folder = nothing, algorithm = :functional, verbose = false)

    Create DLR grids

    #Arguments:
    - `Euv`         : the UV energy scale of the spectral density 
    - `β` or `beta` : inverse temeprature
    - `isFermi`     : bool is fermionic or bosonic
    - `symmetry`    : particle-hole symmetric :ph, or particle-hole asymmetric :pha, or :none
    - `rtol`        : tolerance absolute error
    - `rebuild`     : set false to load DLR basis from the file, set true to recalculate the DLR basis on the fly
    - `folder`      : if rebuild is true and folder is set, then dlrGrid will be rebuilt and saved to the specified folder
                      if rebuild is false and folder is set, then dlrGrid will be loaded from the specified folder
    - `algorithm`   : if rebuild = true, then set :functional to use the functional algorithm to generate the DLR basis, or set :discrete to use the matrix algorithm.
    - `verbose`     : false not to print DLRGrid to terminal, or true to print
    """
    function DLRGrid(Euv, β, rtol, isFermi::Bool, symmetry::Symbol = :none; rebuild = false, folder = nothing, algorithm = :functional, verbose = false)
        Λ = Euv * β # dlr only depends on this dimensionless scale
        # println("Get $Λ")
        @assert rtol > 0.0 "rtol=$rtol is not positive and nonzero!"
        @assert Λ > 0 "Energy scale $Λ must be positive!"
        @assert symmetry == :ph || symmetry == :pha || symmetry == :none "symmetry must be :ph, :pha or nothing"
        @assert algorithm == :functional || algorithm == :discrete "Algorithm is either :functional or :discrete"
        @assert β > 0.0 "Inverse temperature must be temperature."
        @assert Euv > 0.0 "Energy cutoff must be positive."

        if Λ > 1e8 && symmetry == :none
            @warn("Current DLR without symmetry may cause ~ 3-4 digits loss for Λ ≥ 1e8!")
        end

        if rtol > 1e-6
            @warn("Current implementation may cause ~ 3-4 digits loss for rtol > 1e-6!")
        end

        rtolpower = Int(floor(log10(rtol))) # get the biggest n so that rtol>1e-n
        if abs(rtolpower) < 4
            rtolpower = -4
        end
        rtol = 10.0^float(rtolpower)

        function finddlr(folder, filename)
            searchdir(path, key) = filter(x -> occursin(key, x), readdir(path))
            for dir in folder
                if length(searchdir(dir, filename)) > 0
                    #dlr file found
                    return joinpath(dir, filename)
                end
            end
            @warn("Cann't find the DLR file $filename in the folders $folder. Regenerating DLR...")
            return nothing
        end

        function filename(lambda, errpower)
            lambda = Int128(floor(lambda))
            errstr = "1e$errpower"

            if symmetry == :none
                return "universal_$(lambda)_$(errstr).dlr"
            elseif symmetry == :ph
                return "ph_$(lambda)_$(errstr).dlr"
            elseif symmetry == :pha
                return "pha_$(lambda)_$(errstr).dlr"
            else
                error("$symmetry is not implemented!")
            end
        end


        if rebuild == false
            if isnothing(folder)
                Λ = Λ < 100 ? Int(100) : 10^(Int(ceil(log10(Λ)))) # get smallest n so that Λ<10^n
                folderList = [string(@__DIR__, "/../basis/"),]
            else
                folderList = [folder,]
            end

            file = filename(Λ, rtolpower)
            dlrfile = finddlr(folderList, file)

            if isnothing(dlrfile) == false
                dlr = new(isFermi, symmetry, Euv, β, Λ, rtol, [], [], [], [], nothing, nothing)
                _load!(dlr, dlrfile, verbose)
                dlr.kernel_τ = Spectral.kernelT(Val(dlr.isFermi), Val(dlr.symmetry), dlr.τ, dlr.ω, dlr.β, true)
                dlr.kernel_n = Spectral.kernelΩ(Val(dlr.isFermi), Val(dlr.symmetry), dlr.n, dlr.ω, dlr.β, true)
                return dlr
            else
                @warn("No DLR is found in the folder $folder, try to rebuild instead.")
            end

        end

        # try to rebuild the dlrGrid
        dlr = new(isFermi, symmetry, Euv, β, Euv * β, rtol, [], [], [], [], nothing, nothing)
        file2save = filename(Euv * β, rtolpower)
        _build!(dlr, folder, file2save, algorithm, verbose)

        dlr.kernel_τ = Spectral.kernelT(Val(dlr.isFermi), Val(dlr.symmetry), dlr.τ, dlr.ω, dlr.β, true)
        dlr.kernel_n = Spectral.kernelΩ(Val(dlr.isFermi), Val(dlr.symmetry), dlr.n, dlr.ω, dlr.β, true)
        return dlr
    end

    function DLRGrid(; isFermi::Bool, β = -1.0, beta = -1.0, Euv = 1.0, symmetry::Symbol = :none, rtol = 1e-14, rebuild = false, folder = nothing, algorithm = :functional, verbose = false)
        if β <= 0.0 && beta > 0.0
            β = beta
        elseif β > 0.0 && beta <= 0.0
            beta = β
        elseif β < 0.0 && beta < 0.0
            error("Either β or beta needs to be initialized with a positive value!")
        end
        @assert β ≈ beta
        return DLRGrid(Euv, β, rtol, isFermi, symmetry; rebuild = rebuild, folder = folder, algorithm = algorithm, verbose = verbose)
    end
end

function Base.getproperty(obj::DLRGrid, sym::Symbol)
    # if sym === :hasTau
    #     return obj.totalTauNum > 0
    if sym == :size
        return size(obj)
    elseif sym == :tau
        return obj.τ
    elseif sym == :beta
        return obj.β
    elseif sym == :omegaN
        return obj.ωn
    elseif sym == :omega
        return obj.ω
    elseif sym == :lambda
        return obj.Λ
    else # fallback to getfield
        return getfield(obj, sym)
    end
end


"""
Base.size(dlrGrid::DLRGrid) = length(dlrGrid.ω)
Base.length(dlrGrid::DLRGrid) = length(dlrGrid.ω)
rank(dlrGrid::DLRGrid) = length(dlrGrid.ω)

get the rank of the DLR grid, namely the number of the DLR coefficients.
"""
Base.size(dlrGrid::DLRGrid) = length(dlrGrid.ω)
Base.length(dlrGrid::DLRGrid) = length(dlrGrid.ω)
rank(dlrGrid::DLRGrid) = length(dlrGrid.ω)

function _load!(dlrGrid::DLRGrid, dlrfile, verbose = false)

    grid = readdlm(dlrfile, comments = true, comment_char = '#')
    # println("reading $filename")

    β = dlrGrid.β
    ω, τ = grid[:, 2], grid[:, 3]

    if dlrGrid.isFermi
        n = Int.(grid[:, 4])
        ωn = @. (2n + 1.0) * π / β
    else
        n = Int.(grid[:, 5])
        ωn = @. 2n * π / β
    end
    for r = 1:length(ω)
        push!(dlrGrid.ω, ω[r] / β)
        push!(dlrGrid.τ, τ[r] * β)
        push!(dlrGrid.n, n[r])
        push!(dlrGrid.ωn, ωn[r])
    end
    verbose && println(dlrGrid)
end

function _build!(dlrGrid::DLRGrid, folder, filename, algorithm, verbose = false)
    isFermi = dlrGrid.isFermi
    β = dlrGrid.β
    if algorithm == :discrete || dlrGrid.symmetry == :none
        ω, τ, nF, nB = Discrete.build(dlrGrid, verbose)
    elseif algorithm == :functional && (dlrGrid.symmetry == :ph || dlrGrid.symmetry == :pha)
        ω, τ, nF, nB = Functional.build(dlrGrid, verbose)
    else
        error("$algorithm has not yet been implemented!")
    end
    rank = length(ω)
    if isnothing(folder) == false
        open(joinpath(folder, filename), "w") do io
            @printf(io, "# %5s  %25s  %25s  %25s  %20s\n", "index", "freq", "tau", "fermi n", "bose n")
            for r = 1:rank
                @printf(io, "%5i  %32.17g  %32.17g  %16i  %16i\n", r, ω[r], τ[r], nF[r], nB[r])
            end
        end
    end
    for r = 1:rank
        push!(dlrGrid.ω, ω[r] / β)
        push!(dlrGrid.τ, τ[r] * β)
        n = isFermi ? nF[r] : nB[r]
        push!(dlrGrid.n, n)
        push!(dlrGrid.ωn, isFermi ? (2n + 1.0) * π / β : 2n * π / β)
    end
    # println(rank)
end


function Base.show(io::IO, dlr::DLRGrid)
    title = dlr.isFermi ? "ferminoic" : "bosonic"
    println(io, "rank = $(dlr.size) $title DLR with $(dlr.symmetry) symmetry: Euv = $(dlr.Euv), β = $(dlr.β), rtol = $(dlr.rtol)")
    @printf(io, "# %5s  %28s  %28s  %28s      %20s\n", "index", "freq", "tau", "ωn", "n")
    for r = 1:dlr.size
        @printf(io, "%5i  %32.17g  %32.17g  %32.17g  %16i\n", r, dlr.ω[r], dlr.τ[r], dlr.ωn[r], dlr.n[r])
    end

end