using Base: BufferStream
using Lehmann
using Test
using Printf
using JLD2
using CodecZlib #it is important to import CodecZlib explicitly. Otherwise, the code may try to dynamically load this package, and sometimes leads to error

include("spectral.jl")
include("dlr.jl")

# @testset "Lehmann.jl" begin
# Write your tests here.
# end
