# We work with Rydberg units, length scale Bohr radius a_0, energy scale: Ry

###### constants ###########
const e0 = sqrt(2)  # electric charge
const me = 0.5  # electron mass
const dim = 3    # dimension (D=2 or 3, doesn't work for other D!!!)
const spin = 2  # number of spins

const rs = 1.0   
const kF = (dim == 3) ? (9π / (2spin))^(1 / 3) / rs : sqrt(4 / spin) / rs
const EF = kF^2 / (2me)
const β = 25.0 / kF^2
# const Nf = (D==3) ? 