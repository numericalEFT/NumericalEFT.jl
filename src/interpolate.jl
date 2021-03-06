module Interpolate

"""
   linear2D(xgrid, ygrid, data, x, y) 

linear interpolation of data(x, y)

#Arguments:
- xgrid: one-dimensional grid of x
- ygrid: one-dimensional grid of y
- data: two-dimensional array of data
- x: x
- y: y
"""
@inline function linear2D(data, xgrid, ygrid, x, y)

    if (x <= xgrid.grid[firstindex(xgrid)] || x >= xgrid.grid[lastindex(xgrid)] || y <= ygrid.grid[firstindex(ygrid)] || y >= ygrid.grid[lastindex(ygrid)])
        return 0.0
    end
    xidx0 = floor(xgrid, x)
    yidx0 = floor(ygrid, y)
    dx0 = x - xgrid.grid[xidx0]
    dx1 = xgrid.grid[xidx0 + 1] - x
    dy0 = y - ygrid.grid[yidx0]
    dy1 = ygrid.grid[yidx0 + 1] - y

    d00 = data[xidx0, yidx0]
    d01 = data[xidx0, yidx0 + 1]
    d10 = data[xidx0 + 1, yidx0]
    d11 = data[xidx0 + 1, yidx0 + 1]

    g0 = d00 * dx1 + d10 * dx0
    g1 = d01 * dx1 + d11 * dx0

    gx = (g0 * dy1 + g1 * dy0) / (dx0 + dx1) / (dy0 + dy1)
    return gx
end

end # module