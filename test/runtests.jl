using Test, StaticArrays, LinearAlgebra
# import Test: @test, @testset

if isempty(ARGS)
    include("math.jl")
else
    include(ARGS[1])
end
