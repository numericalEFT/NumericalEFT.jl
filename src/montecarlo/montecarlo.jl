"""
Monte Carlo Calculator for Diagrams
"""
module MonteCarlo

export montecarlo, Configuration, Diagram, FermiK, BoseK, Tau, TauPair

using Random, MPI
using LinearAlgebra
using StaticArrays, Printf, Dates
using ..Utility
const RNG = Random.GLOBAL_RNG

include("variable.jl")
include("sampler.jl")
include("updates.jl")
include("statistics.jl")

"""

sample(totalStep, var, dof::Vector{Vector{Int}}, obs, integrand::Function, measure::Function; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, reweight=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

 sample the integrands, collect statistics, and return the expected values and errors

 # Arguments

 - `totalStep`: total number of updates
 
 - `var`: TUPLE of variables, each variable should be derived from the abstract type Variable, see variable.jl for details). Use a tuple rather than a vector improves the performance.

 - `dof`: degrees of freedom of each integrand, e.g., ([0, 1], [2, 3]) means the first integrand has zero var#1 and one var#2; while the second integrand has two var#1 and 3 var#2. 

 - `obs`: observables that is required to calculate the integrands, will be used in the `measure` function call

 - `integrand`: function call to evaluate the integrand. It should accept an argument of the type `Configuration`, and return a weight. 
    Internally, MC only samples the absolute value of the weight. Therefore, it is also important to define Main.abs for the weight if its type is user-defined. 

- `measure`: function call to measure. It should accept an argument of the type `Configuration`, then manipulate observables `obs`. 

- `Nblock`: repeat times. The tasks will automatically distributed to multi-process if `Distributed` package is imported globally.

- `para`: user-defined parameter that is useful in the functions `integrand`, `measure`, `normalize`.

- `neighbor`: vectors that indicates the neighbors of each integrand. e.g., ([2, ], [1, ]) means the neighbor of the first integrand is the second one, while the neighbor of the second integrand is the first. 
    There is a MC update proposes to jump from one integrand to another. If these two integrands' degrees of freedom are very different, then the update is unlikely to be accepted. To avoid this problem, one can specify neighbor to guide the update. 
    By default, we assume the N integrands are in the increase order, meaning the neighbor will be set to ([2, ], [1, 3], [2, 4], ..., [N-1,])
    
- `seed`: the seed for random number generator. If `seed` is set, then the random number generator of each blocks will be initialized with seed+1, seed+2, .... On the other hand, if `seed` is nothing, then MC will call `RandomDevice()` to get a system-generated seed for each block. Since `seed` is different for each block, it could also be used as the id of each block.

- `reweight`: reweight factors for each integrands. If not set, then all factors will be initialized as one.

- `print`: -1 to not print anything, 0 to print minimal information, >0 to print summary for every `print` seconds

- `printio`: `io` to print the information

- `save`: -1 to not save anything, 0 to save observables `obs` in the end of sampling, >0 to save observables `obs` for every `save` seconds

- `saveio`: `io` to save
"""
function sample(totalStep, var, dof::Vector{Vector{Int}}, obs, integrand::Function, measure::Function; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, reweight=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

    ################# diagram initialization #########################
    Nd = length(dof) # number of integrands
    @assert Nd > 0 "At least one integrand is required."

    # add normalization diagram to dof
    dof = deepcopy(dof) # don't modify the input dof
    push!(dof, zeros(Int, length(var))) # add the degrees of freedom for the normalization diagram

    if isnothing(neighbor)
        # By default, only the order-1 and order+1 diagrams are considered to be the neighbors
        # Nd+1 is the normalization diagram, by default, it only connects to the diagram with index 1
        neighbor = Vector{Vector{Int}}([])
        for di in 1:Nd + 1
            if di == 1 # 1 to norm and 2
                # if Nd=1, then 2 is the normalization diagram
                push!(neighbor, Nd == 1 ? [2, ] : [Nd + 1, 2]) 
            elseif di == Nd + 1 # norm to 1
                push!(neighbor, [1, ])  
            elseif di == Nd # last diag to the second last
                push!(neighbor, [Nd - 1,])
            else
                push!(neighbor, [di - 1, di + 1]) 
            end
        end
    end

    ############# initialize reweight factors ########################
    doReweight = false
    if isnothing(reweight)
        reweight = [1.0 for d in 1:Nd + 1] # the last element is for the normalization diagram
        doReweight = true
    end

    ############ initialized timer ####################################
    if print > 0
        push!(timer, StopWatch(print, printSummary))
    end

    ########### initialized MPI #######################################
    if MPI.Initialized() == false
    MPI.Init()
    end
    comm = MPI.COMM_WORLD
    size = MPI.Comm_size(comm)
    rank = MPI.Comm_rank(comm)
    root = 0
    Nblock = (Nblock ÷ size) * size # make Nblock % size ==0
    @assert Nblock % size == 0

    #########  construct configurations for each block ################
    steps = totalStep ÷ Nblock
    obsSum, obsSquaredSum = zero(obs), zero(obs)

    summary = nothing

    for i in 1:Nblock
        # MPI thread rank will run the block with the indexes: rank, rank+size, rank+2size, ...
        if i % size != rank 
            continue
        end

        if isnothing(seed)
            seedi = rand(Random.RandomDevice(), 1:1000000)
        else
            seedi = i + abs(seed)
        end
        # obscopied = deepcopy(obs) # copied observables for each block 
        fill!(obs, zero(eltype(obs))) # reinialize observable
        config = Configuration(seedi, steps, var, para, neighbor, dof, obs, reweight)

        config = montecarlo(config, integrand, measure, print, save, timer, doReweight)

        summary = addStat!(config, summary)

        obsSum .+= config.observable ./ config.normalization
        obsSquaredSum .+= (config.observable ./ config.normalization).^2
        reweight = config.reweight
    end

    #################### collect statistics  ####################################
    MPI.Reduce!(obsSum, MPI.SUM, root, comm) # root node gets the sum of observables from all blocks
    MPI.Reduce!(obsSquaredSum, MPI.SUM, root, comm) # root node gets the squared sum of observables from all blocks
    summary = reduceStat(summary, root, comm)

    if MPI.Comm_rank(comm) == root
        ################################ IO ######################################
        if (print >= 0)
            printSummary(summary, neighbor, var)
        end
        ##################### Extract Statistics  ################################
        mean = obsSum ./ Nblock
        std = @. sqrt((obsSquaredSum / Nblock - mean^2) / (Nblock - 1))
        MPI.Finalize()
        return mean, std
    else # if not the root, return nothing
        MPI.Finalize()
        return nothing, nothing
    end
end

function montecarlo(config::Configuration, integrand::Function, measure::Function, print, save, timer, doReweight)
    ##############  initialization  ################################
    # don't forget to initialize the diagram weight
    config.absWeight = abs(integrand(config))
    
    updates = [changeIntegrand, changeVariable] # TODO: sample changeVariable more often
    # updates = [changeVariable] # TODO: sample changeVariable more often

    ########### MC simulation ##################################
    if (print >= 0)
        printstyled("Seed $(config.seed) Start Simulation ...\n", color=:red)
    end
    startTime = time()
        
    for i = 1:config.totalStep
        config.step += 1
        config.visited[config.curr] += 1
        _update = rand(config.rng, updates) # randomly select an update
        _update(config, integrand)
        if i % 10 == 0 && i >= config.totalStep / 100 
            if config.curr == config.norm # the last diagram is for normalization
                config.normalization += 1.0 / config.reweight[config.norm]
            else
                measure(config)
            end
        end
        if i % 1000 == 0
            for t in timer
                check(t, config, config.neighbor, config.var)
            end
        if doReweight && i > 1000_00 && i % 1000_00 == 0
                reweight(config)
        end
        end
    end

    # if (print >= 0)
    #     # printStatus(config)
    #     printstyled("Seed $(config.seed) End Simulation. Cost $(time() - startTime) seconds.\n\n", color=:red)
    # end

    return config
end

function reweight(config)
    avgstep = sum(config.visited) / length(config.visited)
    for (vi, v) in enumerate(config.visited)
        if v > 1000
            config.reweight[vi] *= avgstep / v
end
    end
end

"""
    progressBar(step, total)

Return string of progressBar (step/total*100%)
"""
function progressBar(step, total)
    barWidth = 70
    percent = round(step / total * 100.0, digits=2)
            str = "["
    pos = barWidth * percent / 100.0
    for i = 1:barWidth
        if i <= pos
            str *= "█"
        else
            str *= " "
        end
    end
    str *= "] $step/$total=$percent%"
    return str
end

function printSummary(summary, neighbor, var)

    steps, totalSteps, visited, reweight, propose, accept = summary.step, summary.totalStep, summary.visited, summary.reweight, summary.propose, summary.accept
    Nd = length(visited)

    barbar = "===============================  Report   ==========================================="
    bar = "-------------------------------------------------------------------------------------"

    println(barbar)
    println(green(Dates.now()))
    println("\nTotalStep:", totalSteps)
    println(bar)

    totalproposed = 0.0
    println(yellow(@sprintf("%-20s %12s %12s %12s", "ChangeIntegrand", "Proposed", "Accepted", "Ratio  ")))
    for n in neighbor[Nd]
        @printf(
            "Norm -> %2d:           %11.6f%% %11.6f%% %12.6f\n",
            n,
            propose[1, Nd, n] / steps * 100.0,
            accept[1, Nd, n] / steps * 100.0,
            accept[1, Nd, n] / propose[1, Nd, n]
        )
        totalproposed += propose[1, Nd, n]
    end
    for idx in 1:Nd - 1
        for n in neighbor[idx]
            if n == Nd  # normalization diagram
                @printf("  %d ->Norm:           %11.6f%% %11.6f%% %12.6f\n",
                    idx,
                    propose[1, idx, n] / steps * 100.0,
                    accept[1, idx, n] / steps * 100.0,
                    accept[1, idx, n] / propose[1, idx, n]
                )
            else
                @printf("  %d -> %2d:            %11.6f%% %11.6f%% %12.6f\n",
                    idx, n,
                    propose[1, idx, n] / steps * 100.0,
                    accept[1, idx, n] / steps * 100.0,
                    accept[1, idx, n] / propose[1, idx, n]
                )
            end
            totalproposed += propose[1, idx, n]
        end
    end
    println(bar)

    println(yellow(@sprintf("%-20s %12s %12s %12s", "ChangeVariable", "Proposed", "Accepted", "Ratio  ")))
    for idx in 1:Nd - 1 # normalization diagram don't have variable to change
        for (vi, var) in enumerate(var)
            typestr = "$(typeof(var))"
            typestr = split(typestr, ".")[end]
            @printf(
                "  %2d / %-10s:   %11.6f%% %11.6f%% %12.6f\n",
                idx, typestr,
                propose[2, idx, vi] / steps * 100.0,
                accept[2, idx, vi] / steps * 100.0,
                accept[2, idx, vi] / propose[2, idx, vi]
            )
            totalproposed += propose[2, idx, vi]
        end
    end
    println(bar)
    println(yellow("Diagrams            Visited      ReWeight\n"))
    @printf("  Norm   :     %12i %12.6f\n", visited[end], reweight[end])
    for idx in 1:Nd - 1
        @printf("  Order%2d:     %12i %12.6f\n", idx, visited[idx], reweight[idx])
    end
    println(bar)
    println(yellow("Total Proposed: $(totalproposed / steps * 100.0)%\n"))
    println(green(progressBar(steps, totalSteps)))
    println()

end

end
