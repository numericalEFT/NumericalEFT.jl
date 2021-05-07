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

sample(config::Configuration, integrand::Function, measure::Function; Nblock=16, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

 sample the integrands, collect statistics, and return the expected values and errors.

 # Remarks
 - User may run the MC in parallel using MPI. Simply run `mpiexec -n N julia userscript.jl` where `N` is the number of workers. In this mode, only the root process returns meaningful results. All other workers return `nothing, nothing`. User is responsible to handle the returning results properly.
 - In the MC, a normalization diagram is introduced to normalize the MC estimates of the integrands. More information can be found in the link: https://kunyuan.github.io/QuantumStatistics.jl/dev/man/important_sampling/#Important-Sampling. User don't need to explicitly specify this normalization diagram.Internally, normalization diagram will be added to each table that is related to the integrands.

 # Arguments

 - `config`: Configuration struct

 - `integrand`: function call to evaluate the integrand. It should accept an argument of the type `Configuration`, and return a weight. 
    Internally, MC only samples the absolute value of the weight. Therefore, it is also important to define Main.abs for the weight if its type is user-defined. 

- `measure`: function call to measure. It should accept an argument of the type `Configuration`, then manipulate observables `obs`. 

- `Nblock`: repeat times. The tasks will automatically distributed to multi-process in MPI mode.

- `print`: -1 to not print anything, 0 to print minimal information, >0 to print summary for every `print` seconds

- `printio`: `io` to print the information

- `save`: -1 to not save anything, 0 to save observables `obs` in the end of sampling, >0 to save observables `obs` for every `save` seconds

- `saveio`: `io` to save
"""
function sample(config::Configuration, integrand::Function, measure::Function; Nblock=16, print=0, printio=stdout, save=0, saveio=nothing, timer=[])


    ############ initialized timer ####################################
    if print > 0
        push!(timer, StopWatch(print, printSummary))
    end

    ########### initialized MPI #######################################
    (MPI.Initialized() == false ) && MPI.Init()
    comm = MPI.COMM_WORLD
    Nworker = MPI.Comm_size(comm)  # number of MPI workers
    rank = MPI.Comm_rank(comm)  # rank of current MPI worker
    root = 0 # rank of the root worker
    # MPI.Barrier(comm)

    #########  construct configurations for each block ################
    if Nblock > Nworker
        Nblock = (Nblock ÷ Nworker) * Nworker # make Nblock % size ==0, error estimation assumes this relation
    else
        Nblock = Nworker  # each worker should handle at least one block
    end
    @assert Nblock % Nworker == 0

    obsSum, obsSquaredSum = zero(config.observable), zero(config.observable)
    summary = nothing
    startTime = time()

    for i in 1:Nblock
        # MPI thread rank will run the block with the indexes: rank, rank+Nworker, rank+2Nworker, ...
        (i % Nworker != rank) && continue

        reset!(config, config.reweight) # reset configuration, keep the previous reweight factors

        config = montecarlo(config, integrand, measure, print, save, timer, true)

        summary = addStat(config, summary)  # collect MC information

        if typeof(obsSum) <: AbstractArray
            obsSum .+= config.observable ./ config.normalization
            obsSquaredSum .+= (config.observable ./ config.normalization).^2
        else
            obsSum += config.observable / config.normalization
            obsSquaredSum += (config.observable / config.normalization)^2
        end
    end

    #################### collect statistics  ####################################
    if typeof(obsSum) <: AbstractArray
        MPI.Reduce!(obsSum, MPI.SUM, root, comm) # root node gets the sum of observables from all blocks
        MPI.Reduce!(obsSquaredSum, MPI.SUM, root, comm) # root node gets the squared sum of observables from all blocks
    else
        result = [obsSum, obsSquaredSum]  # MPI.Reduce works for array only
        MPI.Reduce!(result, MPI.SUM, root, comm) # root node gets the sum of observables from all blocks
        obsSum, obsSquaredSum = result
    end
    summary = reduceStat(summary, root, comm) # root node gets the summed MC information

    if MPI.Comm_rank(comm) == root
        ################################ IO ######################################
        if (print >= 0)
            printSummary(summary, config.neighbor, config.var)
        end
        println(red("All simulation ended. Cost $(time() - startTime) seconds."))
        ##################### Extract Statistics  ################################
        mean = obsSum ./ Nblock
        std = @. sqrt((obsSquaredSum / Nblock - mean^2) / (Nblock - 1))
        # MPI.Finalize()
        return mean, std
    else # if not the root, return nothing
        # MPI.Finalize()
        return nothing, nothing
    end
end

function montecarlo(config::Configuration, integrand::Function, measure::Function, print, save, timer, doReweight)
    ##############  initialization  ################################
    # don't forget to initialize the diagram weight
    config.absWeight = abs(integrand(config))
    
    updates = [changeIntegrand, changeVariable] # TODO: sample changeVariable more often
    for i in 2:length(config.var)
        push!(updates, changeVariable)
    end

    ########### MC simulation ##################################
    if (print >= 0)
        println(green("Seed $(config.seed) Start Simulation ..."))
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

    if (print >= 0)
        # printStatus(config)
        println(green("Seed $(config.seed) End Simulation. Cost $(time() - startTime) seconds."))
    end

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
