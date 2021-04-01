using QuantumStatistics

β = 1000.0
Euv = 10.0
dlrGrid = Basis.dlrGrid(:fermi, Euv, β, 1.0e-12)
# println(dlrGrid)
println(dlrGrid[:τ])
println("ω: ", dlrGrid[:ω])

G = zeros(Float64, (2, length(dlrGrid[:τ])))

G[1, :] = [TwoPoint.fermiT(τ, 0.1, β) for τ in dlrGrid[:τ]]
G[2, :] = [TwoPoint.fermiT(τ, 1.0, β) for τ in dlrGrid[:τ]]
# println("G: ", G)
dlrcoeff = Basis.tau2dlr(:fermi, G, dlrGrid, β; axis=2, eps=1.0e-16)
# println(size(dlrcoeff))
Gp = Basis.dlr2tau(:fermi, dlrcoeff, dlrGrid, β; axis=2)

println("G1")
for i in 1:length(G[1, :])
    println("$(G[1, i])  $(Gp[1, i])   $(G[1, i] - Gp[1, i])")
end

println("G2")
for i in 1:length(G[1, :])
    println("$(G[2, i])  $(Gp[2, i])   $(G[2, i] - Gp[2, i])")
end
println("End")

# println(G[:, 2])
# for i in 1:length(G[:, 2])
#     println("$(G[i, 2])  $(Gp[i, 2])   $(G[i, 2] - Gp[i, 2])")
# end
# println(G[:, 2])
# println(Gp[:, 2])

# kernel, green = Basis.tau2dlr(:fermi, G, dlrGrid, β; axis=1, eps=1.0e-12)

# using PyCall
# linalg = pyimport("scipy.linalg")
# lu, piv = linalg.lu_factor(kernel)
# # println(lu, piv)
# coeff = linalg.lu_solve((lu, piv), G)
# println("coeff ", coeff)
# for i in 1:length(coeff)
#     println("$(dlrcoeff[i])  $(coeff[i])   $(dlrcoeff[i] - coeff[i])")
# end



# Gp = kernel * coeff
# for i in 1:length(G)
#     println("$(G[i])  $(Gp[i])   $(G[i] - Gp[i])")
# end

