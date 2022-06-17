using CompositeGrids, Test, StaticArrays, LinearAlgebra, Printf, Random, Statistics, JLD2, FileIO
# import Test: @test, @testset

if isempty(ARGS)
    include("chebyshev.jl")
    include("grid.jl")
    include("interpolate.jl")
    include("io.jl")
else
    include(ARGS[1])
end
