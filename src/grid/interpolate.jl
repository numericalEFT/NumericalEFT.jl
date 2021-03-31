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

    xarray, yarray = xgrid.grid, ygrid.grid

    if (
        x <= xarray[firstindex(xgrid)] ||
        x >= xarray[lastindex(xgrid)] ||
        y <= yarray[firstindex(ygrid)] ||
        y >= yarray[lastindex(ygrid)]
    )
        return 0.0
    end
    xi0, yi0 = floor(xgrid, x), floor(ygrid, y)
    xi1, yi1 = xi0 + 1, yi0 + 1
    dx0, dx1 = x - xarray[xi0], xarray[xi1] - x
    dy0, dy1 = y - yarray[yi0], yarray[yi1] - y

    d00, d01 = data[xi0, yi0], data[xi0, yi1]
    d10, d11 = data[xi1, yi0], data[xi1, yi1]

    g0 = data[xi0, yi0] * dx1 + data[xi1, yi0] * dx0
    g1 = data[xi0, yi1] * dx1 + data[xi1, yi1] * dx0

    gx = (g0 * dy1 + g1 * dy0) / (dx0 + dx1) / (dy0 + dy1)
    return gx
end
