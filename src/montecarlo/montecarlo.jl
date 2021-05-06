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

"""

 sample(totalStep, var, dof, obs, integrand::Function, measure::Function, normalize::Function=nothing; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, reweight=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

 sample the integrands, collect statistics, and return the expected values and errors

 # Arguments

 - `totalStep`: total number of updates
 
 - `var`: TUPLE of variables, each variable should be derived from the abstract type Variable, see variable.jl for details). Use a tuple rather than a vector improves the performance.

 - `dof`: degrees of freedom of each integrand, e.g., ([0, 1], [2, 3]) means the first integrand has zero var#1 and one var#2; while the second integrand has two var#1 and 3 var#2. 

 - `obs`: observables that is required to calculate the integrands, will be used in the `measure` function call

 - `integrand`: function call to evaluate the integrand. It should accept an argument of the type `Configuration`, and return a weight. 
    Internally, MC only samples the absolute value of the weight. Therefore, it is also important to define Main.abs for the weight if its type is user-defined. 

- `measure`: function call to measure. It should accept an argument of the type `Configuration`, then manipulate observables `obs`. 

- `normalize`: function call to derive averages and errors from observables `obs`. It should accept an argument of the type `Configuration`.

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
function sample(totalStep, var, dof, obs, integrand::Function, measure::Function; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, reweight=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

    ################# diagram initialization #########################
    Nd = length(dof) # number of integrands
    @assert Nd > 0 "At least one integrand is required."

    if isnothing(neighbor)
        # By default, only the order-1 and order+1 diagrams are considered to be the neighbors
        # Nd+1 is the normalization diagram, by default, it only connects to the diagram with index 1
        neighbor = []
        for di in 1:Nd + 1
            if di == 1 # 1 to norm and 2
                push!(neighbor, [Nd + 1, 2]) 
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
        config = Configuration(seedi, steps, var, para, neighbor, dof, obscopied, reweight)
        push!(configList, config)
    end

    #################### distribute MC tasks  ##############################
    mymap = isdefined(Main, :pmap) ? Main.pmap : map # if Distributed module is imported, then use pmap for parallelization
    config = @sync mymap((c) -> montecarlo(c, integrand, measure, print, save, timer, doReweight), configList)
    @assert length(config) == Nblock # make sure all tasks returns

    ##################### Extract Statistics  ################################
    observable = [c.observable / c.normalization for c in config]
    avg = sum(observable) / Nblock

    # println(observable[1])
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
                check(t, [config, ])
            end
            if doReweight && i > 1000_00 && i % 1000_00 == 0
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
    for n in configList[1].neighbor[Nd]
        @printf(
            "Norm -> %2d:            %11.6f%% %11.6f%% %12.6f\n",
            n,
            propose[1, Nd, n] / steps * 100.0,
            accept[1, Nd, n] / steps * 100.0,
            accept[1, Nd, n] / propose[1, Nd, n]
        )
        totalproposed += propose[1, Nd, n]
    end
    for idx in 1:Nd - 1
        for n in configList[1].neighbor[idx]
            if n == Nd  # normalization diagram
                @printf("  %d ->Norm:            %11.6f%% %11.6f%% %12.6f\n",
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

    @printf("%-20s %12s %12s %12s\n", "ChangeVariable", "Proposed", "Accepted", "Ratio  ")
    for idx in 1:Nd - 1 # normalization diagram don't have variable to change
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
    @printf("  Norm    :     %12i %12.6f\n", visited[end], configList[1].reweight[end])
    for idx in 1:Nd - 1
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
