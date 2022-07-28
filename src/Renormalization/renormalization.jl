module Renorm
using DataFrames
using DelimitedFiles
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
        return new(order, ct)
    end
    function CompositeOrder(orders::AbstractVector)
        return CompositeOrder(orders[1], orders[2:end])
    end
end

"""
Merge interaction order and the main order
(normal_order, G_order, W_order) --> (normal+W_order, G_order)
"""
function merge(data::Dict{K,T}, orderList::AbstractVector) where {K,T}
    N = length(keys(data)[1])
    @assert all(x -> length(x) == N, keys(data)) # check the length of each key is the same
    @assert all(x -> x <= N, orderList) # check the order exists
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