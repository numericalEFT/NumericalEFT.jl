module GreenFunc
using StaticArrays
using ..Lehmann
using ..CompositeGrids


include("green/Green.jl")
export TimeDomain, ImTime, ReTime, ImFreq, ReFreq, DLRFreq
export Green2DLR, toTau, toMatFreq, toDLR, dynamic, instant

end
