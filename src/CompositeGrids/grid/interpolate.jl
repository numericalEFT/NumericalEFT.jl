"""
Provide interpolation and integration.
"""
module Interp

using StaticArrays, FastGaussQuadrature, CompositeGrids

#include("chebyshev.jl")

# include("simple.jl")
# using .SimpleG
# include("composite.jl")
# using .CompositeG

abstract type InterpStyle end
struct LinearInterp <: InterpStyle end
struct ChebInterp <: InterpStyle end
struct CompositeInterp <: InterpStyle end
const LINEARINTERP = LinearInterp()
const CHEBINTERP = ChebInterp()
const COMPOSITEINTERP = CompositeInterp()

InterpStyle(::Type) = LinearInterp()
InterpStyle(::Type{<:SimpleG.BaryCheb}) = ChebInterp()
InterpStyle(::Type{<:CompositeG.Composite}) = CompositeInterp()

# return types of findneighbor, contains complete information for interp
abstract type InterpNeighbor end

# for linear interp, two nearest neighbor are needed
struct LinearNeighbor{T<:AbstractFloat} <: InterpNeighbor
    index::UnitRange{Int}
    grid::SVector{2,T}
    x::T
end

# for cheb interp, whole cheb grid is needed, together with interp weight
# index is recorded as first and last index involved, useful for composite grids
struct ChebNeighbor{T<:AbstractFloat} <: InterpNeighbor
    index::UnitRange{Int}
    grid::Vector{T}
    weight::Vector{T}
    x::T
end

"""
    function findneighbor(xgrid::T, x; method=:default) where {T}

Find neighbor grid points and related information for extrapolating
the value of x on xgrid.

#Members:
- xgrid: grid to be interpolated
- x: value to be interpolated
- method: :default use optimized method, :linear use linear interp.
"""
function findneighbor(xgrid::T, x; method=InterpStyle(T)) where {T}
    return findneighbor(method, xgrid, x)
end

function findneighbor(::LinearInterp, xgrid, x)
    T = eltype(xgrid.grid)
    xi0,xi1 = 0,0
    if(x<=xgrid[firstindex(xgrid)])
        xi0=1
        xi1=2
    elseif(x>=xgrid[lastindex(xgrid)])
        xi0=lastindex(xgrid)-1
        xi1=xi0+1
    else
        xi0=floor(xgrid,x)
        xi1=xi0+1
    end

    x0,x1 = xgrid[xi0], xgrid[xi1]

    return LinearNeighbor{T}(xi0:xi1, [x0,x1], x)
end

function findneighbor(::ChebInterp, xgrid, x)
    T = eltype(xgrid.grid)
    return ChebNeighbor{T}(1:xgrid.size, xgrid.grid, xgrid.weight, x)
end

function findneighbor(::CompositeInterp, xgrid, x)
    if CompositeG.getbottomtype(xgrid) <: SimpleG.BaryCheb
        T = eltype(xgrid.grid)
        curr=xgrid
        xi0 = 1
        while !(typeof(curr)<:SimpleG.BaryCheb)
            i = floor(curr.panel, x)
            xi0 += curr.inits[i]-1
            curr = curr.subgrids[i]
        end
        return ChebNeighbor{T}(xi0:(curr.size-1+xi0), curr.grid, curr.weight, x)
    else
        return findneighbor(LinearInterp(), xgrid, x)
    end
end

"""
    function dataslice(data, axes, indices)

Return a view of data sliced on given axes with given indices.
Works like view(data, (:, ..., :, i_1:f_1,  :, ..., i_n:f_n, :, ..., :)).
Type unstable unless slice dims are constant.

#Members:
- data: data to be sliced.
- axes: axes to be sliced. accept Int or NTuple{DIM, Int} for single or multiple axes. when omitted, assume all axes.
- indices: indices of slicing. accept UnitRange{Int} or Vector of UnitRange{Int} like 2:8 or [2:8, 3:7]
"""
function dataslice(data, axes::Int, indices)
    return selectdim(data, axes, indices[1])
end

function dataslice(data, axes::Int, indices::UnitRange{Int})
    return selectdim(data, axes, indices)
end

function dataslice(data, axes::NTuple{DIM,Int}, indices) where {DIM}
    # @assert DIM == length(indices)
    slice = data
    for i in 1:DIM
        slice = selectdim(slice, axes[i], indices[i])
    end
    return slice
end

@inline function dataslice(data, indices::UnitRange{Int})
    return view(data, indices)
end

@inline function dataslice(data, indices::Vector{UnitRange{Int}})
    return view(data, indices...)
end

"""
    function interpsliced(neighbor, data; axis=1)
Interpolate with given neighbor and sliced data. Assume data already sliced on given axis.

#Members:
- neighbor: neighbor from findneighbor()
- data: sliced data
- axis: axis sliced and to be interpolated
"""
function interpsliced(neighbor, data; axis=1)
    return dropdims(mapslices(u->_interpsliced(neighbor, u), data, dims=axis), dims=axis)
end

function interpsliced(neighbor, data::AbstractMatrix; axis=1)
    # trying to make it type stable for matrix
    if axis == 1
        return map(u->_interpsliced(neighbor,u), eachcol(data))
    elseif axis == 2
        return map(u->_interpsliced(neighbor,u), eachrow(data))
    else
        throw(DomainError(axis, "axis should be 1 or 2 for Matrix"))
    end
end

function interpsliced(neighbor, data::AbstractVector; axis=1)
    return _interpsliced(neighbor, data)
end

# function interpsliced(neighbor, data; axis=1)
#     if ndims(data) == 1
#         return _interpsliced(neighbor, data)
#     else
#         return dropdims(mapslices(u->_interpsliced(neighbor, u), data, dims=axis), dims=axis)
#     end
# end

function _interpsliced(neighbor::LinearNeighbor, data)
    # data should be sliced priorly
    dx0, dx1 = neighbor.x - neighbor.grid[1], neighbor.grid[2] - neighbor.x

    d0, d1 = data[1], data[2]

    g = d0 * dx1 + d1 * dx0

    gx = g / (dx0 + dx1) 
    return gx
end

function _interpsliced(neighbor::ChebNeighbor, data)
    # data should be sliced priorly
    return SimpleG.barycheb(length(neighbor.grid), neighbor.x, data, neighbor.weight, neighbor.grid)
end


function interpND(data, xgrids, xs; method=LINEARINTERP)
    #WARNING: This function works but is not type stable
    dim = length(xs)
    neighbors = [findneighbor(xgrids[i], xs[i]; method) for i in 1:dim]
    indices = [nei.index for nei in neighbors]

    # data_slice = dataslice(data, indices)
    data_slice = view(data, indices...)
    curr_data_slice = copy(data_slice)

    for i in 1:dim-1
        curr_data_slice = interpsliced(neighbors[i], curr_data_slice)
    end

    return interpsliced(neighbors[dim], curr_data_slice)
end

"""
    function linearND(data, xgrids, xs)

linear interpolation of data(xs)

#Arguments:
- xgrids: n-dimensional grids, xgrids[i] is a 1D grid
- data: n-dimensional array of data
- xs: list of x, x[i] corresponds to xgrids[i]
"""
function linearND(data, xgrids, xs)
    @inline function enumX(xs)
        # dim = length(xs)
        # result = ones(Float64, 2^dim)
        # for i in 1:2^dim
        #     for j in 1:dim
        #         result[i] *= xs[j]^digits(i-1,base=2,pad=dim)[j]
        #     end
        # end
        # return result
        return [prod(xs .^ digits(i-1, base=2, pad=length(xs)) ) for i in 1:2^length(xs)]
    end
    @inline function f(as, xs)
        # dim = length(xs)
        # result = 0.0
        # ex = enumX(xs)
        # for i in 1:2^dim
        #     result += as[i]*ex[i]
        # end
        # return result
        return sum(as .* enumX(xs))
    end

    dim = length(xs)

    # find grid points below and above xs
    xis = zeros(Int, (dim, 2))
    for i in 1:dim
        xi0,xi1 = 0,0
        x=xs[i]
        if(x<=xgrids[i].grid[firstindex(xgrids[i])])
            xi0=1
            xi1=2
        elseif(x>=xgrids[i].grid[lastindex(xgrids[i])])
            xi0=lastindex(xgrid)-1
            xi1=xi0+1
        else
            xi0=floor(xgrids[i],xs[i])
            xi1=xi0+1
        end
        xis[i,1], xis[i,2] = xi0, xi1
    end

    # data value at nearby grid points
    datas = [data[[xis[j , 1+digits(i-1,base=2,pad=dim)[j]]  for j in 1:dim]...] for i in 1:2^dim]
    # datas = zeros(Float64, 2^dim)
    # for i in 1:2^dim
    #     datas[i] = data[[xis[j , 1+digits(i,base=2,pad=dim)[j]]  for j in 1:dim]...]
    # end

    # create x matrix
    xmat = ones(Float64, (2^dim, 2^dim))
    for i in 1:2^dim
        ii = digits(i-1, base=2,pad=dim)
        xxs = [xgrids[j][xis[j, 1+ii[j]]] for j in 1:dim]
        xmat[:, i] = enumX(xxs)
    end

    xtran = copy(transpose(xmat))
    as = xtran\datas
    #println(xtran)
    #println(as)
    #println(xtran*as)
    #println(datas)
    #@assert xtran*as == datas "$(xtran*as), $(datas), $(xtran)"

    return f(as, xs)
end

@inline function linearND(data::Matrix, xgrids, xs)
    # for 2d, use linear2D
    @assert length(xgrids)==length(xs)==2
    return linear2D(data, xgrids[1],xgrids[2],xs[1],xs[2])
end

@inline function linearND(data::Vector, xgrids, xs)
    # for 2d, use linear2D
    @assert length(xgrids)==length(xs)==1
    return linear2D(data, xgrids[1],xs[1])
end

"""
   linear2D(data, xgrid, ygrid, x, y) 

linear interpolation of data(x, y)

#Arguments:
- xgrid: one-dimensional grid of x
- ygrid: one-dimensional grid of y
- data: two-dimensional array of data
- x: x
- y: y
"""
@inline function linear2D(data, xgrid, ygrid, x, y)

    xarray, yarray = xgrid.grid, ygrid.grid

    # if (
    #     x <= xarray[firstindex(xgrid)] ||
    #     x >= xarray[lastindex(xgrid)] ||
    #     y <= yarray[firstindex(ygrid)] ||
    #     y >= yarray[lastindex(ygrid)]
    # )
    #     return 0.0
    # end
    # xi0, yi0 = floor(xgrid, x), floor(ygrid, y)
    # xi1, yi1 = xi0 + 1, yi0 + 1

    xi0,xi1,yi0,yi1 = 0,0,0,0
    if(x<=xarray[firstindex(xgrid)])
        xi0=1
        xi1=2
    elseif(x>=xarray[lastindex(xgrid)])
        xi0=lastindex(xgrid)-1
        xi1=xi0+1
    else
        xi0=floor(xgrid,x)
        xi1=xi0+1
    end

    if(y<=yarray[firstindex(ygrid)])
        yi0=1
        yi1=2
    elseif(y>=yarray[lastindex(ygrid)])
        yi0=lastindex(ygrid)-1
        yi1=yi0+1
    else
        yi0=floor(ygrid,y)
        yi1=yi0+1
    end

    dx0, dx1 = x - xarray[xi0], xarray[xi1] - x
    dy0, dy1 = y - yarray[yi0], yarray[yi1] - y

    d00, d01 = data[xi0, yi0], data[xi0, yi1]
    d10, d11 = data[xi1, yi0], data[xi1, yi1]

    g0 = data[xi0, yi0] * dx1 + data[xi1, yi0] * dx0
    g1 = data[xi0, yi1] * dx1 + data[xi1, yi1] * dx0

    gx = (g0 * dy1 + g1 * dy0) / (dx0 + dx1) / (dy0 + dy1)
    return gx
end

"""
    function linear1D(data, xgrid, x)

linear interpolation of data(x)

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: x
"""
@inline function linear1D(data, xgrid, x)

    xarray = xgrid.grid

    xi0,xi1 = 0,0
    if(x<=xarray[firstindex(xgrid)])
        xi0=1
        xi1=2
    elseif(x>=xarray[lastindex(xgrid)])
        xi0=lastindex(xgrid)-1
        xi1=xi0+1
    else
        xi0=floor(xgrid,x)
        xi1=xi0+1
    end

    dx0, dx1 = x - xarray[xi0], xarray[xi1] - x

    d0, d1 = data[xi0], data[xi1]

    g = d0 * dx1 + d1 * dx0

    gx = g / (dx0 + dx1) 
    return gx
end

"""
    function interp1D(data, xgrid, x; axis=1, method=InterpStyle(T))

linear interpolation of data(x) with single or multiple dimension.
For 1D data, return a number; for multiple dimension, reduce the given axis.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: x
- axis: axis to be interpolated in data
- method: by default use optimized method; use linear interp if Interp.LinearInterp()
"""
function interp1D(data, xgrid::T, x; axis=1, method=InterpStyle(T)) where {T}
    return dropdims(mapslices(u->interp1D(method, u, xgrid, x), data, dims=axis), dims=axis)
end

function interp1D(data::AbstractMatrix, xgrid::T, x; axis=1, method=InterpStyle(T)) where {T}
    # trying to make it type stable for matrix
    if axis == 1
        return map(u->interp1D(method, u, xgrid, x), eachcol(data))
    elseif axis == 2
        return map(u->interp1D(method, u, xgrid, x), eachrow(data))
    else
        throw(DomainError(axis, "axis should be 1 or 2 for Matrix"))
    end
end

function interp1D(data::AbstractVector, xgrid::T, x;axis=1, method=InterpStyle(T)) where {DT,T}
    return interp1D(method, data, xgrid, x)
end

"""
    function interp1D(::LinearInterp,data, xgrid, x)

linear interpolation of data(x), use floor and linear1D

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: x
"""
function interp1D(::LinearInterp,data, xgrid, x)
    return linear1D(data, xgrid, x)
end

"""
    function interp1D(::ChebInterp, data, xgrid, x)

linear interpolation of data(x), barycheb for BaryCheb grid

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: x
"""
function interp1D(::ChebInterp, data, xgrid, x)
    return SimpleG.barycheb(xgrid.size, x, data, xgrid.weight, xgrid.grid)
end

"""
    function interp1D(::CompositeInterp,data, xgrid, x)

linear interpolation of data(x),
first floor on panel to find subgrid, then call interp1D on subgrid 

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: x
"""
function interp1D(::CompositeInterp,data, xgrid, x)
    i = floor(xgrid.panel, x)
    head, tail = xgrid.inits[i], xgrid.inits[i]+xgrid.subgrids[i].size-1
    return interp1D(view(data, head:tail), xgrid.subgrids[i], x)
end


"""
    function interp1DGrid(data, xgrid, grid; axis=1, method=InterpStyle(T))
For 1D data, do interpolation of data(grid[1:end]), return a Vector.
For ND data, do interpolation of data(grid[1:end]) at given axis, return data of same dimension.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- grid: points to be interpolated on
- axis: axis to be interpolated in data
- method: by default use optimized method; use linear interp if :linear
"""
function interp1DGrid(data, xgrid::T, grid; axis=1, method=InterpStyle(T)) where {T}
    return mapslices(u->interp1DGrid(method, u, xgrid, grid), data, dims=axis)
end

function interp1DGrid(data::AbstractVector, xgrid::T, grid; axis=1, method=InterpStyle(T)) where {T}
    return interp1DGrid(method, data, xgrid, grid)
end

"""
    function interp1DGrid(::Union{LinearInterp,ChebInterp}, data, xgrid, grid)

linear interpolation of data(grid[1:end]), return a Vector
simply call interp1D on each points

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- grid: points to be interpolated on
"""
function interp1DGrid(::Union{LinearInterp,ChebInterp}, data, xgrid, grid)
    ff = zeros(eltype(data), length(grid))
    for (xi, x) in enumerate(grid)
        ff[xi] = interp1D(data, xgrid, x)
        # if x == 1.1386851268496132
        #     println(xgrid.bound)
        #     println(xgrid.grid)
        # end
    end
    return ff
end

"""
    function interp1DGrid(::CompositeInterp, data, xgrid, grid)

linear interpolation of data(grid[1:end]), return a Vector
grid should be sorted.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- grid: points to be interpolated on
"""
function interp1DGrid(::CompositeInterp, data, xgrid, grid)
    ff = zeros(eltype(data), length(grid))

    init, curr = 1, 1
    for pi in 1:xgrid.panel.size-1
        if grid[init]< xgrid.panel[pi+1]
            head, tail = xgrid.inits[pi], xgrid.inits[pi]+xgrid.subgrids[pi].size-1
            while grid[curr]<xgrid.panel[pi+1] && curr<length(grid)
                curr += 1
            end
            if grid[curr]<xgrid.panel[pi+1] && curr==length(grid)
                @assert xgrid.subgrids[pi].bound[1]<=grid[init]<=grid[curr]<=xgrid.subgrids[pi].bound[2]
                ff[init:curr] = interp1DGrid(view(data, head:tail), xgrid.subgrids[pi], view(grid, init:curr))
                return ff
            else
                @assert xgrid.subgrids[pi].bound[1]<=grid[init]<=grid[curr-1]<=xgrid.subgrids[pi].bound[2]
                ff[init:curr-1] = interp1DGrid(view(data, head:tail), xgrid.subgrids[pi], view(grid, init:curr-1))
            end
            # println(view(data, head:tail))
            # println(xgrid.subgrids[pi].grid)
            # println(grid[init:curr-1])
            # println(ff[init:curr-1])
            init = curr
        end
    end
    return ff
end

abstract type IntegrateStyle end
struct WeightIntegrate <: IntegrateStyle end
struct NoIntegrate <: IntegrateStyle end
struct CompositeIntegrate <: IntegrateStyle end
struct ChebIntegrate <: IntegrateStyle end
const WEIGHTINTEGRATE = WeightIntegrate()
const NOINTEGRATE = NoIntegrate()
const COMPOSITEINTEGRATE = CompositeIntegrate()
const CHEBINTEGRATE = ChebIntegrate()

IntegrateStyle(::Type) = NoIntegrate()
IntegrateStyle(::Type{<:SimpleG.BaryCheb}) = ChebIntegrate()
IntegrateStyle(::Type{<:SimpleG.GaussLegendre}) = WeightIntegrate()
IntegrateStyle(::Type{<:SimpleG.Uniform}) = WeightIntegrate()
IntegrateStyle(::Type{<:SimpleG.Arbitrary}) = WeightIntegrate()
IntegrateStyle(::Type{<:SimpleG.Log}) = WeightIntegrate()
IntegrateStyle(::Type{<:CompositeG.Composite}) = CompositeIntegrate()


"""
    function integrate1D(data, xgrid; axis=1)

calculate integration of data[i] on xgrid.
For 1D data, return a number; for multiple dimension, reduce the given axis.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- axis: axis to be integrated in data
"""
function integrate1D(data, xgrid::T; axis=1) where {T}
    return dropdims(mapslices(u->integrate1D(IntegrateStyle(T), u, xgrid), data, dims=axis), dims=axis)
end

function integrate1D(data::AbstractMatrix, xgrid::T; axis=1) where {T}
    if axis == 1
        return map(u->integrate1D(IntegrateStyle(T), u, xgrid), eachcol(data))
    elseif axis == 2
        return map(u->integrate1D(IntegrateStyle(T), u, xgrid), eachrow(data))
    else
        throw(DomainError(axis, "axis should be 1 or 2 for Matrix"))
    end
end

function integrate1D(data::AbstractVector, xgrid::T; axis=1) where {T}
    return integrate1D(IntegrateStyle(T), data, xgrid)
end

"""
    function integrate1D(::NoIntegrate, data, xgrid)

calculate integration of data[i] on xgrid
works for grids that do not have integration weight stored

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
"""
function integrate1D(::NoIntegrate, data, xgrid)
    result = eltype(data)(0.0)

    grid = xgrid.grid
    for i in 1:xgrid.size
        if i==1
            weight = 0.5*(grid[2]-xgrid.bound[1])
        elseif i==xgrid.size
            weight = 0.5*(xgrid.bound[2]-grid[end-1])
        else
            weight = 0.5*(grid[i+1]-grid[i-1])
        end
        result += data[i]*weight
    end
    return result
end

"""
    function integrate1D(::WeightIntegrate, data, xgrid)

calculate integration of data[i] on xgrid
works for grids that have integration weight stored

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
"""
function integrate1D(::WeightIntegrate, data, xgrid)
    result = eltype(data)(0.0)

    for i in 1:xgrid.size
        result += data[i]*xgrid.weight[i]
    end
    return result
end

"""
    function integrate1D(::ChebIntegrate, data, xgrid)

calculate integration of data[i] on xgrid
works for grids that have integration weight stored

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
"""
function integrate1D(::ChebIntegrate, data, xgrid)
    a, b = xgrid.bound[1], xgrid.bound[2]
    return SimpleG.chebint(xgrid.size, -1.0, 1.0, data, xgrid.invVandermonde)*(b-a)/2.0
end

"""
    function integrate1D(::CompositeIntegrate, data, xgrid)

calculate integration of data[i] on xgrid
call integrate1D for each subgrid and return the sum.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
"""
function integrate1D(::CompositeIntegrate, data, xgrid)
    result = eltype(data)(0.0)

    for pi in 1:xgrid.panel.size-1
        head, tail = xgrid.inits[pi], xgrid.inits[pi]+xgrid.subgrids[pi].size-1
        result += integrate1D( view(data, head:tail),xgrid.subgrids[pi])
        currgrid = xgrid.subgrids[pi]
    end
    return result

end

"""
    function integrate1D(data, xgrid, range; axis=1)

calculate integration of data[i] on xgrid.
For 1D data, return a number; for multiple dimension, reduce the given axis.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- range: range of integration, [init, fin] within bound of xgrid.
- axis: axis to be integrated in data
"""
function integrate1D(data, xgrid::T, range; axis=1) where {T}
    return dropdims(mapslices(u->integrate1D(IntegrateStyle(T), u, xgrid, range), data, dims=axis), dims=axis)
end

function integrate1D(data::AbstractMatrix, xgrid::T, range; axis=1) where {T}
    if axis == 1
        return map(u->integrate1D(IntegrateStyle(T), u, xgrid, range), eachcol(data))
    elseif axis == 2
        return map(u->integrate1D(IntegrateStyle(T), u, xgrid, range), eachrow(data))
    else
        throw(DomainError(axis, "axis should be 1 or 2 for Matrix"))
    end
end

function integrate1D(data::AbstractVector, xgrid::T, range; axis=1) where {T}
    return integrate1D(IntegrateStyle(T), data, xgrid, range)
end

@inline function trapezoidInt(data, grid, range)
    return (0.5*(range[1]+range[2])*(data[2]-data[1]) - grid[1]*data[2] + grid[2]*data[1])/(grid[2]-grid[1])*(range[2]-range[1])
end

function integrate1D(::NoIntegrate, data, xgrid, range)
    sign, x1, x2 = (range[1]<range[2]) ? (1.0, range[1], range[2]) : (-1.0, range[2], range[1])
    @assert xgrid.bound[1] <= x1 <= x2 <= xgrid.bound[2]
    xi1, xi2 = floor(xgrid, x1), floor(xgrid, x2)

    result = eltype(data)(0.0)

    grid = xgrid.grid
    # g[xi1], x1, g[xi1+1], g[xi1+2], ... , g[xi2], x2
    if xi1 == xi2
        result += trapezoidInt(view(data, xi1:xi1+1),view(grid, xi1:xi1+1),[x1, x2])
    else
        for i in xi1:xi2
            if i==xi1
                result += trapezoidInt(view(data, i:i+1),view(grid, i:i+1),[x1,grid[i+1]])
            elseif i==xi2
                result += trapezoidInt(view(data, i:i+1),view(grid, i:i+1),[grid[i],x2])
            else
                result += 0.5*(data[i]+data[i+1])*(grid[i+1]-grid[i])
            end
        end
    end
    return result*sign
end

function integrate1D(::ChebIntegrate, data, xgrid, range)
    a, b = xgrid.bound[1], xgrid.bound[2]
    x1, x2 = range[1], range[2]
    c1, c2 = (2x1-a-b)/(b-a), (2x2-a-b)/(b-a)
    return SimpleG.chebint(xgrid.size, c1, c2, data, xgrid.invVandermonde)*(b-a)/2.0
end

function integrate1D(::WeightIntegrate, data, xgrid, range)
    return integrate1D(NoIntegrate(), data, xgrid, range)
    # may give bad result for GaussLegendre grid
    # sign, x1, x2 = (range[1]<range[2]) ? (1.0, range[1], range[2]) : (-1.0, range[2], range[1])
    # @assert xgrid.bound[1] <= x1 <= x2 <= xgrid.bound[2]
    # xi1, xi2 = floor(xgrid, x1), floor(xgrid, x2)

    # result = eltype(data)(0.0)
    # grid=xgrid.grid
    # for i in xi1+1:xi2
    #     if i==xi1+1
    #         weight = 0.5*(grid[i+1]-x1)
    #     elseif i==xi2
    #         weight = 0.5*(x2-grid[xi2])
    #     else
    #         weight = xgrid.weight[i]
    #     end
    #     result += data[i]*weight
    # end
    # return result*sign
end

function integrate1D(::CompositeIntegrate, data, xgrid, range)
    sign, x1, x2 = (range[1]<range[2]) ? (1.0, range[1], range[2]) : (-1.0, range[2], range[1])
    @assert xgrid.bound[1] <= x1 <= x2 <= xgrid.bound[2]
    pi1, pi2 = floor(xgrid.panel, x1), floor(xgrid.panel, x2)

    result = eltype(data)(0.0)
    if pi1==pi2
        pi = pi1
        head, tail = xgrid.inits[pi], xgrid.inits[pi]+xgrid.subgrids[pi].size-1
        result += integrate1D(view(data, head:tail), xgrid.subgrids[pi], [x1,x2])
    else
        for pi in pi1:pi2
            head, tail = xgrid.inits[pi], xgrid.inits[pi]+xgrid.subgrids[pi].size-1
            if pi == pi1
                result += integrate1D(view(data, head:tail), xgrid.subgrids[pi], [x1,xgrid.subgrids[pi].bound[2]])
            elseif pi == pi2
                result += integrate1D(view(data, head:tail), xgrid.subgrids[pi], [xgrid.subgrids[pi].bound[1],x2])
            else
                result += integrate1D( view(data, head:tail),xgrid.subgrids[pi])
            end
        end
    end
    return result*sign
end

abstract type DifferentiateStyle end
struct NoDifferentiate <: DifferentiateStyle end
struct ChebDifferentiate <: DifferentiateStyle end
struct CompositeDifferentiate <: DifferentiateStyle end
const NODIFFERENTIATE = NoDifferentiate()
const COMPOSITEDIFFERENTIATE = CompositeDifferentiate()
const CHEBDIFFERENTIATE = ChebDifferentiate()

DifferentiateStyle(::Type) = NoDifferentiate()
DifferentiateStyle(::Type{<:SimpleG.BaryCheb}) = ChebDifferentiate()
DifferentiateStyle(::Type{<:CompositeG.Composite}) = CompositeDifferentiate()

"""
    function differentiate1D(data, xgrid, x; axis=1)

calculate integration of data[i] on xgrid.
For 1D data, return a number; for multiple dimension, reduce the given axis.

#Arguments:
- xgrid: one-dimensional grid of x
- data: one-dimensional array of data
- x: point to differentiate
- axis: axis to be differentiated in data
"""
function differentiate1D(data, xgrid::T, x; axis=1) where {T}
    return dropdims(mapslices(u->differentiate1D(DifferentiateStyle(T), u, xgrid, x), data, dims=axis), dims=axis)
end

function differentiate1D(data::AbstractMatrix, xgrid::T, x; axis=1) where {T}
    if axis == 1
        return map(u->differentiate1D(DifferentiateStyle(T), u, xgrid,x), eachcol(data))
    elseif axis == 2
        return map(u->differentiate1D(DifferentiateStyle(T), u, xgrid,x), eachrow(data))
    else
        throw(DomainError(axis, "axis should be 1 or 2 for Matrix"))
    end
end

function differentiate1D(data::AbstractVector, xgrid::T, x; axis=1) where {T}
    return differentiate1D(DifferentiateStyle(T), data, xgrid,x)
end

function differentiate1D(::NoDifferentiate, data, xgrid, x)
    #simple numerical differentiate
    xi = floor(xgrid, x)
    return (data[xi+1]-data[xi])/(xgrid[xi+1]-xgrid[xi])
end

function differentiate1D(::ChebDifferentiate, data, xgrid, x)
    a, b = xgrid.bound[1], xgrid.bound[2]
    c = (2x-a-b)/(b-a)
    return SimpleG.chebdiff(xgrid.size, c, data, xgrid.invVandermonde)/(b-a)*2.0
end

function differentiate1D(::CompositeDifferentiate, data, xgrid, x)
    i = floor(xgrid.panel, x)
    head, tail = xgrid.inits[i], xgrid.inits[i]+xgrid.subgrids[i].size-1
    return differentiate1D(view(data, head:tail), xgrid.subgrids[i], x)
end

end
