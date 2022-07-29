using Test, StaticArrays, LinearAlgebra
using NumericalEFT
# import Test: @test, @testset

##### Lehmann   #########
using Base: BufferStream
using Printf
using JLD2
using CodecZlib #it is important to import CodecZlib explicitly. Otherwise, the code may try to dynamically load this package, and sometimes leads to error

########## FeynmanDiagram ########
using Random, Parameters
using AbstractTrees

######### CompositeGrids ##################
using Statistics, FileIO

if isempty(ARGS)
    include("math.jl")

    include("Lehmann/spectral.jl")
    include("Lehmann/dlr.jl")

    @testset "MCIntegration.jl" begin
        # Write your tests here.
        include("MCIntegration/montecarlo.jl")
    end

    include("FeynmanDiagram/common.jl")
    include("FeynmanDiagram/diagram_tree.jl")
    include("FeynmanDiagram/expression_tree.jl")
    include("FeynmanDiagram/parquet_builder.jl")

    include("CompositeGrids/chebyshev.jl")
    include("CompositeGrids/grid.jl")
    include("CompositeGrids/interpolate.jl")
    include("CompositeGrids/io.jl")


    include("GreenFunc/test_Green.jl")
    #include("interpolate.jl")

    include("Renormalization/renorm.jl")
    #include("interpolate.jl")
else
    include(ARGS[1])
end

########## Atom #####################
# include("Atom/hilbert.jl")
# include("Atom/green.jl")
# include("Atom/hubbard.jl")