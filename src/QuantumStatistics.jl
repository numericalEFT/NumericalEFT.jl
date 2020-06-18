module QuantumStatistics
export Green, Grid, FastMath, Yeppp, func
import StaticArrays
# greet() = print("Hello World!")
include("green.jl")
include("grid.jl")
include("fastmath.jl")
include("Yeppp.jl")

"""
    func(x)
return double
"""
func(x)=2x+1
end # module
