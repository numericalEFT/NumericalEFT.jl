"""
Provide a set of fast math functions
"""
module FastMath
using LinearAlgebra
using Cuba
export dot, squaredNorm, norm

# include("Yeppp.jl")
# using .Yeppp

# @inline dot(x, y) = Yeppp.dot(x, y)
# @inline squaredNorm(x) = Yeppp.dot(x, x)
# @inline norm(x) = sqrt(Yeppp.dot(x, x))

# @inline dot(x, y) = dot(x, y)
# @inline squaredNorm(x) = dot(x, x)
# @inline norm(x) = sqrt(dot(x, x))

"""
    invsqrt(x)

The Legendary Fast Inverse Square Root
See the following links: [wikipedia](https://en.wikipedia.org/wiki/Fast_inverse_square_root) and [thesis](https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf)
"""
@inline function invsqrt(x::Float64)
    #   y = x
    x2::Float64 = x * 0.5
    i::Int64 = reinterpret(Int64, x)
    # The magic number is for doubles is from https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf
    i = 0x5fe6eb50c7b537a9 - (i >> 1)
    y = reinterpret(Float64, i)
    y = y * (1.5 - (x2 * y * y)) # 1st iteration
    y = y * (1.5 - (x2 * y * y))  # 2nd iteration, this can be removed
    return y
end

@inline function invsqrt(x::Float32)
    x2 = x * 0.5f0
    i::Int32 = reinterpret(Int32, x)  # evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1)
    y = reinterpret(Float32, i)
    y = y * (1.5f0 - (x2 * y * y)) # 1st iteration
    y = y * (1.5f0 - (x2 * y * y))  # 2nd iteration, this can be removed
    return y
end

end
