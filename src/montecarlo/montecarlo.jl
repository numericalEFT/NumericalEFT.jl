"""
Monte Carlo Calculator for Diagrams
"""
module MonteCarlo
using Random
using LinearAlgebra
using StaticArrays, Printf, Dates
using ..Utility
const RNG = Random.GLOBAL_RNG

include("variable.jl")
include("sampler.jl")
include("updates.jl")

function montecarlo(block::Int, diagrams, T::Variable, K::Variable, Ext::External, integrand::Function, measure::Function; pid=nothing, rng=GLOBAL_RNG, timer=nothing, blockStep=1000_000)
    ##############  initialization  ################################
    if (pid === nothing)
        r = Random.RandomDevice()
        pid = abs(rand(r, Int)) % 1000000
    end
    @assert pid >= 0 "pid should be positive!"
    Random.seed!(rng, pid) # pid will be used as the seed to initialize the random numebr generator

    @assert block > 0 "block number should be positive!"
    @assert length(diagrams) > 0 "diagrams should not be empty!"

    config = Configuration(pid, block, blockStep, diagrams, T, K, Ext, rng)
    # don't forget to initialize the diagram weight
    config.absWeight =
        integrand(config.curr, config.X, config.K, config.ext, config.step)

    if timer === nothing
        printTime = 10
        timer = [StopWatch(printTime, printStatus)]
    end

    updates = [increaseOrder, decreaseOrder, changeX, changeK, changeExt]

    for diag in config.diagrams
        diag.propose = zeros(Float64, length(updates)) .+ 1.0e-8
        # add a small number so that the ratio propose/accept=0 if there is no such update proposed and accepted
        diag.accept = zeros(Float64, length(updates)) 
    end

    ########### MC simulation ##################################
    printstyled("PID $pid Start Simulation ...\n", color=:red)
    startTime = time()

    for blk = 0:block
        for i = 1:blockStep
            config.step += 1
            config.curr.visitedSteps += 1
            _update = rand(config.rng, updates) # randomly select an update
            _update(config, integrand)
            (i % 10 == 0 && blk >= 1) && measure(config.curr, config.X, config.K, config.ext, config.step, blk)
            if i % 1000 == 0
                for t in timer
                    check(t, config)
                end
            end
        end
        (blk >= 1) && reweight(config)
    end

    printStatus(config)
    printstyled("PID $pid End Simulation. Cost $(time() - startTime) seconds.\n\n", color=:red)
end

mutable struct Configuration{TX,TK,R}
    pid::Int
    totalBlock::Int
    blockStep::Int
    diagrams::Vector{Diagram}
    X::TX
    K::TK
    ext::External

    step::Int64
    curr::Diagram
    rng::R
    absWeight::Float64

    function Configuration(_pid, _totalBlock, _blockStep, _diagrams, _varX::TX, _varK::TK, _ext, rng::R) where {TX,TK,R}
        curr = _diagrams[1]
        config = new{TX,TK,R}(_pid, _totalBlock, _blockStep, collect(_diagrams), _varX, _varK, _ext, 0, curr, rng, 0.0)
        return config
    end
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

function printStatus(config)
    barbar = "====================================================================================="
    bar = "-------------------------------------------------------------------------------------"

    println(barbar)
    printstyled(Dates.now(), color=:green)
    println("\nStep:", config.step)
    println(bar)

    name = ["increaseOrder", "decreaseOrder", "changeX", "changeK", "changeExt"]
    totalproposed = 0.0

    for num = 1:length(name)
        @printf(
            "%-14s %12s %12s %12s\n",
            String(name[num]),
            "Proposed",
            "Accepted",
            "Ratio  "
        )
        for (idx, diag) in enumerate(config.diagrams)
            @printf(
                "  Order%2d:     %11.6f%% %11.6f%% %12.6f\n",
                diag.id,
                diag.propose[num] / config.step * 100.0,
                diag.accept[num] / config.step * 100.0,
                diag.accept[num] / diag.propose[num]
            )
            totalproposed += diag.propose[num]
        end
        println(bar)
    end
    printstyled("Diagrams            Visited      ReWeight\n", color=:yellow)
    for (idx, diag) in enumerate(config.diagrams)
        @printf(
                "  Order%2d:     %12i %12.6f\n",
                diag.id,
                diag.visitedSteps,
                diag.reWeightFactor
            )
    end
    println(bar)
    printstyled("Total Proposed: $(totalproposed / config.step * 100.0)%\n", color=:yellow)
    # printstyled(progressBar(round(config.step / config.blockStep, digits=2), config.totalBlock + 1), color=:green)
    printstyled(progressBar(round(config.step / config.blockStep, digits=2), config.totalBlock + 1), color=:green)
    println()
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

end
