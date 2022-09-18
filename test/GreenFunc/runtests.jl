using GreenFunc
using Test, StaticArrays, LinearAlgebra, Printf, Statistics, Lehmann, CompositeGrids
using JLD2, FileIO
using CodecZlib

if isempty(ARGS)
    #include("test_Green.jl")
    include("test_Green_sym.jl")
    #include("interpolate.jl")
else
    include(ARGS[1])
end
