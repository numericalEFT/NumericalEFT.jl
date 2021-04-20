# Random phase approxiamtion for electron gas 

using QuantumStatistics

"""
3D Imaginary-time effective interaction derived from random phase approximation. 
Return ``dW_0(q, τ)/v_q`` where ``v_q`` is the bare Coulomb interaction and ``dW_0`` is the dynamic part of the effective interaction. 

The total effective interaction can be recoverd using,  
```math
W_0(q, τ) = v_q δ(τ) + dW_0(q, τ).
```

The dynamic contribution is the fourier transform of,
```math
dW_0(q, iω_n)=v_q^2 Π(q, iω_n)/(1-v_q Π(q, iω_n))
```
Note that this dynamic contribution ``dW_0'' diverges at small q. For this reason, this function returns ``dW_0/v_q``
"""
function dWRPA(qgrid, τgrid, kF, β, spin, mass, e)
    @assert all(qgrid .!= 0.0)
    vq = [4π * e^2 / (q^2) for q in qgrid] # instantaneous interaction (Coulomb interaction)
    EF = kF^2 / (2mass)
    dlr = DLR.DLRGrid(:corr, 10EF, β, 1e-10) # effective interaction is a correlation function of the form <O(τ)O(0)>
    Nq, Nτ = length(qgrid), length(τgrid)
    Π = zeros(Complex{Float64}, (Nq, dlr.size)) # Matsubara grid is the optimized sparse DLR grid 
    dW0norm = similar(Π)
    for (ni, n) in enumerate(dlr.n)
        for (qi, q) in enumerate(qgrid)
            Π[qi, ni] = TwoPoint.LindhardΩnFiniteTemperature(3, q, n, kF, β, mass, 2)[1]
        end
        dW0norm[:, ni] = @. vq * Π[:, ni] / (1 - vq * Π[:, ni])
        println("ω_n=2π/β*$(n), Π(q=0, n=0)=$(Π[1, ni])")
    end
    dW0norm = DLR.matfreq2tau(:corr, dW0norm, dlr, τgrid, axis=2) # dW0/vq in imaginary-time representation, real-valued but in complex format
    return vq, real.(dW0norm) 
end

if abspath(PROGRAM_FILE) == @__FILE__
    using Gaston
    kF, β, spin = 1.919, 100.0, 2
    m, e = 0.5, sqrt(2.0) # Rydberg units
    EF = kF^2 / (2m)
    β = β / EF

    qgrid = Grid.boseK(kF, 3kF, 0.2kF, 32) 
    τgrid = Grid.tau(β, EF, 128)
    # println("qGrid: ", qgrid.grid)
    # println("τGrid: ", τgrid.grid)

    vq, dW0norm = dWRPA(qgrid.grid, τgrid.grid, kF, β, spin, m, e)
    display(plot(qgrid.grid ./ kF, dW0norm[:, 1]))
    sleep(100)
    # plot(qgrid.grid ./ kF, dW0norm[:, 1])
    # show()
#     plot(qgrid.grid ./ kF, dW0norm[:, 1])
# show()
end