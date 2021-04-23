abstract type Variable end
const MaxOrder = 16

mutable struct FermiK{D} <: Variable
    data::Vector{SVector{D,Float64}}
    # data::Vector{Vector{Float64}}
    kF::Float64
    δk::Float64
    maxK::Float64
    function FermiK(dim, kF, δk, maxK, size=MaxOrder)
        k0 = SVector{dim,Float64}([kF for i = 1:dim])
        # k0 = @SVector [kF for i = 1:dim]
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

mutable struct Angle <: Variable
    data::Vector{Float64}
    λ::Float64
    function Angle(λ=0.5, size=MaxOrder)
        theta = [π]
        return new(theta, λ)
    end
end


mutable struct TauPair <: Variable
    data::Vector{MVector{2,Float64}}
    λ::Float64
    β::Float64
    function TauPair(β=1.0, λ=0.5, size=MaxOrder)
        t = [@MVector [β / 3.0, β / 2.0] for i = 1:size]
        return new(t, λ, β)
    end
end

mutable struct Discrete <: Variable
    data::Vector{Int}
    lower::Int
    upper::Int
    size::Int
    function Discrete(lower, upper, size=MaxOrder)
        d = [1 for i in 1:size]
        @assert upper > lower
        return new(d, lower, upper, upper - lower + 1)
    end
end


Base.getindex(Var::Variable, i::Int) = Var.data[i]
function Base.setindex!(Var::Variable, v, i::Int)
    Var.data[i] = v
end
Base.firstindex(Var::Variable) = 1 # return index, not the value
Base.lastindex(Var::Variable) = length(Var.data) # return index, not the value


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
    nvar::Vector{Int}

    reWeightFactor::Float64
    visitedSteps::Float64
    proposeDiag::Vector{Float64}
    acceptDiag::Vector{Float64}
    proposeVar::Vector{Float64}
    acceptVar::Vector{Float64}

    function Diagram(_nvar)
        proposeDiag = Vector{Float64}(undef, 0)
        acceptDiag = Vector{Float64}(undef, 0)
        proposeVar = Vector{Float64}(undef, 0)
        acceptVar = Vector{Float64}(undef, 0)
        return new(_nvar, 1.0, 1.0e-6, proposeDiag, acceptDiag, proposeVar, proposeVar)
    end
end

mutable struct Configuration{V,R,P,O}
    pid::Int
    para::P
    totalStep::Int64
    diagrams::Vector{Diagram}
    neighbor::Vector{Vector{Int}}
    obs::O
    var::V

    step::Int64
    curr::Int
    rng::R
    absWeight::Float64 # the absweight of the current diagrams. Store it for fast updates

    function Configuration(pid, totalStep, diagrams, neighbor, var::V, obs::O, para::P, rng::R) where {V,R,P,O}
        @assert pid >= 0 "pid should be positive!"
        @assert totalStep > 0 "Total step should be positive!"
        @assert length(diagrams) > 0 "diagrams should not be empty!"
        @assert length(diagrams) == length(neighbor) 
        for d in diagrams
            @assert length(d.nvar) == length(var)
        end

        @assert V <: Tuple{Vararg{Variable}} "Configuration.var must be a tuple of Variable to maximize efficiency"

        curr = 1 # set the current diagram to be the first one
        # a small initial absweight makes the initial configuaration quickly updated,
        # so that no error is caused even if the intial absweight is wrong, 
        absweight = 1.0e-10 

        return new{V,R,P,O}(pid, para, Int64(totalStep), collect(diagrams), neighbor, obs, var, 0, curr, rng, absweight)
    end
end

# Macro to make struct
# macro make_struct(struct_name, schema...)
#     fields=[:($(entry.args[1])::$(entry.args[2])) for entry in schema]
#     esc(quote struct $struct_name
#         $(fields...)
#         end
#     end)
# end

macro configuration(schema...)
    fields = [:($(entry.args[1])::$(entry.args[2])) for entry in schema]
    esc(quote struct $struct_name
            $(fields...)
        end
    end)
end
