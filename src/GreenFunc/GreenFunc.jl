module GreenFunc
using StaticArrays
using ..Lehmann
using ..CompositeGrids
using ..BrillouinZoneMeshes
# Write your package code here.

include("meshgrids/MeshGrids.jl")
using .MeshGrids
export MeshGrids
export locate, volume
export FERMION, BOSON
export TemporalGrid
export MeshProduct
export DLRFreq, ImTime, ImFreq

# include("green/Green.jl")
# #export TimeDomain, ImTime, ReTime, ImFreq, ReFreq, DLRFreq
# export Green2DLR, toTau, toMatFreq, toDLR


# include("green/GreenSym.jl")
# export GreenSym2DLR, dynamic, instant

# include("green/meshgrids/MeshProduct.jl")
# export MeshProduct
# export locate, volume

include("mesharrays/MeshArrays.jl")
using .MeshArrays
export MeshArrays
export MeshArray, MeshMatrix, MeshVector
export int_to_matfreq, matfreq_to_int, matfreq

include("triqs/Triqs.jl")
using .Triqs
export Triqs
export from_triqs

include("green/transform.jl")
export dlr_to_imfreq, dlr_to_imtime
export imfreq_to_dlr, imtime_to_dlr, to_dlr, to_imtime, to_imfreq

include("green/testcase.jl")

include("deprecated/Deprecated.jl")
export Deprecated

end
