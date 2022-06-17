# GreenFunc

[![Stable](https://img.shields.io/badge/docs-stable-blue.svg)](https://numericalEFT.github.io/GreenFunc.jl/stable)
[![Dev](https://img.shields.io/badge/docs-dev-blue.svg)](https://numericalEFT.github.io/GreenFunc.jl/dev)
[![Build Status](https://github.com/numericalEFT/GreenFunc.jl/actions/workflows/CI.yml/badge.svg?branch=master)](https://github.com/numericalEFT/GreenFunc.jl/actions/workflows/CI.yml?query=branch%3Amaster)
[![Coverage](https://codecov.io/gh/numericalEFT/GreenFunc.jl/branch/master/graph/badge.svg)](https://codecov.io/gh/numericalEFT/GreenFunc.jl)

This library provides structures of different types of Green's function. Here we give a introduction of these Green's function.
## Features

We provide the following containers to save different Green's functions:

 -One body Green's function that has a built-in discrete Lehamnn representation (DLR),  which is a generic and  compact representation of Green's functions proposed in the Ref. [1]. 

For all Green's functions we provide the following manipulations:

- Fast and accurate Fourier transform between the imaginary-time domain and the Matsubara-frequency domain.

- Fast and accurate Green's function interpolation.

## Installation
This package has been registered. So, simply type `import Pkg; Pkg.add("GreenFunc")` in the Julia REPL to install.

## Basic Usage


