# This example demonstrated how to calculate one-loop diagram of free electrons using the Monte Carlo module
# Observable is normalized: Γ₄*N_F where N_F is the free electron density of states

using Distributed
using QuantumStatistics, LinearAlgebra, Random, Printf, StaticArrays, BenchmarkTools, InteractiveUtils, Parameters

const Ncpu = 16 # number of workers (CPU)
const totalStep = 1e8 # MC steps of each worker

addprocs(Ncpu) 

@everywhere using QuantumStatistics, Parameters, StaticArrays, Random, LinearAlgebra
@everywhere include("parameter.jl")

# claim all globals to be constant, otherwise, global variables could impact the efficiency
########################### parameters ##################################
@everywhere const IsF = false # calculate quasiparticle interaction F or not
@everywhere const AngSize = 16

########################## variables for MC integration ##################
@everywhere const Weight = SVector{2,Float64}
@everywhere const Base.abs(w::Weight) = abs(w[1]) + abs(w[2]) # define abs(Weight)
@everywhere const KInL = [kF, 0.0, 0.0] # incoming momentum of the left particle
@everywhere const Qd = [0.0, 0.0, 0.0] # transfer momentum is zero in the forward scattering channel

################ construct RPA interaction ################################
@everywhere const qgrid = Grid.boseK(kF, 6kF, 0.2kF, 256) 
@everywhere const τgrid = Grid.tau(β, EF / 20, 128)

include("RPA.jl") # dW0 will be only calculated once in the master, then distributed to other workers. Therefore, there is no need to import RPA.jl for all workers.

@everywhere struct Para
    extAngle::Vector{Float64}
    dW0::Matrix{Float64}
    function Para(AngSize)
        extAngle = collect(LinRange(0.0, π, AngSize)) # external angle grid
        vqinv = [(q^2 + mass2) / (4π * e0^2) for q in qgrid.grid]
        dW0 = dWRPA(vqinv, qgrid.grid, τgrid.grid, kF, β, spin, me) # dynamic part of the effective interaction
        # println(size(extAngle))
        # println(size(dW0))
        return new(extAngle, dW0)
    end
end

@everywhere function interaction(config, qd, qe, τIn, τOut)
    dW0 = config.para.dW0

    dτ = abs(τOut - τIn)

    kDiQ = sqrt(dot(qd, qd))
    vd = 4π * e0^2 / (kDiQ^2 + mass2)
    if kDiQ <= qgrid.grid[1]
        wd = vd * Grid.linear2D(dW0, qgrid, τgrid, qgrid.grid[1] + 1.0e-6, dτ) # the current interpolation vanishes at q=0, which needs to be corrected!
    else
        wd = vd * Grid.linear2D(dW0, qgrid, τgrid, kDiQ, dτ) # dynamic interaction, don't forget the singular factor vq
    end

    kExQ = sqrt(dot(qe, qe))
    ve = 4π * e0^2 / (kExQ^2 + mass2)
    if kExQ <= qgrid.grid[1]
        we = ve * Grid.linear2D(dW0, qgrid, τgrid, qgrid.grid[1] + 1.0e-6, dτ) # dynamic interaction, don't forget the singular factor vq
    else
        we = ve * Grid.linear2D(dW0, qgrid, τgrid, kExQ, dτ) # dynamic interaction, don't forget the singular factor vq
    end

    return -vd / β, ve / β, -wd, we
end

@everywhere function phase(tInL, tOutL, tInR, tOutR)
    if (IsF)
        return cos(π * ((tInL + tOutL) - (tInR + tOutR)));
    else
        return cos(π * ((tInL - tOutL) + (tInR - tOutR)))
    end
end

@everywhere function integrand(config)
    if config.curr == 1
        return Weight(1.0, 0.0) # return a weight!
    elseif config.curr == 2
        return eval2(config)
    else
        error("Not implemented!")
    end
end

@everywhere function eval2(config)
    T, K, Ang = config.var[1], config.var[2], config.var[3]
    k1, k2 = K[1], K[1] - Qd
    t1, t2 = T[1], T[2] # t1, t2 both have two tau variables
    θ = config.para.extAngle[Ang[1]] # angle of the external momentum on the right
    KInR = [kF * cos(θ), kF * sin(θ), 0.0]

    vld, vle, wld, wle = interaction(config, Qd, KInL - k1, t1[1], t1[2])
    vrd, vre, wrd, wre = interaction(config, Qd, KInR - k2, t2[1], t2[2])

    ϵ1, ϵ2 = (dot(k1, k1) - kF^2) / (2me), (dot(k2, k2) - kF^2) / (2me) 
    wd, we = 0.0, 0.0
    # possible green's functions on the top
    gt1 = Spectral.kernelFermiT(t2[1] - t1[1], ϵ1, β)


    # gt2 = Spectral.kernelFermiT(t1[1] - t2[1], ϵ2, β)
    # wd += 1.0 / β * 1.0 / β * gt1 * gt2 / (2π)^3 * phase(t1[1], t1[1], t2[1], t2[1])

    # gt3 = Spectral.kernelFermiT(t1[1] - t2[2], ϵ2, β)
    # G = gt1 * gt3 / (2π)^3 * phase(t1[1], t1[1], t2[2], t2[1])
    # wd += G * (vld * wre)

    # wd += spin * (vld + wld) * (vrd + wrd) * gt1 * gt2 / (2π)^3 * phase(t1[1], t1[1], t2[1], t2[1])
    # println(vld, ", ", wld, "; ", vrd, ", ", wld)

    ############## Diagram v x v ######################
    """
      KInL                      KInR
       |                         | 
  t1.L ↑     t1.L       t2.L     ↑ t2.L
       |-------------->----------|
       |       |    k1    |      |
       |   ve  |          |  ve  |
       |       |    k2    |      |
       |--------------<----------|
  t1.L ↑    t1.L        t2.L     ↑ t2.L
       |                         | 
      KInL                      KInR
"""
    gd1 = Spectral.kernelFermiT(t1[1] - t2[1], ϵ2, β)
    G = gt1 * gd1 / (2π)^3 * phase(t1[1], t1[1], t2[1], t2[1])
    we += G * (vle * vre)
    ##################################################

    ############## Diagram w x v ######################
    """
      KInL                      KInR
       |                         | 
  t1.R ↑     t1.L       t2.L     ↑ t2.L
       |-------------->----------|
       |       |    k1    |      |
       |   we  |          |  ve  |
       |       |    k2    |      |
       |--------------<----------|
  t1.L ↑    t1.R        t2.L     ↑ t2.L
       |                         | 
      KInL                      KInR
    """
    gd2 = Spectral.kernelFermiT(t1[2] - t2[1], ϵ2, β)
    G = gt1 * gd2 / (2π)^3 * phase(t1[1], t1[2], t2[1], t2[1])
    we += G * (wle * vre) 
    ##################################################

    ############## Diagram v x w ######################
    """
      KInL                      KInR
       |                         | 
  t1.L ↑     t1.L       t2.L     ↑ t2.L
       |-------------->----------|
       |       |    k1    |      |
       |   ve  |          |  we  |
       |       |    k2    |      |
       |--------------<----------|
  t1.L ↑    t1.L        t2.R     ↑ t2.R
       |                         | 
      KInL                      KInR
    """
    gd3 = Spectral.kernelFermiT(t1[1] - t2[2], ϵ2, β)
    G = gt1 * gd3 / (2π)^3 * phase(t1[1], t1[1], t2[2], t2[1])
    we += G * (vle * wre)
    ##################################################

    ############## Diagram w x w ######################
    """
      KInL                      KInR
       |                         | 
  t1.R ↑     t1.L       t2.L     ↑ t2.L
       |-------------->----------|
       |       |    k1    |      |
       |   we  |          |  we  |
       |       |    k2    |      |
       |--------------<----------|
  t1.L ↑    t1.R        t2.R     ↑ t2.R
       |                         | 
      KInL                      KInR
"""
    gd4 = Spectral.kernelFermiT(t1[2] - t2[2], ϵ2, β)
    G = gt1 * gd4 / (2π)^3 * phase(t1[1], t1[2], t2[2], t2[1])
    we += G * (wle * wre)
    ##################################################

    return Weight(wd, we)
end

@everywhere function measure(config)
    angidx = config.var[3][1]
    factor = 1.0 / config.reweight[config.curr]
    if config.curr == 1
        config.observable[1][:, angidx] .+= factor
    elseif config.curr == 2
        weight = integrand(config)
        config.observable[2][:, angidx] .+= weight / abs(weight) * factor
    else
        error("Not implemented!")
    end
end

@everywhere normalize(config) = config.observable[2] / sum(config.observable[1]) * AngSize * β 

function run(totalStep)
    T = MonteCarlo.TauPair(β, β / 2.0)
    K = MonteCarlo.FermiK(3, kF, 0.2 * kF, 10.0 * kF)
    Ext = MonteCarlo.Discrete(1, AngSize) # external variable is specified

    dof = ([1, 0, 1], [2, 1, 1])
    obs = (zeros(Float64, (2, AngSize)), zeros(Float64, (2, AngSize)))

    para = Para(AngSize)

    avg, std = MonteCarlo.sample(totalStep, (T, K, Ext), dof, obs, integrand, measure, normalize; para=para, print=10)

    NF = TwoPoint.LindhardΩnFiniteTemperature(dim, 0.0, 0, kF, β, me, spin)[1]
    println("NF = $NF")

    avg = avg * NF
    std = std * NF

    println(size(avg))
    for (idx, angle) in enumerate(para.extAngle)
        @printf("%10.6f   %10.6f ± %10.6f  %10.6f ± %10.6f\n", angle, avg[1, idx], std[1,idx], avg[2,idx], std[2,idx])
    end
end

# @btime run(1, 10)
run(totalStep)
# @time run(Repeat, totalStep)