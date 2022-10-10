# basic AbstractArray implement
Base.length(tg::TemporalGrid) = length(tg.grid)
Base.size(tg::TemporalGrid) = size(tg.grid)
Base.size(tg::TemporalGrid, I::Int) = size(tg.grid, I)
Base.getindex(tg::TemporalGrid, I::Int) = tg.grid[I]
Base.firstindex(tg::TemporalGrid) = 1
Base.lastindex(tg::TemporalGrid) = length(tg)

# iterator
Base.iterate(tg::TemporalGrid) = (tg[1], 1)
Base.iterate(tg::TemporalGrid, state) = (state >= length(tg)) ? nothing : (tg[state+1], state + 1)
# Base.IteratorSize(tg)
Base.IteratorSize(::Type{TemporalGrid{GT}}) where {GT} = Base.HasLength()
Base.IteratorEltype(::Type{TemporalGrid{GT}}) where {GT} = Base.HasEltype()
Base.eltype(::Type{TemporalGrid{GT}}) where {GT} = eltype(GT)

# locate and volume could fail if tg.grid has no implementation
volume(tg::TemporalGrid, I::Int) = volume(tg.grid, I)
volume(tg::TemporalGrid) = volume(tg.grid)
locate(tg::TemporalGrid, pos) = locate(tg.grid, pos)

Base.floor(tg::TemporalGrid, pos) = floor(tg.grid, pos)

function _round(grid, sigdigits)
    if sigdigits <= 0
        return grid
    else
        return [round(x, sigdigits=sigdigits) for x in grid]
    end
end

# return a pretty print for grid. 
# If grid is very long, return [grid[1], grid[2], grid[3], ..., grid[end-2], grid[end-1], grid[end]]
# If grid is short, return the entire grid
function _grid(grid, n=3)
    if eltype(grid) <: Int
        # return join(io, ["[", join([grid[1:n]..., "...", grid[end-n:end]...], ", "), "]"])
        digits = 0
    else
        resolution = grid[2] - grid[1]
        digits = Int(round(log(grid[end] / resolution) / log(10))) + 3
        digits = digits < 5 ? 5 : digits
    end
    if length(grid) <= 2n + 3
        return join(["[", join(_round(grid, digits), ", "), "]"])
    else
        return join(["[", join([_round(grid[1:n], digits)..., "...", _round(grid[end-(n-1):end], digits)...], ", "), "]"])
    end
end

Base.show(io::IO, ::MIME"text/plain", tg::TemporalGrid) = Base.show(io, tg)
Base.show(io::IO, ::MIME"text/html", tg::TemporalGrid) = Base.show(io, tg)