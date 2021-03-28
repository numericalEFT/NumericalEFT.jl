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
mutable struct Group{A<:AbstractArray,F<:Function}
    id::Int
    internal::Vector{Int}
    observable::A
    eval::Function

    propose::Dict{Symbol, Float64}
    accept::Dict{Symbol, Float64}
    reWeightFactor::Float64
    visitedSteps::Float64
    absWeight::Float64

    function Group(_id, _internal, _obs::A, _eval::F) where {A,F}
        # _obs=zeros(_obstype, Tuple(_external))
        # obstype=Array{_obstype, length(_external)}
        propose=Dict{Symbol, Float64}()
        accept=Dict{Symbol, Float64}()

        return new{A,F}(_id, collect(_internal), _obs, _eval, propose, accept, 1.0, 0.0, eps())
    end
end

mutable struct Configuration{V,R}
    pid::Int
    totalBlock::Int
    groups::Tuple{Vararg{Group}}

    step::Int64
    var::V
    ext::External
    curr::Group
    rng::R

    function Configuration(
        _totalBlock,
        _groups,
        _var::V,
        _ext;
        pid = nothing,
        rng::R = Random.GLOBAL_RNG,
    ) where {V,R}
        if (pid == nothing)
            r = Random.RandomDevice()
            pid = rand(r, Int) % 1000000
        end

        curr=_groups[1]
        config=new{V,R}(pid, _totalBlock, Tuple(_groups), 0, _var, _ext, curr, rng)
        curr.absWeight=curr.eval(config)
        return config
    end
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
