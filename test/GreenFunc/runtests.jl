using GreenFunc
using Test, StaticArrays, LinearAlgebra, Printf, Statistics, Lehmann, CompositeGrids
using JLD2, FileIO
using CodecZlib

if isempty(ARGS)
    # include("test_Green.jl") #deprecated, not going to test anymore
    include("test_MeshProduct.jl")
    include("test_MeshArrays.jl")
    include("test_transform.jl")
    include("test_MeshGrids.jl")
    include("test_Triqs.jl")
else
    include(ARGS[1])
end
