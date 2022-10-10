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

function check(mean, error, expect, ratio=7.0)
    # println(mean, error)
    for ei in eachindex(expect)
        @test abs(mean[ei] - expect[ei]) < error[ei] * ratio
    end
end

function check(result::Result, expect, ratio=7.0)
    # println(result)
    mean, error = result.mean, result.stdev
    check(mean, error, expect, ratio)
end

function check_complex(result::Result, expect, ratio=7.0)
    mean, error = result.mean, result.stdev
    # println(mean, error)
    check(real(mean), real(error), real(expect), ratio)
    check(imag(mean), imag(error), imag(expect), ratio)
end

if isempty(ARGS)
    include("math.jl")

    include("Lehmann/spectral.jl")
    include("Lehmann/dlr.jl")

    # Write your tests here.
    include("MCIntegration/utility.jl")
    include("MCIntegration/montecarlo.jl")
    include("MCIntegration/thread.jl")
    include("MCIntegration/bubble.jl")
    include("MCIntegration/bubble_FermiK.jl")

    include("FeynmanDiagram/common.jl")
    include("FeynmanDiagram/diagram_tree.jl")
    include("FeynmanDiagram/expression_tree.jl")
    include("FeynmanDiagram/parquet_builder.jl")

    include("CompositeGrids/chebyshev.jl")
    include("CompositeGrids/grid.jl")
    include("CompositeGrids/interpolate.jl")
    include("CompositeGrids/io.jl")


    # include("GreenFunc/test_Green.jl")
    #include("interpolate.jl")
    include("GreenFunc/test_MeshProduct.jl")
    include("GreenFunc/test_MeshArrays.jl")
    include("GreenFunc/test_transform.jl")
    include("GreenFunc/test_MeshGrids.jl")
    include("GreenFunc/test_Triqs.jl")

    # include("Renormalization/renorm.jl")
    #include("interpolate.jl")

    # let
    #     using BrillouinZoneMeshes.AbstractTrees
    #     using BrillouinZoneMeshes.GridTree
    #     using BrillouinZoneMeshes.BaseMesh
    #     using BrillouinZoneMeshes.BaryCheb
    #     include("BrillouinZoneMeshes/tree.jl")
    #     include("BrillouinZoneMeshes/basemesh.jl")
    #     include("BrillouinZoneMeshes/barycheb.jl")
    #     # include("BrillouinZoneMeshes/mc.jl") #locate and volume is not properly handled in BZmeshes
    # end
else
    include(ARGS[1])
end

module _Test_BrillouinZoneMeshes

using NumericalEFT
using BrillouinZoneMeshes.AbstractTrees
using BrillouinZoneMeshes.GridTree
using BrillouinZoneMeshes.BaseMesh
using BrillouinZoneMeshes.BaryCheb
# using BrillouinZoneMeshes.TreeMeshes
# using BrillouinZoneMeshes.BaseMesh
# using BrillouinZoneMeshes.BaryCheb
# using BrillouinZoneMeshes.SymMaps
using LinearAlgebra, Random
using Test
@testset "BrillouinZoneMeshes.jl" begin
    if isempty(ARGS)
        # include("barycheb.jl")
        # include("BaseMesh.jl")
        # include("TreeMeshes.jl")
        # include("mc.jl")
        include("BrillouinZoneMeshes/tree.jl")
        include("BrillouinZoneMeshes/basemesh.jl")
        include("BrillouinZoneMeshes/barycheb.jl")
        # include("BrillouinZoneMeshes/mc.jl") #locate and volume is not properly handled in BZmeshes
    else
        include(ARGS[1])
    end
end

end

########## Atom #####################
# include("Atom/hilbert.jl")
# include("Atom/green.jl")
# include("Atom/hubbard.jl")