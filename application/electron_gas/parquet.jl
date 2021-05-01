using QuantumStatistics
include("parameter.jl")

chan = [Parquet.T, Parquet.U, Parquet.S]

para = Parquet.Para(chan, [1, 2])

ver4 = Parquet.Ver4{Weight}(2, 1, para)
Parquet.showTree(ver4, para, verbose=1, depth=3)
# println(ver4)
