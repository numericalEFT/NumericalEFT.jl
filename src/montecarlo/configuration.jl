"""
    Group{A}(type::Int, internal::Vector{Int}, external::Vector{Int}, observable) 

create a group of diagrams

#Arguments:
- type: integer identifier of the group
- internal: array of internal variable numbers, e.g. [number of internal momentum, number of internal tau]
- external: array of size of external index, e.g. [size of external momentum index, size of external tau]
- observable: array of size specified by `external`
"""
mutable struct Group{A<:AbstractArray}
    type::Int
    internal::Vector{Int}
    external::Vector{Int}
    observable::A

    reWeightFactor::Float64
    visitedSteps::Float64
    absWeight::Float64

    function Group{A}(_type, _internal, _external, _obs::A) where A<:AbstractArray
        @assert length(_external)==length(size(_obs)) "number of external variables must be the same as the dimension of the array of the observable"
        return new(_type, collect(_internal), collect(_external), _obs, 1.0, 0.0, eps())
    end
end

# mutable struct Configuration
#     seed::Int
#     step::UInt64
#     var::Any
#     groups::Array{Group}
#     curr::Group
#     rng::Any

#     function Configuration(_var, _groups, _curridx = 1, _seed=nothing, _rng = GLOBAL_RNG)
#         if (_seed==nothing)
#             r=Random.RandomDevice()
#             _seed=rand(r, Int)%1000000
#         end
#         return new(_seed, 0, _var, _groups, _groups[_curridx], _rng)
#     end
# end

# function measure(configuration)
#     curr=configuration.curr

#     factor = 1.0 / curr.absWeight / curr.reWeightFactor
#     weight = eval(configuration)
#     curr.observable[curr.external...] += weight*factor
# end

# function save(obs::OneBody)
#     filename = "$(name())_pid$(curr.PID).jld2"
#     data = Dict("PID" => curr.PID, "Norm" => obs.norm, "Data" => obs.data / obs.norm * obs.phy)

#     for ki in 1:KGridSize
#         println("k=$(Grid.K.grid[ki]): ", sum(obs.data[:, ki, 1]) / length(TauGridSize) * Beta / obs.norm * obs.phy)
#     end

#     # println(obs.data[1,1,1], "norm:", obs.norm)

#     # FileIO.save(filename, data, compress = true)
#     FileIO.save(filename, data)
# end
