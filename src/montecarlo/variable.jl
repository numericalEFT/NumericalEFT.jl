# getindex(X, i)	X[i], indexed element access
# setindex!(X, v, i)	X[i] = v, indexed assignment
# firstindex(X)	The first index, used in X[begin]
# lastindex(X)	The last index, used in X[end]

abstract type Variable end
const MaxOrder = 16


mutable struct FermiK{D} <: Variable
    data::Vector{SVector{D,Float64}}
    kF::Float64
    δk::Float64
    maxK::Float64
    function FermiK(dim, kF, δk, maxK, size=MaxOrder)
        k0 = SVector{dim,Float64}([kF for i = 1:dim])
        k = [k0 for i = 1:size]
        return new{dim}(k, kF, δk, maxK)
    end
end

mutable struct BoseK{D} <: Variable
    data::Vector{SVector{D,Float64}}
    maxK::Float64
end

mutable struct Tau <: Variable
    data::Vector{Float64}
    λ::Float64
    β::Float64
    function Tau(β=1.0, λ=0.5, size=MaxOrder)
        t = [β / 2.0 for i = 1:size]
        return new(t, λ, β)
    end
end

mutable struct TauPair <: Variable
    data::Vector{Float64}
    λ::Float64
    β::Float64
end

Base.getindex(Var::Variable, i::Int) = Var.data[i]
function Base.setindex!(Var::Variable, v, i::Int)
    Var.data[i] = v
end
Base.firstindex(Var::Variable) = Var.data[1]
Base.lastindex(Var::Variable) = Var.data[end]


mutable struct External
    idx::Vector{Int}
    size::Vector{Int}
    function External(size)
        idx = [1 for i in size] # initialize all idx with 1
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
mutable struct Diagram
    id::Int
    order::Int
    nX::Int
    nK::Int

    reWeightFactor::Float64
    visitedSteps::Float64
    propose::Vector{Float64}
    accept::Vector{Float64}

    function Diagram(_id, _order, _nX, _nK)
        propose = Vector{Float64}(undef, 0)
        accept = Vector{Float64}(undef, 0)

        return new(_id, _order, _nX, _nK, 1.0, 1.0e-6, propose, accept)
    end
end

mutable struct Configuration{TX,TK,R}
    pid::Int
    totalStep::Int64
    diagrams::Vector{Diagram}
    X::TX
    K::TK
    ext::External

    step::Int64
    curr::Diagram
    rng::R
    absWeight::Float64 # the absweight of the current diagrams. Store it for fast updates

    function Configuration(totalStep, diagrams, X::TX, K::TK, ext::External; pid=nothing, rng::R=GLOBAL_RNG) where {TX,TK,R}
        if (pid === nothing)
            r = Random.RandomDevice()
            pid = abs(rand(r, Int)) % 1000000
        end
        @assert pid >= 0 "pid should be positive!"
        Random.seed!(rng, pid) # pid will be used as the seed to initialize the random numebr generator

        @assert totalStep > 0 "Total step should be positive!"
        @assert length(diagrams) > 0 "diagrams should not be empty!"
        curr = diagrams[1]
        config = new{TX,TK,R}(pid, Int64(totalStep), collect(diagrams), X, K, ext, 0, curr, rng, 0.0)
        return config
    end
end
