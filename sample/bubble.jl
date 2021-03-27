using QuantumStatistics

kF, β = 1.0, 25.0
function eval1(configuration)
    return 1.0
end

function eval2(configuration)
    return 1.0
end

K = MonteCarlo.FermiK(3, kF, 0.5 * kF, 10.0 * kF)
T = MonteCarlo.Tau(β, β / 2.0)
Ext = MonteCarlo.External([1]) #external variable is specified
group1 = MonteCarlo.Group(1, [0, 1], zeros(Float64, Ext.size...), eval1)
group2 = MonteCarlo.Group(2, [1, 2], zeros(Float64, Ext.size...), eval2)
config = MonteCarlo.Configuration(10, (group1, group2), (K, T), Ext; pid = 1)

println("eval: ", group1.eval(config))

println(K[1])
println(MonteCarlo.create!(K, 1))
println(MonteCarlo.remove(K, 1))
println(K[1])

MonteCarlo.montecarlo(config)