# Package QuantumStatistics

A platform for numerical experiments on quantum statistics.

| Status | Coverage | Document
| :----: | :----: | :----: |
| [![Build Status](https://travis-ci.org/kunyuan/QuantumStatistics.jl.svg?branch=master)](https://travis-ci.org/kunyuan/QuantumStatistics.jl) | [![codecov](https://codecov.io/gh/kunyuan/QuantumStatistics.jl/branch/master/graph/badge.svg)](https://codecov.io/gh/kunyuan/QuantumStatistics.jl) | [![Document](https://img.shields.io/badge/docs-dev-blue.svg)](https://kunyuan.github.io/QuantumStatistics.jl/dev) |

## Installation

The package can be installed with the Julia package manager. 
From the Julia REPL, type `]` to enter the Pkg REPL mode and run:
```
pkg> add https://github.com/kunyuan/QuantumStatistics.jl
```
<!-- 
Or, equivalently, via the `Pkg` API:
```julia
julia> import Pkg; Pkg.add("https://github.com/kunyuan/QuantumStatistics.jl")
``` 
-->

## Documentation
<!-- - [**STABLE**][docs-stable-url] &mdash; **documentation of the most recently tagged version.** -->
- [**DEVEL**](https://kunyuan.github.io/QuantumStatistics.jl/dev/) &mdash; *documentation of the in-development version.*

## Project Status

The package is in development stage. Many components are missing. For now, we have implemented the following utilities:

- Fermionic Green's function in both the imaginary-time and Matsubara frequency.
- One-dimensional basis for the correlation functions in the imaginary-time, and fermionic/bosonic momentum.
- Fast elementary math functions. Some of them are adapted from the [Yeppp.jl package](https://github.com/JuliaMath/Yeppp.jl). It supports more generic array types than the original package.

## Questions and Contributions

Contributions are very welcome, as are feature requests and suggestions. Please open an issue if you encounter any problems.

<!-- Example of Julia package to go along with [these notes](https://tlienart.github.io/pub/julia/dev-pkg2.html). -->
<!-- https://travis-ci.org/github/kunyuan/QuantumStatistics.jl -->
<!-- [![codecov](https://codecov.io/gh/kunyuan/QuantumStatistics.jl/branch/master/graph/badge.svg)](https://codecov.io/gh/kunyuan/QuantumStatistics.jl) -->