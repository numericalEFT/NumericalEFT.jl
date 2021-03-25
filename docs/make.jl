push!(LOAD_PATH, "../src/")
using Documenter, QuantumStatistics

makedocs(modules = [QuantumStatistics], sitename = "QuantumStatistics.jl")

deploydocs(repo = "github.com/kunyuan/QuantumStatistics.jl.git")
