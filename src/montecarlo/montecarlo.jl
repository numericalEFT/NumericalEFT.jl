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

function sample(totalStep, var, dof, obs, integrand::Function, measure::Function, normalize::Function=nothing; Nblock=16, para=nothing, neighbor=nothing, seed=nothing, print=0, printio=stdout, save=0, saveio=nothing, timer=[])

    ################# diagram initialization #########################
    @assert length(dof) > 1 "At least two integrands are needed. One to calcualte, one for normalization."
    # initialize diagrams with the given degrees of freedom
    # we call them diagrams of order 1, 2, 3 ...
    diagrams = [Diagram(d) for d in dof] 

    if isnothing(neighbor)
        # By default, only the order-1 and order+1 diagrams are considered to be the neighbors
        neighbor = []
        for (di, diag) in enumerate(diagrams)
            if di == 1
                push!(neighbor, [di + 1,])
            elseif di == length(diagrams)
                push!(neighbor, [di - 1,])
            else
                push!(neighbor, [di - 1, di + 1])
            end
        end
    end

    #########  construct configurations for each block ################
    steps = totalStep ÷ Nblock
    configList = []
    for i in 1:Nblock
        if isnothing(seed)
            pid = rand(Random.RandomDevice(), 1:1000000)
        else
            pid = i + abs(seed)
        end
        obscopied = deepcopy(obs) # copied observables for each block 
        config = Configuration(pid, steps, diagrams, neighbor, var, obscopied, para, MersenneTwister(pid))
        push!(configList, config)
    end

    #################### distribute MC tasks  ##############################
    mymap = isdefined(Main, :pmap) ? Main.pmap : map # if Distributed module is imported, then use pmap for parallelization
    config = mymap((c) -> montecarlo(c, integrand, measure; print=print, printio=printio, save=save, saveio=saveio, timer=timer), configList)
    # mymap((c) -> montecarlo(c, integrand, measure; print=print, printio=printio, save=save, saveio=saveio, timer=timer), configList)
    # @distributed 
    # config = configList
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

function montecarlo(config::Configuration, integrand::Function, measure::Function; print=0, printio=stdout, save=0, saveio=nothing, timer=[])
    ##############  initialization  ################################
    # don't forget to initialize the diagram weight
    config.absWeight = abs(integrand(config))
    
    if print > 0
        push!(timer, StopWatch(print, printSummary))
    end

    updates = [changeDiagram, changeVar] # TODO: sample changeVar more often
    # for var in config.var
    #     # changeVar should be call more often if there are more variables
        #     push!(updates, changeVar)
    # end

    for diag in config.diagrams
        diag.proposeDiag = zeros(Float64, length(config.diagrams)) .+ 1.0e-8
        # add a small number so that the ratio propose/accept=0 if there is no such update proposed and accepted
        diag.acceptDiag = zeros(Float64, length(config.diagrams)) 

        diag.proposeVar = zeros(Float64, length(config.var)) .+ 1.0e-8
        # add a small number so that the ratio propose/accept=0 if there is no such update proposed and accepted
        diag.acceptVar = zeros(Float64, length(config.var)) 
    end

    ########### MC simulation ##################################
    if (print >= 0)
        printstyled("PID $(config.pid) Start Simulation ...\n", color=:red)
    end
    startTime = time()
        
    for i = 1:config.totalStep
        config.step += 1
        config.diagrams[config.curr].visitedSteps += 1
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
        printstyled("PID $(config.pid) End Simulation. Cost $(time() - startTime) seconds.\n\n", color=:red)
    end

    return config
end

function reweight(config)
    # config.diagrams[1].reWeightFactor = 1.0
    # config.diagrams[2].reWeightFactor = 8.0
    avgstep = sum([g.visitedSteps for g in config.diagrams]) / length(config.diagrams)
    for g in config.diagrams
        if g.visitedSteps > 10000
            # g.reWeightFactor=g.reWeightFactor*0.5+totalstep/g.visitedSteps*0.5
            g.reWeightFactor *= avgstep / g.visitedSteps
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
    diags = deepcopy(configList[1].diagrams)

    for (di, d) in enumerate(diags)
        for i in 2:length(configList)
            d.visitedSteps += configList[i].diagrams[di].visitedSteps
            d.proposeDiag .+= configList[i].diagrams[di].proposeDiag
            d.acceptDiag .+= configList[i].diagrams[di].acceptDiag
            d.proposeVar .+= configList[i].diagrams[di].proposeVar
            d.acceptVar .+= configList[i].diagrams[di].acceptVar
        end
    end
    return steps, totalSteps, diags
end

function printSummary(configList)
    @assert length(configList) > 0

    steps, totalSteps, diags = collectStatus(configList)

    barbar = "===============================  Report   ==========================================="
    bar = "-------------------------------------------------------------------------------------"

    println(barbar)
    printstyled(Dates.now(), color=:green)
    println("\nTotalStep:", totalSteps)
    println(bar)

    # name = ["increaseOrder", "decreaseOrder"]
    # for (vi, var) in enumerate(configList[1].var)
    #     # typeof(Var) is something like QuantumStatistics.MonteCarlo.Tau, only the last block is the type name
    #     typestr = "$(typeof(var))"
    #     typestr = split(typestr, ".")[end]
    #     append!(name, ["change_$typestr", ])
    # end
    totalproposed = 0.0

    @printf("%-20s %12s %12s %12s\n", "ChangeDiagrams", "Proposed", "Accepted", "Ratio  ")
    for (idx, diag) in enumerate(diags)
        for n in configList[1].neighbor[idx]
            @printf(
                "  %d -> %2d:            %11.6f%% %11.6f%% %12.6f\n",
                idx, n,
                diag.proposeDiag[n] / steps * 100.0,
                diag.acceptDiag[n] / steps * 100.0,
                diag.acceptDiag[n] / diag.proposeDiag[n]
            )
            totalproposed += diag.proposeDiag[n]
        end
    end
    println(bar)

    @printf("%-20s %12s %12s %12s\n", "ChangeVariable", "Proposed", "Accepted", "Ratio  ")
    for (idx, diag) in enumerate(diags)
        for (vi, var) in enumerate(configList[1].var)
            typestr = "$(typeof(var))"
            typestr = split(typestr, ".")[end]
            @printf(
                "  %2d / %-10s:   %11.6f%% %11.6f%% %12.6f\n",
                idx, typestr,
                diag.proposeVar[vi] / steps * 100.0,
                diag.acceptVar[vi] / steps * 100.0,
                diag.acceptVar[vi] / diag.proposeVar[vi]
            )
            totalproposed += diag.proposeVar[vi]
        end
    end
    println(bar)

    #     typestr = "$(typeof(var))"
    #     typestr = split(typestr, ".")[end]
    #     append!(name, ["change_$typestr", ])

    # @printf(
    #     "%-20s %12s %12s %12s\n",
    #     String("changeVar"),
    #     "Proposed",
    #     "Accepted",
    #     "Ratio  "
    # )
    # for (idx, diag) in enumerate(diags)
    #     for n in configList[1].neighbor[idx]
    #         @printf(
    #             "  %d -> %2d:            %11.6f%% %11.6f%% %12.6f\n",
    #             idx, n,
    #             diag.proposeDiag[n] / steps * 100.0,
    #             diag.acceptDiag[n] / steps * 100.0,
    #             diag.acceptDiag[n] / diag.proposeDiag[n]
    #         )
    #         totalproposed += diag.propose[num]
    #     end
    # end
    # println(bar)
    printstyled("Diagrams            Visited      ReWeight\n", color=:yellow)
    for (idx, diag) in enumerate(diags)
        @printf(
                "  Order%2d:     %12i %12.6f\n",
                idx,
                diag.visitedSteps,
                diag.reWeightFactor
            )
    end
    println(bar)
    printstyled("Total Proposed: $(totalproposed / steps * 100.0)%\n", color=:yellow)
    if length(configList) == 1 
        printstyled(progressBar(steps, totalSteps), color=:green)
    end
    # printstyled(progressBar(round(config.step / config.blockStep, digits=2), config.totalBlock + 1), color=:green)
    println()

end

end
