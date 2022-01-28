# NumericalEFT

Numerical effective field theory toolbox for quantum many-body problem.

[![Stable](https://img.shields.io/badge/docs-stable-blue.svg)](https://numericalEFT.github.io/NumericalEFT.jl/)
[![Dev](https://img.shields.io/badge/docs-dev-blue.svg)](https://numericalEFT.github.io/NumericalEFT.jl/)
[![Build Status](https://github.com/numericalEFT/NumericalEFT.jl/workflows/CI/badge.svg)](https://github.com/numericalEFT/NumericalEFT.jl/actions)
[![codecov](https://codecov.io/gh/numericalEFT/NumericalEFT.jl/branch/master/graph/badge.svg?token=OKnDPEC3In)](https://codecov.io/gh/numericalEFT/NumericalEFT.jl)

## Motivation

Collective modes of quantum many-body systems are often well described by weakly-interacting quantum fields. Effective field theory (EFT) is a framework to systematically study such problems. EFT has been widely used in high-energy physics, nuclear physics and condensed matter physics. 

For quantum material applications, EFTs are often too complicated to calculate with bare hands. Therefore, we are motivated to create a numerical package for such problems.

Potential applications of this package are the electron liquids and superconductors in real materials, chiral EFT in neutron-rich matter, and all kinds of emergent low-energy field theories in lattice models.

## Features

The package ``NumericalEFT.jl`` is a collection of loosely coupled components, which are organized in the following infrastructure: 

![NumericalEFT](docs/src/assets/numericalEFT.svg)

Most of the components have been published as independent packages, so that user has the freedom to try them separately. The package is in development stage. Many components are still missing. Here we list the components that are ready for applications:

- [Lehmann.jl](https://github.com/numericalEFT/Lehmann.jl): Discrete Lehmann representation (DLR) for imaginary-time/Matsubara frequency Green's function. For a generic Green's function at a temperature T, DLR is capable of representing it up to a given accuracy ϵ with a cost ~ log(1/T)log(1/ϵ), signicantly cheaper than a naive approach with a cost ~ 1/(Tϵ). 

- [FeynmanDiagram.jl](https://github.com/numericalEFT/FeynmanDiagram.jl): High-order Feynman diagram builder for general quantum many-body systems. The diagrams are compiled into an expression tree for subsequent efficient evaluations using a MC integrator. It Supports propagator/interaction renormalization, which is important for renormalization group analysis. 

- [MCIntegration.jl](https://github.com/numericalEFT/MCIntegration.jl): An adapative Monte Carlo calculator for general high dimensional integral. It is particularly suitable to calculate multiple integrals that are strongly correlated, for example, the Feynman diagrams. 

- [CompositeGrids.jl](https://github.com/numericalEFT/CompositeGrids.jl): Composite Cheybeshev/Gaussian/logarithmic grid systems for one-dimensional interpolation and integration. It allows the user to combine different grids to represent one highly non-smooth functions.

- [GreenFunc.jl](https://github.com/numericalEFT/GreenFunc.jl): A container for generic Green's functions.

<!-- - Fast elementary math functions. Some of them are adapted from the package [Yeppp.jl](https://github.com/JuliaMath/Yeppp.jl). It supports more generic array types than the original package. -->


## Installation

Currently, this package is not yet registered. So, `Pkg.add("NumericalEFT")` will not work (yet).

There two ways to install this package:

1. The package can be installed with the Julia package manager. 
From the Julia REPL, type `]` to enter the Pkg REPL mode and run:
```julia
pkg> add https://github.com/numericalEFT/NumericalEFT.jl
```

2. Alternatively, you can run the following command from the Julia REPL:
```julia
julia> using Pkg; Pkg.add(PackageSpec(url="https://github.com/numericalEFT/NumericalEFT.jl"))
```
## Questions and Contributions

Contributions are very welcome, as are feature requests and suggestions. Please open an issue if you encounter any problems.

<!-- Example of Julia package to go along with [these notes](https://tlienart.github.io/pub/julia/dev-pkg2.html). -->
<!-- https://travis-ci.org/github/kunyuan/QuantumStatistics.jl -->
<!-- [![codecov](https://codecov.io/gh/kunyuan/QuantumStatistics.jl/branch/master/graph/badge.svg)](https://codecov.io/gh/kunyuan/QuantumStatistics.jl) -->
