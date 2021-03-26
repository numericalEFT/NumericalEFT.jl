abstract type Variable end

mutable struct FermiK{D} <:Variable
    k::Vector{SVector{Float64, D}}
    maxK::Float64
    kF::Float64
end

mutable struct BoseK{D} <:Variable
    k::Vector{SVector{Float64, D}}
    maxK::Float64
end

mutable struct Tau<:Variable
    t::Vector{Float64}
    β::Float64
end

mutable struct TauPair<:Variable
    t::Vector{Float64}
    β::Float64
end

mutable struct External<:Variable
    idx::Vector{Int}
    size::Vector{Int}
end

"""
    Group{A}(type::Int, internal::Tuple{Vararg{Int}}, external::Tuple{Vararg{Int}}, eval, obstype=Float64) 

create a group of diagrams

#Arguments:
- type: integer identifier of the group
- internal: internal variable numbers, e.g. [number of internal momentum, number of internal tau]
- external: array of size of external index, e.g. [size of external momentum index, size of external tau]
- eval: function to evaluate the group
- obstype: type of the diagram weight, e.g. Float64
"""
mutable struct Group{A<:AbstractArray, F<:Function}
    type::Int
    order::Int
    observable::A
    eval::Function

    reWeightFactor::Float64
    visitedSteps::Float64
    absWeight::Float64

    # function Group(_type, _internal, _external, _obs::T) where T
    #     @assert length(_external)==length(size(_obs)) "number of external variables must be the same as the dimension of the array of the observable"
    #     return new{T}(_type, Tuple(_internal), Tuple(_external), _obs, 1.0, 0.0, eps())
    # end

    function Group(_type, _order, _obs::A, _eval::F) where {A, F}
        # _obs=zeros(_obstype, Tuple(_external))
        # obstype=Array{_obstype, length(_external)}
        return new{A, F}(_type, _order, _obs, eval, 1.0, 0.0, eps())
    end
end

mutable struct Configuration{V, R}
    pid::Int
    step::UInt64
    var::Vector{V}
    groups::Tuple{Vararg{Group}}
    curr::Group
    rng::R

    function Configuration(_groups, _vartype::T; pid=nothing, rng::R = Random.GLOBAL_RNG) where {T, R}
        if (pid==nothing)
            r=Random.RandomDevice()
            pid=rand(r, Int)%1000000
        end
        curridx=1

        _var=Vector{T...}(undef, 0, 0)

        return new{T..., R}(pid, 0, _var, Tuple(_groups), _groups[curridx], rng)
    end
end

function measure(configuration)
    curr=configuration.curr

    factor = 1.0 / curr.absWeight / curr.reWeightFactor
    weight = curr.eval(curr, configuration.step)
    curr.observable[curr.external...] += weight*factor
end

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
