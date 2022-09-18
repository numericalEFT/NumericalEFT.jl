module CompositeGrids
using StaticArrays

include("old/grid.jl")
export Grid

include("grid/chebyshev.jl")
export BaryChebTools

include("grid/simple.jl")
const SimpleGrid = SimpleG # alias for older convention
const AbstractGrid, OpenGrid, ClosedGrid = SimpleGrid.AbstractGrid, SimpleGrid.OpenGrid, SimpleGrid.ClosedGrid
export SimpleG, SimpleGrid, AbstractGrid, OpenGrid, ClosedGrid, denseindex

include("grid/composite.jl")
const CompositeGrid = CompositeG
export CompositeG, CompositeGrid

include("grid/interpolate.jl")
export Interp

end # module
