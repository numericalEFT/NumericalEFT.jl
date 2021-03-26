"""
utility for Monte Carlo
"""
module MonteCarlo
using Random
using LinearAlgebra
const RNG = Random.GLOBAL_RNG

include("configuration.jl")
include("sampler.jl")

function montecarlo!(totalblock, config, timer=nothing, updates=nothing, savefile=nothing)
    println("Start Simulation ...")
    block = 0

    # printTimer = StopWatch(PrintTime, Markov.printStatus)
    # saveTimer = StopWatch(SaveTime, Markov.save)
    # reweightTimer = StopWatch(ReWeightTime, Markov.reweight)
    # messageTimer = StopWatch(MessageTime, Markov.save)

    for block = 1:totalblock
        for i = 1:1000_000
            config.step += 1
            _update = rand(config.rng, updates) #randomly select an update
            _update(config)
            i % 10 == 0 && measure(config)
            if i % 1000 == 0
                for t in timer
                    check(t, config)
                end
            end
        end
    end

    # printStatus()
    println("End Simulation. ")
end

# const barbar = "====================================================================================="
# const bar = "-------------------------------------------------------------------------------------"

# function printStatus(state, updates)
#     # Var.counter += 1
#     println(barbar)
#     printstyled(Dates.now(), color = :green)
#     println("\nStep:", state.step)
#     println(bar)
#     for i = 1:UpdateNum
#         @printf("%-14s %12s %12s %12s\n", Name[i], "Proposed", "Accepted", "Ratio  ")
#         for o = 0:Order
#             @printf(
#                 "  Order%2d:     %12.0f %12.0f %12.6f\n",
#                 o,
#                 Proposed[i, o + 1],
#                 Accepted[i, o + 1],
#                 Accepted[i, o + 1] / Proposed[i, o + 1]
#             )
#         end
#         println(bar)
#     end
#     println(progressBar(round(curr.step / 1000_000, digits = 2), TotalBlock))
#     println()
# end

end
