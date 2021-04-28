# We work with Rydberg units, length scale Bohr radius a_0, energy scale: Ry
using StaticArrays

###### constants ###########
const e0 = sqrt(2)  # electric charge
const me = 0.5  # electron mass
const dim = 3    # dimension (D=2 or 3, doesn't work for other D!!!)
const spin = 2  # number of spins

const rs = 1.0  
const kF = (dim == 3) ? (9π / (2spin))^(1 / 3) / rs : sqrt(4 / spin) / rs
const EF = kF^2 / (2me)
const β = 100.0 / kF^2
const mass2 = 0.01

const Weight = SVector{2,Float64}
const Base.abs(w::Weight) = abs(w[1]) + abs(w[2]) # define abs(Weight)
const INL, OUTL, INR, OUTR = 1, 2, 3, 4
# const Nf = (D==3) ? 

println("rs=$rs, β=$β, kF=$kF, EF=$EF, mass2=$mass2")