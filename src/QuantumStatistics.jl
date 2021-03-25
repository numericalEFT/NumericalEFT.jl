module QuantumStatistics
export Green, Grid, FastMath, Diagram, Interpolate, Spectral, MonteCarlo, Utility
import StaticArrays
# greet() = print("Hello World!")
include("green.jl")
include("spectral.jl")
include("diagram.jl")
include("grid.jl")
include("interpolate.jl")
include("fastmath.jl")
include("montecarlo.jl")
include("utility.jl")

end # module
