"""
    struct DLRFreq{T<:Real} <: TemporalGrid{Int}

Discrete-Lehmann-representation grid for Green's functions. 

# Parameters
- `T`: type of the `grid` point, `β` and `Euv`.

# Members
- `dlr`: built-in DLR grid.
- `grid`: 1D grid of time axis, with locate, volume, and AbstractArray interface implemented.
  It should be grid of Int for ImFreq, and DLRGrid for DLRFreq.
- `β`: inverse temperature.
- `Euv`:  the UV energy scale of the spectral density.
- `rtol`: tolerance absolute error.
- `symmetry`: `:ph` for particle-hole symmetric, `:pha` for particle-hole symmetry, and `:none` for no symmetry. By default, `sym = :none`.
- `isFermi`: the statistics for particles. 
"""
struct DLRFreq{T<:Real} <: TemporalGrid{T}
    dlr::DLRGrid
    grid::SimpleG.Arbitrary{T}
    β::T
    Euv::T
    rtol::Float64
    symmetry::Symbol
    isFermi::Bool
end

"""
    function DLRFreq(β, isFermi::Bool=false;
        dtype=Float64,
        rtol=1e-12,
        Euv=1000 / β,
        sym=:none,
        rebuild=false,
        dlr::Union{DLRGrid,Nothing}=nothing
    )

Create a `DLRFreq` struct from parameters.

# Arguments
- `β`: inverse temperature.
- `isFermi`: the statistics for particles is fermionic or not. False by default.
- `dtype`: type of `β` and `Euv`.
- `rtol`: tolerance absolute error. By default, `rtol` = 1e-12.
- `Euv`: the UV energy scale of the spectral density. By default, `Euv = 1000 / β`.
- `symmetry`: `:ph` for particle-hole symmetric, `:pha` for particle-hole symmetry, and `:none` for no symmetry. By default, `sym = :none`.
- `rebuild`: if no dlr is input, set false to load DLRGrid from the file; set true to recalculate the DLRGrid on the fly. By default, `rebuild = false`.
"""
function DLRFreq(β::Real, isFermi::Bool=false;
    dtype=Float64,
    rtol=1e-12,
    Euv=1000 / β,
    symmetry=:none,
    rebuild=false
)
    dlr = DLRGrid(Euv, β, rtol, isFermi, symmetry; rebuild=rebuild)
    grid = SimpleG.Arbitrary{dtype}(dlr.ω)
    return DLRFreq{dtype}(dlr, grid, β, Euv, rtol, symmetry, isFermi)
end

"""
    function DLRFreq(dlr::DLRGrid)

Create a `DLRFreq` struct from `DLRGrid`.

# Arguments
- `dlr`: 1D DLR grid.
"""
function DLRFreq(dlr::DLRGrid)
    dtype = Float64 #TODO: replace it with dlr type
    grid = SimpleG.Arbitrary{dtype}(dlr.ω)
    return DLRFreq{dtype}(dlr, grid, dlr.β, dlr.Euv, dlr.rtol, dlr.symmetry, dlr.isFermi)
end

"""
    show(io::IO, tg::DLRFreq)

Write a text representation of the DLR grid `tg` to the output stream `io`.
"""
Base.show(io::IO, tg::DLRFreq) = print(io, "DLR frequency grid with $(length(tg)) points, inverse temperature = $(tg.β), UV Energy scale = $(tg.Euv), rtol = $(tg.rtol), sym = $(tg.symmetry), fermionic = $(tg.isFermi): $(_grid(tg.grid))")
