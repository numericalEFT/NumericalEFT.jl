using Test, StaticArrays, LinearAlgebra
using NumericalEFT
# import Test: @test, @testset

include("math.jl")

##### Lehmann   #########
using Base: BufferStream
using Test
using Printf
using JLD2
using CodecZlib #it is important to import CodecZlib explicitly. Otherwise, the code may try to dynamically load this package, and sometimes leads to error

include("Lehmann/spectral.jl")
include("Lehmann/dlr.jl")


#####  MCIntegration  #######

@testset "MCIntegration.jl" begin
    # Write your tests here.
    include("MCIntegration/montecarlo.jl")
end

########## FeynmanDiagram ########
using Test, LinearAlgebra, Random, StaticArrays, Printf, Parameters, Documenter
using AbstractTrees

include("FeynmanDiagram/common.jl")
include("FeynmanDiagram/diagram_tree.jl")
include("FeynmanDiagram/expression_tree.jl")
include("FeynmanDiagram/parquet_builder.jl")


######### CompositeGrids ##################
using StaticArrays, LinearAlgebra, Printf, Random, Statistics, JLD2, FileIO

if isempty(ARGS)
    include("CompositeGrids/chebyshev.jl")
    include("CompositeGrids/grid.jl")
    # include("CompositeGrids/interpolate.jl")
    # include("CompositeGrids/io.jl")
else
    include(ARGS[1])
end

########### GreenFunc #######################
using Test, StaticArrays, LinearAlgebra, Printf, Statistics
using JLD2, FileIO
using CodecZlib

if isempty(ARGS)
    include("GreenFunc/test_Green.jl")
    #include("interpolate.jl")
else
    include(ARGS[1])
end

########## Atom #####################
# include("Atom/hilbert.jl")
# include("Atom/green.jl")
# include("Atom/hubbard.jl")