using BrillouinZoneMeshes
using BrillouinZoneMeshes.AbstractTrees, BrillouinZoneMeshes.GridTree, BrillouinZoneMeshes.BaseMesh, BrillouinZoneMeshes.BaryCheb
using LinearAlgebra, Random
using Test

@testset "BrillouinZoneMeshes.jl" begin


    if isempty(ARGS)
        include("tree.jl")
        include("basemesh.jl")
        include("barycheb.jl")
        include("mc.jl")
    else
        include(ARGS[1])
    end
end

