module Renorm
using DataFrames
using DelimitedFiles
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

struct CompositeOrder
    order::Int
    ct::Vector{Int} #counterterm
    function CompositeOrder(order::Int, ct::AbstractVector)
        @assert 0 <= order <= 9
        @assert all(x -> (0 <= x <= 9), ct)
        @assert length(ct) <= 9
        return new(order, ct)
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
Merge interaction order and the main order
(normal_order, G_order, W_order) --> (normal+W_order, G_order)
"""
function mergeCT(data::Dict{CompositeOrder,T}, orderList::AbstractVector, weight=ones(length(orderList))) where {T}
    N = length(keys(data)[1])
    # @assert all(x -> length(x) == N, keys(data)) # check the length of each key is the same
    # @assert all(x -> x <= N, orderList) # check the order exists
    res = Dict{Vector{Int},eltype(vals(data))}()
    for (p, val) in data
        # @assert length(p) == 3
        # println(p)
        mp = (p[1] + p[3], p[2])
        if haskey(res, mp)
            res[mp] += val
        else
            res[mp] = val
        end
    end
    return res
    # else # nothing to merge
    #     @warn("Invalid dataset merge. Dict with same length of keys expected.")
    #     return data
    # end
end

end