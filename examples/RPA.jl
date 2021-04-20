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
    # instantaneous interaction (Coulomb interaction)
    vq = [4π * e^2 / (q^2 + 0.001) for q in qgrid]
    EF = kF^2 / (2mass)
    dlr = DLR.DLRGrid(:corr, 10EF, β, 1e-10) # effective interaction is a correlation function of the form <O(τ)O(0)>
    Nq, Nτ = length(qgrid), length(τgrid)
    Π = zeros(Complex{Float64}, (Nq, dlr.size))
    dW0norm = similar(Π)
    for (ni, n) in enumerate(dlr.n)
        for (qi, q) in enumerate(qgrid)
            iωn = 2π * n / β * im
            Π[qi, ni] = Diagram.bubble(3, q, iωn, kF, β, mass)[1] .* spin
        end
        dW0norm[:, ni] = @. vq * Π[:, ni] / (1 - vq * Π[:, ni])
        println("ω_n=2π/β*$(n), Π(q=0, n=0)=$(Π[1, ni])")
    end
    println(Π[10, :])
    for (qi, q) in enumerate(qgrid)
        println("$qi  $q    $(dW0norm[qi, 1])")
    end
    for (ni, n) in enumerate(dlr.n)
        println("$ni  $n   $(dW0norm[10, ni])")
    end
    dW0norm = DLR.matfreq2tau(:corr, dW0norm, dlr, τgrid, axis=2)
    return vq, real.(dW0norm)
end

if abspath(PROGRAM_FILE) == @__FILE__
    using Gaston
    kF, β, spin = 1.919, 25.0, 2
    m, e = 0.5, sqrt(2.0) # Rydberg units
    EF = kF^2 / (2m)
    β = β / EF

    qgrid = Grid.boseK(kF, 3kF, 0.2kF, 32) 
    τgrid = Grid.tau(β, EF, 128)
    println("qGrid: ", qgrid.grid)
    println("τGrid: ", τgrid.grid)

    vq, dW0norm = dWRPA(qgrid.grid, τgrid.grid, kF, β, spin, m, e)
    println(dW0norm[1, :])
    display(plot(qgrid.grid ./ kF, dW0norm[:, 1]))
    sleep(100)
    # plot(qgrid.grid ./ kF, dW0norm[:, 1])
    # show()
#     plot(qgrid.grid ./ kF, dW0norm[:, 1])
# show()
end