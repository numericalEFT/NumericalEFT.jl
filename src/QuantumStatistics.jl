module QuantumStatistics
export Green, Grid, FastMath, Diagram, Interpolate, Spectral
import StaticArrays
# greet() = print("Hello World!")
include("green.jl")
include("spectral.jl")
include("diagram.jl")
include("grid.jl")
include("interpolate.jl")
include("fastmath.jl")

end # module
