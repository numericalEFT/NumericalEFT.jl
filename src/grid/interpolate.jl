using Statistics
"""
   linear1D(data,xgrid, x) 

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
testInterpolation1D(func, grid1, grid2)

Test interpolation on grid1 with function func, compare results with func(grid2).
Return max deviation and std of deviation.

"""
function testInterpolation1D(func, grid1, grid2)
    data1 = []
    for i in 1:grid1.size
        push!(data1, func(grid1[i]))
    end

    data2 = []
    data2_int = []
    for i in 1:grid2.size
        push!(data2, func(grid2[i]))
        push!(data2_int, linear1D(data1, grid1, grid2[i]))
    end

    d_max = maximum( abs.( data2-data2_int ))
    d_std = std(data2-data2_int)
    return d_max, d_std
end

function testInterpolation1D_rel(func, grid1, grid2)
    data1 = []
    for i in 1:grid1.size
        push!(data1, func(grid1[i]))
    end

    data2 = []
    data2_int = []
    for i in 1:grid2.size
        push!(data2, func(grid2[i]))
        push!(data2_int, linear1D(data1, grid1, grid2[i]))
    end

    d_rel = abs.((data2-data2_int) ./ data2)
    d_max = maximum(d_rel)
    d_std = std(d_rel)
    return d_max, d_std
end
