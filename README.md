# NumericalEFT

Numerical effective field theory toolbox for quantum many-body problem.

[![Stable](https://img.shields.io/badge/docs-stable-blue.svg)](https://numericalEFT.github.io/NumericalEFT.jl/)
[![Dev](https://img.shields.io/badge/docs-dev-blue.svg)](https://numericalEFT.github.io/NumericalEFT.jl/)
[![Build Status](https://github.com/numericalEFT/NumericalEFT.jl/workflows/CI/badge.svg)](https://github.com/numericalEFT/NumericalEFT.jl/actions)
[![codecov](https://codecov.io/gh/numericalEFT/NumericalEFT.jl/branch/master/graph/badge.svg?token=OKnDPEC3In)](https://codecov.io/gh/numericalEFT/NumericalEFT.jl)

## Motivation

Collective modes of quantum many-body systems are often well described by weakly-interacting quantum fields. Effective field theory (EFT) is a framework to systematically study such problems. EFT has been widely used in high-energy physics, nuclear physics and condensed matter physics. 

For quantum material applications, EFTs are often too complicated to calculate with bare hands. It motivates us to develop this numerical package.

Potential applications of Numerical EFT are the electron liquids in real materials, chiral EFT in neutron-rich matter, and all kinds of emergent low-energy field theories in lattice models.

## Features

The package ``NumericalEFT.jl`` is a collection of several independent components, which are organized in the following infrastructure: 

![NumericalEFT](docs/src/assets/numericalEFT2.svg)

Most of the components have been published as independent packages, you can test or use them separately. The packages are still fast envolving, but many of them are production ready.

### Core packages: a set of self-contained and general-purpose tools.

- [Lehmann.jl](https://github.com/numericalEFT/Lehmann.jl): Discrete Lehmann representation (DLR) for imaginary-time/Matsubara frequency Green's function. For a generic Green's function at a temperature T, DLR is capable of representing it up to a given accuracy ϵ with a cost ~ log(1/T)log(1/ϵ), signicantly cheaper than a naive approach with a cost ~ 1/(Tϵ). 

- [FeynmanDiagram.jl](https://github.com/numericalEFT/FeynmanDiagram.jl): A mini-compiler that compiles generic multi-loop Feynman diagrams to a computational graph, which then can be efficiently manipulated or evaluated. The same type of computatoinal graph has been the corner of the neural-network-based machine learning.

- [MCIntegration.jl](https://github.com/numericalEFT/MCIntegration.jl): An adapative Monte Carlo calculator for general high dimensional integral. It is one of the most robust and fastest Monte Carlo integration package on the market. 

- [CompositeGrids.jl](https://github.com/numericalEFT/CompositeGrids.jl): Composite Cheybeshev/Gaussian/logarithmic grid systems for highly efficient one-dimensional interpolation and integration. It allows the user to combine different grids to represent nontrivial functions.

- [BZMeshs.jl](https://github.com/numericalEFT/BZMeshs.jl)(WIP): Compact mesh systems for representing and manipulating functions defined on generic Brillouin zone in 2D or 3D. 

### Toolbox packages: integrated software for quantum many-body problems

- [GreenFunc.jl](https://github.com/numericalEFT/GreenFunc.jl): A software to calculate, manipulate and analyze the correlation functions, Green's functions and the vertex functions at the tree-level, which characterize the quantum field dynamics in the semiclassical limit ħ→0. It consists of:
  - A container of generic Green's functions based on the discrete Lehmann representation and adaptive mesh systems. 
  - Several helper functions to generate typical Green's functions. For example, a set of functions to calculate the Many-body Green's functions of a small cluster of Hubbard-type atoms a small-scale exact diagonalization solver. 
  - Import(export) Green's function from(to) the [Triqs](https://triqs.github.io/triqs/latest/).

- [MultiLoop.jl](https://github.com/numericalEFT/MultiLoop.jl)(WIP): A software to calculate and analyze the quantum many-body effects beyond the semiclassical limit. It consists of:
  - Diagrammatic Monte Carlo calculator of multi-loop Feynman diagrams of correlation functions and vertex functions. 
  - Renormalization technique to improve the perturbation theory.
  - Renormalization Group analysis.

### Applications

- [ElectronGas.jl](https://github.com/numericalEFT/ElectronGas.jl): A package that uses the numerical effective field theory to explore many-fermion systems with singular interactions. It could be used to study the uniform electron gas problem, Hertz-Millis theory, etc. 

- QuantumMaterials.jl (WIP): ab-initio methods for real materials based on an effective field theory.

<!-- - Fast elementary math functions. Some of them are adapted from the package [Yeppp.jl](https://github.com/JuliaMath/Yeppp.jl). It supports more generic array types than the original package. -->


## Installation

This package has been registered. So, simply type ``import Pkg; Pkg.add("NumericalEFT")`` in the Julia REPL to install.

## Questions and Contributions

Contributions are very welcome, as are feature requests and suggestions. Please open an issue if you encounter any problems.