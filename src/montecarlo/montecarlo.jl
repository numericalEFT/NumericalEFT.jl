"""
Monte Carlo Calculator for Diagrams
"""
module MonteCarlo

export montecarlo, Configuration, Diagram, FermiK, BoseK, Tau, TauPair

using Random
using LinearAlgebra
using StaticArrays, Printf, Dates
using ..Utility
const RNG = Random.GLOBAL_RNG

include("variable.jl")
include("sampler.jl")
include("updates.jl")

function sample(totalStep, var, dof, obs, integrand::Function, measure::Function, normalize::Function=nothing; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, reWeight=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

    ################# diagram initialization #########################
    Nd = length(dof) # number of integrands
    @assert Nd > 1 "At least two integrands are required. One to calcualte, one for normalization."

    if isnothing(neighbor)
        # By default, only the order-1 and order+1 diagrams are considered to be the neighbors
        neighbor = []
        for di in 1:Nd
            if di == 1
                push!(neighbor, [di + 1,])
            elseif di == Nd
                push!(neighbor, [di - 1,])
            else
                push!(neighbor, [di - 1, di + 1])
            end
        end
    end

    ############# initialize reweight factors ########################
    if isnothing(reWeight)
        reWeight = [1.0 for d in 1:Nd]
    end

    ############ initialized timer ####################################
    if print > 0
        push!(timer, StopWatch(print, printSummary))
    end


    #########  construct configurations for each block ################
    steps = totalStep ÷ Nblock
    configList = []
    for i in 1:Nblock
        if isnothing(seed)
            seedi = rand(Random.RandomDevice(), 1:1000000)
        else
            seedi = i + abs(seed)
        end
        obscopied = deepcopy(obs) # copied observables for each block 
        config = Configuration(seedi, steps, var, para, neighbor, dof, obscopied, reWeight)
        push!(configList, config)
    end

    #################### distribute MC tasks  ##############################
    mymap = isdefined(Main, :pmap) ? Main.pmap : map # if Distributed module is imported, then use pmap for parallelization
    config = @sync mymap((c) -> montecarlo(c, integrand, measure, print, save, timer), configList)
    @assert length(config) == Nblock # make sure all tasks returns

    ##################### Extract Statistics  ################################
    observable = [normalize(c) for c in config]
    avg = sum(observable) / Nblock

    if Nblock > 1
        err = sqrt.(sum([(obs .- avg).^2 for obs in observable]) / (Nblock - 1)) / sqrt(Nblock)
    else
        err = abs.(avg)
    end

    ################################ IO ######################################
    if (print >= 0)
        printSummary(config)
    end
    return avg, err
end

function montecarlo(config::Configuration, integrand::Function, measure::Function, print, save, timer)
    ##############  initialization  ################################
    # don't forget to initialize the diagram weight
    config.absWeight = abs(integrand(config))
    
    updates = [changeIntegrand, changeVariable] # TODO: sample changeVariable more often

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
        (i % 10 == 0 && i >= config.totalStep / 100) && measure(config)
        if i % 1000 == 0
            for t in timer
                check(t, [config, ])
            end
            if i > 1000_00 && i % 1000_00 == 0
                reweight(config)
        end
        end
    end

    if (print >= 0)
        # printStatus(config)
        printstyled("Seed $(config.seed) End Simulation. Cost $(time() - startTime) seconds.\n\n", color=:red)
    end

    return config
end

function reweight(config)
    # config.diagrams[1].reWeightFactor = 1.0
    # config.diagrams[2].reWeightFactor = 8.0
    avgstep = sum(config.visited) / length(config.visited)
    for (vi, v) in enumerate(config.visited)
        if v > 10000
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

function collectStatus(configList)
    @assert length(configList) > 0
    steps = sum([config.step for config in configList])
    totalSteps = sum([config.totalStep for config in configList])
    visited = sum([config.visited for config in configList])
    propose = sum([config.propose for config in configList])
    accept = sum([config.accept for config in configList])
    return steps, totalSteps, visited, propose, accept
end

function printSummary(configList)
    @assert length(configList) > 0

    steps, totalSteps, visited, propose, accept = collectStatus(configList)
    Nd = length(visited)

    barbar = "===============================  Report   ==========================================="
    bar = "-------------------------------------------------------------------------------------"

    println(barbar)
    printstyled(Dates.now(), color=:green)
    println("\nTotalStep:", totalSteps)
    println(bar)

    totalproposed = 0.0
    @printf("%-20s %12s %12s %12s\n", "ChangeIntegrand", "Proposed", "Accepted", "Ratio  ")
    for idx in 1:Nd
        for n in configList[1].neighbor[idx]
            @printf(
                "  %d -> %2d:            %11.6f%% %11.6f%% %12.6f\n",
                idx, n,
                propose[1, idx, n] / steps * 100.0,
                accept[1, idx, n] / steps * 100.0,
                accept[1, idx, n] / propose[1, idx, n]
            )
            totalproposed += propose[1, idx, n]
        end
    end
    println(bar)

    @printf("%-20s %12s %12s %12s\n", "ChangeVariable", "Proposed", "Accepted", "Ratio  ")
    for idx in 1:Nd
        for (vi, var) in enumerate(configList[1].var)
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
    printstyled("Diagrams            Visited      ReWeight\n", color=:yellow)
    for idx in 1:Nd
        @printf("  Order%2d:     %12i %12.6f\n", idx, visited[idx], configList[1].reweight[idx])
    end
    println(bar)
    printstyled("Total Proposed: $(totalproposed / steps * 100.0)%\n", color=:yellow)
    if length(configList) == 1 
        printstyled(progressBar(steps, totalSteps), color=:green)
    end
    println()

end

end
