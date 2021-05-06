abstract type Variable end
const MaxOrder = 16

"""
mutable struct Configuration

    Struct that saves everything needed by MC.

    There are three different pieces of information:

 # Members

 ## Static parameters

 - `seed`: seed to initialize random numebr generator, also serves as the unique pid of the configuration

 - `rng`: a MersenneTwister random number generator, seeded by `seed`

 - `para`: user-defined parameter, could be nothing if not needed

 - `totalStep`: the total number of updates for this configuration

 - `var`: TUPLE of variables, each variable should be derived from the abstract type Variable, see variable.jl for details). Use a tuple rather than a vector improves the performance.

 ## integrand properties
 
 - `neighbor`: vectors that indicates the neighbors of each integrand. e.g., ([2, ], [1, ]) means the neighbor of the first integrand is the second one, while the neighbor of the second integrand is the first. 
    There is a MC update proposes to jump from one integrand to another. If these two integrands' degrees of freedom are very different, then the update is unlikely to be accepted. To avoid this problem, one can specify neighbor to guide the update. 
    By default, we assume the N integrands are in the increase order, meaning the neighbor will be set to ([2, ], [1, 3], [2, 4], ..., [N-1,])

 - `dof`: degrees of freedom of each integrand, e.g., ([0, 1], [2, 3]) means the first integrand has zero var#1 and one var#2; while the second integrand has two var#1 and 3 var#2. 

 - `observable`: observables that is required to calculate the integrands, will be used in the `measure` function call

 - `reweight`: reweight factors for each integrands. If not set, then all factors will be initialized as one.

 - `visited`: how many times this integrand is visited by the Markov chain.

 ## current MC state

 - `step`: the number of MC updates performed up to now

 - `curr`: the current integrand

 - `absWeight`: the abolute weight of the current integrand

 - `propose/accept`: array to store the proposed and accepted updates for each integrands and variables.
    Their shapes are (number of updates X integrand number X max(integrand number, variable number).
    The last index will waste some memory, but the dimension is small anyway.
"""
mutable struct Configuration{V,P,O}
    ########### static parameters ###################
    seed::Int # seed to initialize random numebr generator, also serves as the unique pid of the configuration
    rng::MersenneTwister
    para::P
    totalStep::Int64
    var::V

    ########### integrand properties ##############
    neighbor::Vector{Vector{Int}}
    dof::Vector{Vector{Int}} # degrees of freedom
    observable::O  # observables for each integrand
    reweight::Vector{Float64}
    visited::Vector{Float64}

    ############# current state ######################
    step::Int64
    curr::Int # index of current integrand
    absWeight::Float64 # the absweight of the current diagrams. Store it for fast updates
    normalization::Float64 # normalization factor for observables

    propose::Array{Float64,3} # updates index, integrand index, integrand index
    accept::Array{Float64,3} # updates index, integrand index, integrand index 

    function Configuration(seed, totalStep, var::V, para::P, neighbor, dof, obs::O, reweight) where {V,P,O}
        @assert seed > 0 "seed should be positive!"
        @assert totalStep > 0 "Total step should be positive!"
        Nd = length(neighbor)  # number of integrands
        Nv = length(var) # number of variables

        @assert Nd > 0 "diagrams should not be empty!"
        @assert Nd == length(dof) 
        @assert Nd == length(reweight) 
        for nv in dof
            @assert length(nv) == Nv
        end
        @assert length(reweight) == Nd + 1 "reweight vector size is wrong! Note that the last element in reweight vector is for the normalization diagram."

        rng = MersenneTwister(seed)

        @assert V <: Tuple{Vararg{Variable}} "Configuration.var must be a tuple of Variable to maximize efficiency"

        curr = 1 # set the current diagram to be the first one
        # a small initial absweight makes the initial configuaration quickly updated,
        # so that no error is caused even if the intial absweight is wrong, 
        absweight = 1.0e-10 
        normalization = 1.0e-10

        # visited[end] is for the normalization diagram
        visited = zeros(Float64, Nd + 1) .+ 1.0e-8  # add a small initial value to avoid Inf when inverted

        # propose and accept shape: number of updates X integrand number X max(integrand number, variable number)
        # the last index will waste some memory, but the dimension is small anyway
        propose = zeros(Float64, (2, Nd + 1, max(Nd + 1, Nv))) .+ 1.0e-8 # add a small initial value to avoid Inf when inverted
        accept = zeros(Float64, (2, Nd + 1, max(Nd + 1, Nv))) 

        return new{V,P,O}(seed, rng, para, totalStep, var,  # static parameters
        collect(neighbor), collect(dof), obs, collect(reweight), visited, # integrand properties
        0, curr, absweight, normalization, propose, accept  # current MC state
         ) 
    end
end

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
