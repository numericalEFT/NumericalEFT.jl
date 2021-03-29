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
mutable struct Group
    id::Int
    order::Int
    # internal::Vector{Int}
    nX::Int
    nK::Int
    observable::Any

    reWeightFactor::Float64
    visitedSteps::Float64
    propose::Vector{Float64}
    accept::Vector{Float64}

    function Group(_id, _order, _nX, _nK, _obs)
        # _obs=zeros(_obstype, Tuple(_external))
        # obstype=Array{_obstype, length(_external)}
        propose = Vector{Float64}(undef, 0)
        accept = Vector{Float64}(undef, 0)

        return new(_id, _order, _nX, _nK, _obs, 1.0, 1.0e-6, propose, accept)
    end
end

mutable struct Configuration{TX, TK ,R}
    pid::Int
    totalBlock::Int
    groups::Vector{Group}
    X::TX
    K::TK
    ext::External

    step::Int64
    curr::Group
    rng::R
    absWeight::Float64

    function Configuration(
        _totalBlock,
        _groups,
        _varX::TX,
        _varK::TK,
        _ext;
        pid = nothing,
        rng::R = Random.GLOBAL_RNG,
    ) where {TX, TK,R}
        if (pid == nothing)
            r = Random.RandomDevice()
            pid = abs(rand(r, Int)) % 1000000
        end

        Random.seed!(rng, pid)

        curr = _groups[1]
        config = new{TX, TK, R}(pid, _totalBlock, collect(_groups), _varX, _varK, _ext, 0, curr, rng, 0.0)
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
