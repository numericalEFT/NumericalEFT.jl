"""
utility for Monte Carlo
"""
module MonteCarlo
using Random
using LinearAlgebra
using StaticArrays, Printf, Dates, NamedArrays
const RNG = Random.GLOBAL_RNG

include("variable.jl")
include("configuration.jl")
include("sampler.jl")
include("updates.jl")

function montecarlo(
    config,
    timer = nothing,
    updates = nothing
)

    timer, updates=initialize(config, timer, updates)
    println("Start Simulation ...")

    # printTimer = StopWatch(PrintTime, Markov.printStatus)
    # saveTimer = StopWatch(SaveTime, Markov.save)
    # reweightTimer = StopWatch(ReWeightTime, Markov.reweight)
    # messageTimer = StopWatch(MessageTime, Markov.save)

    for block = 1:config.totalBlock
        for i = 1:1000_000
            config.step += 1
            config.curr.visitedSteps += 1
            _update = rand(config.rng, updates) #randomly select an update
            _update(config)
            i % 10 == 0 && measure(config)
            if i % 1000 == 0
                # println(config.var[1][1], ", ", config.var[2][1], ", ", config.var[2][2])
                for t in timer
                    check(t, config)
                end
            end
        end
        reweight(config)
    end

    printStatus(config)
    println("End Simulation. ")
end

function initialize(config, timer, updates)
    if timer==nothing
        printTime=10
        timer=[StopWatch(printTime, printStatus), ]
    end

    if updates==nothing
        updates=[increaseOrder, decreaseOrder, changeInternal]
    end
    for update in updates
        for group in config.groups
            group.propose[Symbol(update)]=1.0e-10
            group.accept[Symbol(update)]=1.0e-10
        end
    end
    return timer, updates
end

function reweight(config)
    # config.groups[1].reWeightFactor=1.0
    # config.groups[2].reWeightFactor=8.0
    avgstep=sum([g.visitedSteps for g in config.groups])/length(config.groups)
    for g in config.groups
        if g.visitedSteps>10000
            # g.reWeightFactor=g.reWeightFactor*0.5+totalstep/g.visitedSteps*0.5
            g.reWeightFactor *=avgstep/g.visitedSteps
        end
    end
end

function measure(config)
    curr = config.curr
    factor = 1.0 / curr.absWeight / curr.reWeightFactor
    weight = curr.eval(config)
    curr.observable[config.ext.idx...] += weight * factor
end

const barbar = "====================================================================================="
const bar = "-------------------------------------------------------------------------------------"

function printStatus(config)
    # Var.counter += 1
    println(barbar)
    printstyled(Dates.now(), color = :green)
    println("\nStep:", config.step)
    println(bar)

    for (update, val) in config.groups[1].propose
        @printf("%-14s %12s %12s %12s\n", String(update), "Proposed", "Accepted", "Ratio  ")
        for (idx, group) in enumerate(config.groups)
            @printf(
                "  Order%2d:     %12.8f %12.8f %12.6f\n",
                group.id,
                group.propose[update]/config.step,
                group.accept[update]/config.step,
                group.accept[update] / group.propose[update]
            )
        end
        println(bar)
    end
    println(progressBar(round(config.step / 1000_000, digits = 2), config.totalBlock))
    println()
end

"""
    StopWatch(start, interval, callback)

Initialize a stopwatch. 

# Arguments
- `start::Float64`: initial time (in seconds)
- `interval::Float64` : interval to click (in seconds)
- `callback` : callback function after each click (interval seconds)
"""
mutable struct StopWatch
    start::Float64
    interval::Float64
    f::Function
    StopWatch(_interval, callback) = new(time(), _interval, callback)
end

"""
    check(stopwatch, parameter...)

Check stopwatch. If it clicks, call the callback function with the unpacked parameter
"""
function check(watch::StopWatch, parameter...)
    now = time()
    if now - watch.start > watch.interval
        watch.f(parameter...)
        watch.start = now
    end
end

"""
    progressBar(step, total)

Return string of progressBar (step/total*100%)
"""
function progressBar(step, total)
    barWidth = 70
    percent = round(step / total * 100.0, digits = 2)
    str = "["
    pos = barWidth * percent / 100.0
    for i = 1:barWidth
        if i <= pos
            str *= "I"
        else
            str *= " "
        end
    end
    str *= "] $step/$total=$percent%"
    return str
end

end
