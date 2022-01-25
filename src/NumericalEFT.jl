module NumericalEFT
using StaticArrays
include("common.jl")
export σx, σy, σz, σ0

include("fastmath.jl")
export FastMath

include("utility/utility.jl")
export Utility

##########################  Compact Lehmann representation  ########################################
using Lehmann
# main module
export Lehmann
# submodules
export Spectral, Sample
export DLRGrid
export tau2dlr, dlr2tau, matfreq2dlr, dlr2matfreq, tau2matfreq, matfreq2tau, tau2tau, matfreq2matfreq

##########################  Monte Carlo Integration  ###############################################
using MCIntegration
# main module
export MCIntegration
export montecarlo, Configuration, Continuous, Discrete

##########################  CompositeGrids  #######################################################
using CompositeGrids
# main module
export CompositeGrids
# submodules
export Grid, Interp
export SimpleG, SimpleGrid, AbstractGrid, OpenGrid, ClosedGrid, denseindex
export CompositeG, CompositeGrid

##########################  Green's function  #######################################################
using GreenFunc
# main module
export GreenFunc
export TimeDomain, ImTime, ReTime, ImFreq, ReFreq, DLRFreq
export Green2DLR, toTau, toMatFreq, toDLR, dynamic, instant

##########################  Diagram Builder  #######################################################
using FeynmanDiagram
# main module
export FeynmanDiagram
export SigmaDiag, PolarDiag, Ver3Diag, Ver4Diag, GreenDiag
export Wirreducible, Girreducible, NoBubble, NoHatree, NoFock, Proper
export Response, ChargeCharge, SpinSpin, UpUp, UpDown
export AnalyticProperty, Instant, Dynamic
export GenericPara, Interaction

# submodules
export DiagTree
export TwoBodyChannel, Alli, PHr, PHEr, PPr, AnyChan
export Permutation, Di, Ex, DiEx
export Diagram, addSubDiagram!, toDataFrame
export evalDiagNode!, evalDiagTree!
export Operator, Sum, Prod
export DiagramId, GenericId, Ver4Id, Ver3Id, GreenId, SigmaId, PolarId, InteractionId
export uidreset, toDataFrame, mergeby, plot_tree

# submodules
export Parquet
export ParquetBlocks

# submodules
export ExprTree
export Component, Diagrams
export addpropagator!, addnode!
export setroot!, addroot!
export evalNaive, showTree


end # module
