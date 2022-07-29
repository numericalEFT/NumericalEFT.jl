module Renorm
using DataFrames
using DelimitedFiles
using StaticArrays
export CompositeOrder
# using PyCall
# export mergeInteraction, fromFile, toFile, appendDict, chemicalpotential
# export muCT, zCT
# export z_renormalization, chemicalpotential_renormalization
# export derive_onebody_parameter_from_sigma
# export getSigma

# function partition(order::Int, n::Int)
#     par = Vector{Vector{Int}}()
#     if order == 0
#         return [n]
#     else
#         return [n] + partition(order - 1, n / 2)
#     end
# end

"""
    struct CompositeOrder

composite orders (order and a list of counterterm orders), we will represent with (order; ct1, ct2, ...)

# Arguments
order::Int      : main order
ct::Vector{Int} : counterterm orders

You may create it from a list of integers. The first integer is the main order, and the rest are counterterm orders.
For example,
CompositeOrder([1, 2, 3]) will create an object with main order 1 and counterterm order [2, 3].
"""
struct CompositeOrder{N}
    order::Int
    ct::SVector{N,Int} #counterterm
    function CompositeOrder(order::Int, ct::AbstractVector)
        @assert 0 <= order <= 9
        @assert all(x -> (0 <= x <= 9), ct)
        @assert length(ct) <= 9
        return new{length(ct)}(order, ct)
    end
    function CompositeOrder(orders::AbstractVector)
        return CompositeOrder(orders[1], orders[2:end])
    end
    function CompositeOrder(n::Union{Int32,Int64,Int128})
        # integer to list of digits
        @assert n >= 0
        digits = Vector{Int}()
        while n > 0
            push!(digits, n % 10)
            n = n ÷ 10
        end
        return CompositeOrder(reverse(digits))
    end
    function CompositeOrder(n::String)
        nn = parse(Int128, n)
        return CompositeOrder(nn)
    end
end

function Base.:(==)(a::CompositeOrder, b::CompositeOrder)
    return (a.order == b.order) && (a.ct == b.ct)
end

function Base.:(==)(a::CompositeOrder, b::Union{AbstractVector,Tuple})
    return (a.order == b[1]) && (a.ct == collect(b[2:end]))
end

function Base.:(==)(b::Union{AbstractVector,Tuple}, a::CompositeOrder)
    return (a.order == b[1]) && (a.ct == collect(b[2:end]))
end

function short(order::CompositeOrder)
    @assert length(order.ct) <= 9
    total = Int64(0)
    for (ci, co) in enumerate(order.ct)
        total += co * 10^(length(order.ct) - ci)
    end
    return order.order * 10^length(order.ct) + total
end


"""
    function merge(data::Dict{CompositeOrder,T}, axes) where {T}

Merge counterterm order and the main order
For example, to merge the counterterm with axes = 2
(main_order; G_order, W_order) --> (main_order + W_order; G_order)
"""
function merge(data::Dict{CompositeOrder{N},T}, axes) where {N,T}
    # N = length(keys(data)[1])
    # @assert all(x -> length(x) == N, keys(data)) # check the length of each key is the same
    # @assert all(x -> x <= N, orderList) # check the order exists
    axes = collect(axes)
    @assert N >= length(axes)
    res = Dict{CompositeOrder{N - length(axes)},T}()
    for (p, val) in data
        # @assert length(p) == 3
        # println(p)
        total = p.order + sum(p.ct[axes])
        ct = collect(p.ct)
        deleteat!(ct, axes)
        mp = CompositeOrder(total, ct)
        if haskey(res, mp)
            res[mp] += val
        else
            res[mp] = val
        end
    end
    return res
end

end