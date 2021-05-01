using QuantumStatistics
include("parameter.jl")

chantype = [:T, :U, :S]
F = [2, 3]
V = [1, 2]
Full = [1, 2, 3]
bubble = Dict([1 => (F, Full), 2 => (F, Full), 3 => (V, Full)])

para = Parquet.Para(chantype, bubble, [1, 2])
println(para)

ver4 = Parquet.Ver4{Weight}(2, 1, para)
println(ver4)
