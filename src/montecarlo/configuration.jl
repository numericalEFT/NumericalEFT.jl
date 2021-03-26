abstract type Variable end
const MaxOrder=16

mutable struct FermiK{D} <:Variable
    k::Vector{SVector{D, Float64}}
    kF::Float64
    δk::Float64
    maxK::Float64
    function FermiK(dim, kF, δk, maxK, size=MaxOrder)
        k0=SVector{dim, Float64}([kF for i in 1:dim])
        k=[k0 for i in 1:size]
        return new{dim}(k, kF, δk, maxK)
    end
end

mutable struct BoseK{D} <:Variable
    k::Vector{SVector{Float64, D}}
    maxK::Float64
end

mutable struct Tau<:Variable
    t::Vector{Float64}
    λ::Float64
    β::Float64
    function Tau(β=1.0, λ=0.5, size=MaxOrder)
        t=[β/2.0 for i in 1:size]
        return new(t, β)
    end
end

mutable struct TauPair<:Variable
    t::Vector{Float64}
    λ::Float64
    β::Float64
end

mutable struct External<:Variable
    idx::Vector{Int}
    size::Vector{Int}
    function External(size)
        idx=[1 for i in size]
        return new(idx, size)
    end
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
    id::Int
    order::Int
    observable::A
    eval::Function

    reWeightFactor::Float64
    visitedSteps::Float64
    absWeight::Float64

    function Group(_id, _order, _obs::A, _eval::F) where {A, F}
        # _obs=zeros(_obstype, Tuple(_external))
        # obstype=Array{_obstype, length(_external)}
        return new{A, F}(_id, _order, _obs, eval, 1.0, 0.0, eps())
    end
end

mutable struct Configuration{V, R}
    pid::Int
    step::Int64
    var::V
    ext::External
    groups::Tuple{Vararg{Group}}
    curr::Group
    rng::R

    function Configuration(_groups, _var::V, _ext; pid=nothing, rng::R = Random.GLOBAL_RNG) where {V, R}
        if (pid==nothing)
            r=Random.RandomDevice()
            pid=rand(r, Int)%1000000
        end
        curridx=1

        return new{V, R}(pid, 0, _var, _ext, Tuple(_groups), _groups[curridx], rng)
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
