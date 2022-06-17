"""
barychebinit(n)

Get Chebyshev nodes of first kind and corresponding barycentric Lagrange interpolation weights. 
Reference: Berrut, J.P. and Trefethen, L.N., 2004. Barycentric lagrange interpolation. SIAM review, 46(3), pp.501-517.

# Arguments
- `n`: order of the Chebyshev interpolation

# Returns
- Chebyshev nodes
- Barycentric Lagrange interpolation weights
"""
function barychebinit(n)
    x = zeros(Float64, n)
    w = similar(x)
    for i in 1:n
        c = (2i - 1)π / (2n)
        x[n - i + 1] = cos(c)
        w[n - i + 1] = (-1)^(i - 1) * sin(c)
    end
    return x, w
end

function vandermonde(x)
    n = length(x)
    vmat = zeros(Float64, (n, n))
    for i in 1:n
        for j in 1:n
            vmat[i,j] = x[i]^(j-1)
        end
    end
    return vmat
end

function invvandermonde(order)
    n = order
    vmat = zeros(Float64, (n, n))
    for i in 1:n
        for j in 1:n
            c = (2n-2i+1)π/(2n)
            x = cos(c)
            vmat[i,j] = x^(j-1)
        end
    end
    return inv(transpose(vmat))
end

@inline function divfactorial(j, i)
    if i == 0
        return 1
    elseif i == 1
        return 1/j
    elseif i == -1
        return j-1
    else
        return factorial(j-1) / factorial(j+i-1)
    end
end

function weightcoef(a, i::Int, n)
    # integrate when i=1; differentiate when i=-1
    b = zeros(Float64, n)
    for j in 1:n
        if j+i-1 > 0
            b[j] = a^(j+i-1) * divfactorial(j, i)
        elseif j+i-1 == 0
            b[j] = 1
        else
            b[j] = 0
        end
    end
    return b
end

@inline function calcweight(invmat, b)
    return invmat*b
end

"""
function barycheb(n, x, f, wc, xc)

Barycentric Lagrange interpolation at Chebyshev nodes
Reference: Berrut, J.P. and Trefethen, L.N., 2004. Barycentric lagrange interpolation. SIAM review, 46(3), pp.501-517.

# Arguments
- `n`: order of the Chebyshev interpolation
- `x`: coordinate to interpolate
- `f`: array of size n, function at the Chebyshev nodes
- `wc`: array of size n, Barycentric Lagrange interpolation weights
- `xc`: array of size n, coordinates of Chebyshev nodes

# Returns
- Interpolation result
"""
function barycheb(n, x, f, wc, xc)
    for j in 1:n
        if x == xc[j]
            return f[j]
        end    
    end

    num, den = 0.0, 0.0
    for j in 1:n
        q = wc[j] / (x - xc[j])
        num += q * f[j]
        den += q
    end
    return num / den
end

function barycheb2(n, x, f, wc, xc)
    for j in 1:n
        if x == xc[j]
            return f[j, :]
        end    
    end

    den = 0.0
    num = zeros(eltype(f), size(f)[2])
    for j in 1:n
        q = wc[j] / (x - xc[j])
        num += q .* f[j, :]
        den += q
    end
    return num ./ den
end

function chebint(n, a, b, f, invmat)
    wc = weightcoef(b, 1, n) .- weightcoef(a, 1, n)
    intw = calcweight(invmat, wc)
    return sum(intw .* f)
end

function chebdiff(n, x, f, invmat)
    wc = weightcoef(x, -1, n)
    intw = calcweight(invmat, wc)
    return sum(intw .* f)
end

