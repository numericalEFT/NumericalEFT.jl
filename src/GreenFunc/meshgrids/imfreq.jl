"""
    struct ImFreq{T, G, R} <: TemporalGrid{Int}

Imaginary-frequency grid for Green's functions. 

# Parameters
- `T<:Real`: type of the `grid` point, `β` and `Euv`.
- `G<:AbstractGrid{T}`: type of 1D grid with `T` as the grid point type.
- `R`: type of the representation.

# Members
- `grid`: 1D grid of time axis, with locate, volume, and AbstractArray interface implemented.
  It should be grid of Int for ImFreq, and DLRGrid for DLRFreq.
- `β`: inverse temperature.
- `Euv`:  the UV energy scale of the spectral density.
- `isFermi`: the statistics for particles is fermionic or not.
- `symmetry`: `:ph` for particle-hole symmetric, `:pha` for particle-hole symmetry, and `:none` for no symmetry. By default, `sym = :none`.
- `rtol`: relative tolerance
- `representation`: the representation of the Green's function.
"""
struct ImFreq{T<:Real,G<:AbstractGrid{Int},R} <: TemporalGrid{Int}
    grid::G
    β::T
    Euv::T
    isFermi::Bool
    symmetry::Symbol
    rtol::T
    representation::R # representation of the imaginary-time axis
end

"""
    function ImFreq(β, isFermi::Bool=false;
        dtype=Float64,
        Euv=1000 / β,
        rtol=1e-12,
        symmetry=:none,
        grid::Union{AbstractGrid,AbstractVector,Nothing}=nothing
    )

Create a `ImFreq` struct from parameters.

# Arguments
- `β`: inverse temperature.
- `isFermi`: the statistics for particles is fermionic or not. False by default.
- `dtype`: type of `β` and `Euv`. By default, `dtype = Float64`.
- `Euv`: the UV energy scale of the spectral density. By default, `Euv = 1000 / β`.
- `symmetry`: `:ph` for particle-hole symmetric, `:pha` for particle-hole symmetry, and `:none` for no symmetry. By default, `sym = :none`.
- `grid`: 1D Matsubara-frequency integer-valued grid as a AbstractVector or CompositeGrids.AbstractGrid. By default, a optimized grid built in DLR is used.
"""
function ImFreq(β, isFermi::Bool=false;
    dtype=Float64,
    Euv=1000 / β,
    rtol=1e-12,
    symmetry=:none,
    grid::Union{AbstractGrid,AbstractVector,Nothing}=nothing
)
    dlr = DLRGrid(Euv, β, rtol, isFermi, :none)
    if isnothing(grid)
        # TODO: replace the dlr.n with a non-dlr grid. User don't want dlr if it is not initialized with a dlr
        grid = SimpleG.Arbitrary{Int}(dlr.n)
    elseif (grid isa AbstractVector)
        grid = SimpleG.Arbitrary{Int}(Int.(grid))
    else
        error("Proper grid or basis are required.")
    end

    @assert issorted(grid) "The grid should be sorted."
    @assert eltype(grid) <: Int "Matsubara-frequency grid should be Int."
    return ImFreq{dtype,typeof(grid),typeof(dlr)}(grid, β, Euv, isFermi, symmetry, rtol, dlr)
end

"""
    function ImFreq(dlr::DLRGrid; dtype=Float64, grid::Union{AbstractGrid,AbstractVector}=SimpleG.Arbitrary{Int}(dlr.n))

Construct `ImFreq` from a `DLRGrid`, with a given `grid`. By default, `grid` is the Matsubara-frequency points from `DLRGrid`.
"""
function ImFreq(dlr::DLRGrid; dtype=Float64, grid::Union{AbstractGrid,AbstractVector}=SimpleG.Arbitrary{Int}(dlr.n))
    if (grid isa AbstractGrid) == false
        grid = SimpleG.Arbitrary{Int}(grid)
    end
    @assert issorted(grid) "The grid should be sorted."
    @assert eltype(grid) <: Int "Matsubara-frequency grid should be Int."
    return ImFreq{dtype,typeof(grid),typeof(dlr)}(grid, dlr.β, dlr.Euv, dlr.isFermi, dlr.symmetry, dlr.rtol, dlr)
end
ImFreq(dlrfreq::DLRFreq; kwargs...) = ImFreq(dlrfreq.dlr; kwargs...)

matfreq_to_int(tg::ImFreq, ωn) = tg.isFermi ? Int(round((ωn * tg.β / π - 1) / 2)) : Int(round((ωn * tg.β / π) / 2))
int_to_matfreq(tg::ImFreq, n::Int) = tg.isFermi ? (2n + 1) * π / tg.β : 2n * π / tg.β

matfreq(tg::ImFreq) = [int_to_matfreq(tg, n) for n in tg.grid]

"""
    getindex(g::ImFreq, I::Int)

Equivalent to `g[I]`, get the __real-valued__ Matsubara frequency of the Ith point in the grid. 
For fermion, return (2g[I]+1)π/β, for boson, return 2g[I]*π/β.

If you need the __integer-valued__ frequency, use `g.grid[I]` instead.
"""
Base.getindex(tg::ImFreq, I::Int) = int_to_matfreq(tg, tg.grid[I])

"""
    show(io::IO, tg::ImFreq)

Write a text representation of the Imaginary-frequency grid `tg` to the output stream `io`.
"""
Base.show(io::IO, tg::ImFreq) = print(io, "Matsubara frequency grid with $(length(tg)) points, inverse temperature = $(tg.β), UV Energy scale = $(tg.Euv), fermionic = $(tg.isFermi): $(_grid(tg.grid))")


"""
    locate(tg::ImFreq, n::Int)
    locate(tg::ImFreq, ωn)

Find the location in `tg.grid` for the Matsubara frequency `ωn` or the integer `n`.
"""
locate(tg::ImFreq, n::Int) = locate(tg.grid, n)
function locate(tg::ImFreq, ωn)
    n = matfreq_to_int(tg, ωn)
    return locate(tg.grid, n)
end
