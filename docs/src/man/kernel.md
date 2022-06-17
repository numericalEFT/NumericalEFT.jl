# Convention

The kernel in the Lehammn representation is a function that only depends on the statistics of the quantum particles, and the symmetry of the Green's function. It is universal in the sense that it doesn't depend on the microscopic details of the quantum many-body system. 

The definition of the kernel is not unique. Here we give the defintion in this package.

We use the following conventions:
- Temperature $T$ .
- Inverse temperature $\beta= 1/T$.
- Real frequency $\omega$.
- Imaginary time $\tau$.
- Matsubara frequancy $i\omega_n$. 
    
    For the fermonic case, $\omega_n = (2n+1)\pi T$. 

    For the bosonic case,  $\omega_n = 2n\pi T$
- Fermionic Green's function is antiperiodic $G(\tau)=-G(\beta+\tau)$. 

    Bosonic one is periodic $G(\tau)=G(\beta+\tau)$. 
    
    Don't confuse the periodicity with the time-reversal symmetry (a.k.a, particle-hole symmetry). 

- Fourier transform follows the convention in the book "Quantum Many-particle Systems" by J. Negele and H. Orland, Page 95,

```math
G(\tau) = \frac{1}{\beta} \sum_n G(i\omega_n) \text{e}^{-i\omega_n \tau}
```

```math
G(i\omega_n) = \int_0^\beta G(\tau) \text{e}^{i\omega_n \tau} d\tau
```

# Fermion without Symmetry 

- Imaginary time 
```math
K(τ, ω) = \frac{e^{-ωτ}}{1+e^{-ωβ}}
```
- Matusbara frequency 
```math
K(iω_n, ω) = -\frac{1}{iω_n-ω},
```


# Boson without Symmetry 

We use a bosonic kernel with a regularator near $\omega =$. The imaginary-time kernel happens to be the same as the fermionic kernel. The details can be found in Appendix A of this [DLR paper](https://arxiv.org/pdf/2107.13094.pdf). 

- Imaginary time 
```math
K(τ, ω) = \frac{e^{-ωτ}}{1+e^{-ωβ}}
```
- Matusbara frequency 
```math
K(iω_n, ω) = -\frac{1}{iω_n-ω}\frac{1-e^{-ωβ}}{1+e^{-ωβ}},
```

# Fermion with the Particle-hole Symmetry 

Particle-hole symmetry means the time reversal symmetry, so that $G(\tau)=G(\beta-\tau)$.

- Imaginary time
```math
K(τ, ω) = e^{-ω|τ|}+e^{-ω(β-|τ|)}
```
- Matusbara frequency
```math
K(iω_n, ω) = \frac{2iω_n}{ω^2+ω_n^2}(1+e^{-ωβ}),
```

# Boson with the Particle-hole Symmetry 

Particle-hole symmetry means the time reversal symmetry, so that $G(\tau)=G(\beta-\tau)$.

- Imaginary time
```math
K(τ, ω) = e^{-ω|τ|}+e^{-ω(β-|τ|)}
```
- Matusbara frequency
```math
K(iω_n, ω) = \frac{2ω}{ω^2+ω_n^2}(1-e^{-ωβ}),
```

# Fermion with the Particle-hole Anti-Symmetry 

Particle-hole antisymmetry means the time reversal symmetry, so that $G(\tau)=-G(\beta-\tau)$.

- Imaginary time
```math
K(τ, ω) = e^{-ω|τ|}-e^{-ω(β-|τ|)}
```
- Matusbara frequency
```math
K(iω_n, ω) = \frac{2ω}{ω^2+ω_n^2}(1+e^{-ωβ}),
```

# Boson with the Particle-hole Anti-Symmetry 

Particle-hole antisymmetry means the time reversal symmetry, so that $G(\tau)=-G(\beta-\tau)$.

- Imaginary time
```math
K(τ, ω) = e^{-ω|τ|}-e^{-ω(β-|τ|)}
```
- Matusbara frequency
```math
K(iω_n, ω) = \frac{2iω_n}{ω^2+ω_n^2}(1-e^{-ωβ}),
```
