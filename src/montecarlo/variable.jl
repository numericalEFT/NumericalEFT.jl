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
    function FermiK(dim, kF, δk, maxK, size = MaxOrder)
        k0 = SVector{dim,Float64}([kF for i = 1:dim])
        k = [k0 for i = 1:size]
        return new{dim}(k, kF, δk, maxK)
    end
end

mutable struct BoseK{D} <: Variable
    data::Vector{SVector{Float64,D}}
    maxK::Float64
end

mutable struct Tau <: Variable
    data::Vector{Float64}
    λ::Float64
    β::Float64
    function Tau(β = 1.0, λ = 0.5, size = MaxOrder)
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
Base.firstindex(Var::Variable) = Var.data[begin]
Base.lastindex(Var::Variable) = Var.data[end]


mutable struct External
    idx::Vector{Int}
    size::Vector{Int}
    function External(size)
        idx = [1 for i in size]
        return new(idx, size)
    end
end
