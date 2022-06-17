"""
Composite grid that has tree structure. The whole interval is first divided by a panel grid,
then each interval of a panel grid is divided by a smaller grid in subgrids. Subgrid could also be
composite grid.
"""
module CompositeG

export LogDensedGrid, Composite, denseindex

using StaticArrays, FastGaussQuadrature
using ..SimpleG

"""
    struct Composite{T<:AbstractFloat,PG,SG} <: SimpleG.ClosedGrid

Composite grid generated with panel grid of type PG and subgrids of type SG.
PG should always be ClosedGrid, while SG could be any grid.

#Members:
- `bound` : boundary of the grid
- `size` : number of grid points
- `grid` : grid points
- `panel` : panel grid
- `subgrids` : a vector of subgrids
- `inits` : index of the first grid point of a subgrid on the whole grid

#Constructor:
-    function Composite{T,PG,SG}(panel, subgrids) where {T<:AbstractFloat,PG,SG}
create Composite grid from panel and subgrids.
if the boundary grid point of two neighbor subgrids are too close, they will be combined
in the whole grid.
"""
struct Composite{T<:AbstractFloat,PG,SG} <: SimpleG.ClosedGrid
    bound::SVector{2,T}
    size::Int
    grid::Vector{T}


    panel::PG
    subgrids::Vector{SG}
    inits::Vector{Int}


    """
        function Composite{T,PG,SG}(panel, subgrids) where {T<:AbstractFloat,PG,SG}

    create Composite grid from panel and subgrids.
    if the boundary grid point of two neighbor subgrids are too close, they will be combined
    in the whole grid.
    """
    function Composite{T,PG,SG}(panel, subgrids) where {T<:AbstractFloat,PG,SG}
        bound = [panel[1], panel[end]]
        @assert panel.size - 1 == length(subgrids)
        inits = zeros(Int, length(subgrids))
        grid = Vector{T}([])
        for i in 1:length(subgrids)
            @assert panel[i] == subgrids[i].bound[1] "$(panel[i])!=$(subgrids[i].bound[1])"
            @assert panel[i+1] == subgrids[i].bound[2] "$(panel[i+1])!=$(subgrids[i].bound[2])"
            if i == 1
                inits[i] = 1
                append!(grid, subgrids[i].grid)
            else
                if abs(grid[end] - subgrids[i].grid[1]) < eps(T) * 10
                    inits[i] = length(grid)
                    append!(grid, subgrids[i].grid[2:end])
                else
                    inits[i] = length(grid) + 1
                    append!(grid, subgrids[i].grid)
                end
            end

        end
        size = length(grid)

        return new{T,PG,SG}(bound, size, grid, panel, subgrids, inits)
    end

end

# function that returns the bottom type of the grid
getbottomtype(grid::CompositeG.Composite{T,PG,SG}) where {T,PG,SG} = (SG <: CompositeG.Composite) ? (getbottomtype(grid.subgrids[1])) : (SG)

"""
    function Base.floor(grid::Composite{T,PG,SG}, x) where {T,PG,SG}

first find the corresponding subgrid by flooring on panel grid,
then floor on subgrid and collect result.
give the floor result on the whole grid.
if floor on panel grid is needed, simply call floor(grid.panel, x).

return 1 for x<grid[1] and grid.size-1 for x>grid[end].
"""
function Base.floor(grid::Composite{T,PG,SG}, x) where {T,PG,SG}
    if SG <: SimpleG.ClosedGrid
        i = floor(grid.panel, x)
        return grid.inits[i] - 1 + floor(grid.subgrids[i], x)
    end

    if x <= grid.grid[1]
        return 1
    elseif x >= grid.grid[end]
        return grid.size - 1
    end

    result = searchsortedfirst(grid.grid, x) - 1
    return result
end

@inline function denseindex(grid::Composite{T,PG,SG}) where {T,PG,SG}
    if PG == SimpleG.Log{T}
        return [(grid.panel.d2s) ? 1 : grid.size,]
        # elseif SG == SimpleG.Log{T}
        #     println("in di",[([grid.inits[i]-1+SimpleG.denseindex(grid.subgrids[i]) for i in 1:grid.size]...)...])
        #     return unique([([grid.inits[i]-1+SimpleG.denseindex(grid.subgrids[i]) for i in 1:grid.size]...)...])
    else
        # for i in 1:length(grid.subgrids)
        #     println(grid.inits[i])
        #     println(length(grid.subgrids[i].grid))
        #     println(grid.subgrids[i].grid)
        #     println(grid.subgrids[i].panel.grid)
        #     println(denseindex(grid.subgrids[i]))
        # end
        return unique([([grid.inits[i] .- 1 .+ denseindex(grid.subgrids[i]) for i in 1:length(grid.subgrids)]...)...])
    end
end

"""
    function CompositeLogGrid(type, bound, N, minterval, d2s, order, T=Float64)

create a composite grid with a Log grid as panel and subgrids of selected type.

#Members:
- `type` : type of the subgrids, currently in [:cheb, :gauss, :uniform]
- `bound` : boundary of the grid
- `N` : number of grid points of panel grid
- `minterval` : minimum interval of panel grid
- `d2s` : panel grid is dense to sparse or not
- `order` : number of grid points of subgrid
"""
function CompositeLogGrid(type, bound, N, minterval, d2s, order, T=Float64, invVandermonde=SimpleG.invvandermonde(order))
    if type == :cheb
        SubGridType = SimpleG.BaryCheb{T}
    elseif type == :gauss
        SubGridType = SimpleG.GaussLegendre{T}
    elseif type == :uniform
        SubGridType = SimpleG.Uniform{T}
    else
        error("$type not implemented!")
    end

    panel = SimpleG.Log{T}(bound, N, minterval, d2s)
    #println("logpanel:",panel.grid)
    subgrids = Vector{SubGridType}([])

    for i in 1:N-1
        _bound = [panel[i], panel[i+1]]
        if type == :cheb
            push!(subgrids, SubGridType(_bound, order, invVandermonde))
        else
            push!(subgrids, SubGridType(_bound, order))
        end
    end

    return Composite{T,SimpleG.Log{T},SubGridType}(panel, subgrids)

end

"""
    function LogDensedGrid(type, bound, dense_at, N, minterval, order, T=Float64)

create a composite grid of CompositeLogGrid as subgrids.
the grid is densed at selected points in dense_at, which in the real situation
could be [kF,] for fermi k grid and [0, 2kF] for bose k grid, etc.
if two densed point is too close to each other, they will be combined.
    
#Members:
- `type` : type of the subgrid of subgrid, currently in [:cheb, :gauss, :uniform]
- `bound` : boundary of the grid
- `dense_at` : list of points that requires densed grid
- `N` : number of grid points of panel grid
- `minterval` : minimum interval of panel grid
- `order` : number of grid points of subgrid
"""
function LogDensedGrid(type, bound, dense_at, N, minterval, order, T=Float64)
    if type == :cheb
        SubGridType = SimpleG.BaryCheb{T}
    elseif type == :gauss
        SubGridType = SimpleG.GaussLegendre{T}
    elseif type == :uniform
        SubGridType = SimpleG.Uniform{T}
    else
        error("$type not implemented!")
    end

    dense_at = sort(dense_at)
    @assert bound[1] <= dense_at[1] <= dense_at[end] <= bound[2]

    dp = Vector{T}([])
    for i in 1:length(dense_at)
        if i == 1
            if abs(dense_at[i] - bound[1]) < minterval
                push!(dp, bound[1])
            elseif abs(dense_at[i] - bound[2]) < minterval
                push!(dp, bound[2])
            else
                push!(dp, dense_at[i])
            end
        elseif i != length(dense_at)
            if abs(dense_at[i] - dp[end]) < minterval
                if dp[end] != bound[1]
                    dp[end] = (dense_at[i] + dense_at[i-1]) / 2.0
                end
            else
                push!(dp, dense_at[i])
            end
        else
            if abs(dense_at[i] - bound[2]) < minterval
                if abs(dp[end] - bound[2]) < minterval
                    dp[end] = bound[2]
                else
                    push!(dp, bound[2])
                end
            elseif abs(dense_at[i] - dp[end]) < minterval
                if dp[end] != bound[1]
                    dp[end] = (dense_at[i] + dense_at[i-1]) / 2.0
                end
            else
                push!(dp, dense_at[i])
            end
        end
    end

    panelgrid = Vector{T}([])
    d2slist = Vector{Bool}([])
    for i in 1:length(dp)
        if i == 1
            push!(panelgrid, bound[1])
            if dp[1] != bound[1]
                push!(panelgrid, dp[1])
                push!(d2slist, false)
            end
        else
            push!(panelgrid, (dp[i] + dp[i-1]) / 2.0)
            if isempty(d2slist)
                push!(d2slist, true)
            else
                push!(d2slist, !d2slist[end])
            end
            push!(panelgrid, dp[i])
            push!(d2slist, !d2slist[end])
        end
    end
    if dp[end] != bound[2]
        push!(panelgrid, bound[2])
        # @assert !d2slist[end] == true
        push!(d2slist, true)
    end

    panel = SimpleG.Arbitrary{T}(panelgrid)
    #println("panel:",panel.grid)
    subgrids = Vector{Composite{T,SimpleG.Log{T},SubGridType}}([])
    invVandermonde = SimpleG.invvandermonde(order)
    for i in 1:length(panel.grid)-1
        push!(subgrids, CompositeLogGrid(type, [panel[i], panel[i+1]], N, minterval, d2slist[i], order, T, invVandermonde))
    end

    return Composite{T,SimpleG.Arbitrary{T},Composite{T,SimpleG.Log{T},SubGridType}}(panel, subgrids)

end

end
