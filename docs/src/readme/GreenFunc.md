# GreenFunc



[![Stable](https://img.shields.io/badge/docs-stable-blue.svg)](https://numericalEFT.github.io/GreenFunc.jl/stable)
[![Dev](https://img.shields.io/badge/docs-dev-blue.svg)](https://numericalEFT.github.io/GreenFunc.jl/dev)
[![Build Status](https://github.com/numericalEFT/GreenFunc.jl/actions/workflows/CI.yml/badge.svg?branch=master)](https://github.com/numericalEFT/GreenFunc.jl/actions/workflows/CI.yml?query=branch%3Amaster)
[![Coverage](https://codecov.io/gh/numericalEFT/GreenFunc.jl/branch/master/graph/badge.svg)](https://codecov.io/gh/numericalEFT/GreenFunc.jl)

GreenFunc.jl is a differentiable numerical framework to manipulate multidimensional Green's functions.

## Features
 - `MeshArray` type as an array defined on meshes, which provides a generic data structure for Green's functions, vertex functions or any other correlation/response functions.
 - Structured (non-)uniform Brillouin Zone meshes powered by the package [`BrillouinZoneMeshes.jl`](https://github.com/numericalEFT/BrillouinZoneMeshes.jl).
 - Structured (non-)uniform temporal meshes for (imaginary-)time or (Matsubara-)frequency domains powered by the pacakge [`CompositeGrids.jl`](https://github.com/numericalEFT/CompositeGrids.jl).
 - Compat representation based on the Discrete Lehmann representation (DLR) powered by the package [`Lehmann.jl`](https://github.com/numericalEFT/Lehmann.jl).
 - Accurate and fast Fourier transform.
 - Interface to the [`TRIQS`](https://triqs.github.io/) library.
 
## Installation
This package has been registered. So, simply type `import Pkg; Pkg.add("GreenFunc")` in the Julia REPL to install.

## Basic Usage

### Example 1: Green's function of a single level

We first show how to use `MeshArray` to present Green's function of a single-level quantum system filled with spinless fermionic particles. We assume that the system could exchange particles and energy with the environment so that it's equilibrium state is a grand canonical ensemble. The single-particle Green's function then has a simple form in Matsubara-frequency representation:  $G(ωₙ) = \frac{1}{(iωₙ - E)}$ where $E$ is the level energy. We show how to generate and manipulate this Green's function.
     
```julia
    using GreenFunc

    β = 100.0; E = 1.0 # inverse temperature and the level energy
    ωₙ_mesh = MeshGrids.ImFreq(100.0, FERMION; Euv = 100E) # UV energy cutoff is 100 times larger than the level energy
    Gn =  MeshArray(ωₙ_mesh; dtype=ComplexF64); # Green's function defined on the ωₙ_mesh

    for (n, ωₙ) in enumerate(Gn.mesh[1])
        Gn[n] = 1/(ωₙ*im - E)
    end
```

- Green's function describes correlations between two or more spacetime events. The spacetime continuum needs to be discretized into spatial and temporal meshes. This example demonstrates how to define a one-body Green's function on a temporal mesh. The package provides three types of temporal meshes: imaginary-time grid, Matsubara-frequency grid, and DLR grid. The latter provides a generic compressed representation for Green's functions (We will show how to use DLR later).  Correspondingly, They can be created with the `ImTime`, `ImFreq`, and `DLRFreq` methods. The user needs to specify the inverse temperature, whether the particle is fermion or boson (using the constant `FERMION` or `BOSON`). Internally, a set of non-uniform grid points optimized for the given inverse temperature and the cutoff energy will be created with the given parameters.

- Once the meshes are created, one can define a `MeshArray` on them to represent the Green's function `Gn`. The constructor of `MeshArray` takes a set of meshes and initializes a multi-dimensional array. Each mesh corresponds to one dimension of the array. The data type of the `MeshArray` is specified by the optional keyword argument `dtype`, which is set to `Float64` by default. You can access the meshes (stored as a tuple) with `Gn.mesh` and the array data with `Gn.data`.

- By default, `Gn.data` is left undefined if not specified by the user. To initialize it, one can either use the optional keyword argument `data` in the constructor or use the iterator interface of the meshes and the `MeshArray`. 

### Example 2: Green's function of a free electron gas

Now let us show how to create a Green's function of a free electron gas. Unlike the spinless fermionic particle, the electron is a spin-1/2 particle so that it has two inner states. In free space, it has a kinetic energy $ϵ_q = q^2-E$ (we use the unit where $m_e = 1/2$). The Green's function in Matsubara-frequency space is then given by the following equation: $G_n = G_{\sigma_1, \sigma_2}(q,\omega_n) = \frac{1}{i \omega_n - \epsilon_q}$, where $\sigma_i$ denotes the spins of the incoming and the outgoing electron in the propagator. We inherit the Matsubara-frequency grid from the first example. We show how to use the `CompositeGrids` package to generate momentum grids and how to treat the multiple inner states and the meshes with `MeshArray`.
```julia
    using GreenFunc, CompositeGrids
    β = 100.0; E = 1.0 # inverse temperature and the level energy
    ωₙ_mesh = MeshGrids.ImFreq(100.0, FERMION; Euv = 100E) # UV energy cutoff is 100 times larger than the level energy
    kmesh = SimpleGrid.Uniform{Float64}([0.0, 10.0], 50); # initialze an uniform momentum grid
    G_n =  MeshArray(1:2, 1:2, kmesh, ωₙ_mesh; dtype=ComplexF64); # Green's function of free electron gas with 2x2 innerstates

    for ind in eachindex(G_n)
        q = G_n.mesh[3][ind[3]]
        ω_n = G_n.mesh[4][ind[4]]
        G_n[ind] = 1/(ω_n*im - (q^2-E))
    end
```
- One can generate various types of grids with the `CompositeGrids` package. The `SimpleGrid` module here provides several basic grids, such as uniform grids and logarithmically dense grids. The` Uniform` method here generates a 1D linearly spaced grid. The user has to specify the number of grid points `N` and the boundary points `[min, max]`. One can also combine arbitrary numbers of `SimpleGrid` subgrids with a user-specified pattern defined by a `panel grid`. These more advanced grids optimized for different purposes can be found in this [link](https://github.com/numericalEFT/CompositeGrids.jl).

- The constructor of `MeshArray` can take any iterable objects as one of its meshes. Therefore for discrete inner states such as spins, one can simply use a `1:2`, which is a `UnitRange{Int64}` object.

### Example 3: Green's function of a Hubbard lattice

Now we show how to generate a multi-dimensional Green's function on a Brillouin Zone meshe. We calculate the Green's function of a free spinless Fermi gas on a square lattice. It has a tight-binding dispersion $\epsilon_q = -2t(\cos(q_x)+\cos(q_y))$, which gives
$G(q, \omega_n) = \frac{1}{i\omega_n - \epsilon_q}$.
The momentum is defined on the first Brillouin zone captured by a 2D k-mesh.

```julia
    using GreenFunc
    using GreenFunc: BrillouinZoneMeshes

    DIM, nk = 2, 8
    latvec = [1.0 0.0; 0.0 1.0] .* 2π
    bzmesh = BrillouinZoneMeshes.BaseMesh.UniformMesh{DIM, nk}([0.0, 0.0], latvec)
    ωₙmesh = ImFreq(10.0, FERMION)
    g_freq =  MeshArray(bzmesh, ωₙmesh; dtype=ComplexF64)

    t = 1.0
    for ind in eachindex(g_freq)
        q = g_freq.mesh[1][ind[1]]
        ωₙ = g_freq.mesh[2][ind[2]]
        g_freq[ind] = 1/(ωₙ*im - (-2*t*sum(cos.(q))))
    end
```

- For lattice systems with multi-dimensional Brillouin zone, the momentum grids internally generated with the [`BrillouinZoneMeshes.jl`](https://github.com/numericalEFT/BrillouinZoneMeshes.jl) package. Here a `UniformMesh{DIM,N}(origin, latvec)` generates a linearly spaced momentum mesh on the first Brillouin zone defined by origin and lattice vectors given. For more detail, see the [link](https://github.com/numericalEFT/BrillouinZoneMeshes.jl).


### Example 4:  Fourier Transform of Green's function with DLR
DLR provides a compact representation for one-body Green's functions. At a temperature $T$ and an accuracy level $\epsilon$, it represents a generic Green's function with only $\log (1/T) \log (1/\epsilon)$ basis functions labeled by a set of real frequency grid points. It enables fast Fourier transform and interpolation between the imaginary-time and the Matsubara-frequency representations with a cost $O(\log (1/T) \log (1/\epsilon))$. `GreenFunc.jl` provide DLR through the package [`Lehmann.jl`](https://github.com/numericalEFT/Lehmann.jl).

In the following example, we demonstrate how to perform DLR-based Fourier transform in `GreenFunc.jl` between the imaginary-time and the Matsubara-frequency domains back and forth through the DLR representation.
```julia
    using GreenFunc, CompositeGrids

    β = 100.0; E = 1.0 # inverse temperature and the level energy
    ωₙ_mesh = ImFreq(100.0, FERMION; Euv = 100E) # UV energy cutoff is 100 times larger than the level energy
    kmesh = SimpleGrid.Uniform{Float64}([0.0, 10.0], 50); # initialze an uniform momentum grid
    G_n =  MeshArray(1:2, 1:2, kmesh, ωₙ_mesh; dtype=ComplexF64); # Green's function of free electron gas with 2x2 innerstates

    for ind in eachindex(G_n)
        q = G_n.mesh[3][ind[3]]
        ω_n = G_n.mesh[4][ind[4]]
        G_n[ind] = 1/(im*ω_n - (q^2-E))
    end

    G_dlr = to_dlr(G_n) # convert G_n to DLR space 
    G_tau = to_imtime(G_dlr) # convert G_dlr to the imaginary-time domain 

    #alternative, you can use the pipe operator
    G_tau = G_n |> to_dlr |> to_imtime #Fourier transform to (k, tau) domain

```
The imaginary-time Green's function after the Fourier transform shoud be consistent with the analytic solution $G_{\tau} = -e^{-\tau \epsilon_q}/(1+e^{-\beta \epsilon_q})$.

- For any Green's function that has at least one imaginary-temporal grid (`ImTime`, `ImFreq`, and `DLRFreq`) in meshes, we provide a set of operations (`to_dlr`, `to_imfreq` and `to_imtime`) to bridge the DLR space with imaginary-time and Matsubara-frequency space. By default, all these functions find the dimension of the imaginary-temporal mesh within Green's function meshes and perform the transformation with respect to it. Alternatively, one can specify the dimension with the optional keyword argument `dim`. Be careful that the original version of DLR is only guaranteed to work with one-body Green's function.

- Once a spectral density `G_dlr` in DLR space is obtained, one can use `to_imfreq` or `to_imtime` methods to reconstruct the Green's function in the corresponding space. By default, `to_imfreq` and `to_imtime` uses an optimized imaginary-time or Matsubara-frequency grid from the DLR. User can assign a target imaginary-time or Matsubara-frequency grid if necessary.   

- Combining `to_dlr`, `to_imfreq` and `to_imtime` allows both _interpolation_ as well as _Fourier transform_.

- Since the spectral density `G_dlr` can be reused whenever the user wants to change the grid points of Green's function (normally through interpolation that lost more accuracy than DLR transform), we encourage the user always to keep the `G_dlr` objects. If the intermediate DLR Green's function is not needed, the user can use piping operator `|>` as shown to do Fourier transform directly between `ImFreq` and `ImTime` in one line.

##  Interface with TRIQS

TRIQS (Toolbox for Research on Interacting Quantum Systems) is a scientific project providing a set of C++ and Python libraries for the study of interacting quantum systems. We provide a direct interface to convert TRIQS objects, such as the temporal meshes, the Brillouin zone meshes, and the  multi-dimensional (blocked) Green's functions, to the equivalent objects in our package. It would help TRIQS users to make use of our package without worrying about the different internal data structures.

We rely on the package [`PythonCall.jl`](https://github.com/cjdoris/PythonCall.jl) to interface with the python language. You need to install TRIQS package from the python environment that `PythonCall` calls. We recommand you to check the sections [`Configuration`](https://cjdoris.github.io/PythonCall.jl/stable/pythoncall/#pythoncall-config) and [`Installing Python Package`](https://cjdoris.github.io/PythonCall.jl/stable/pythoncall/#python-deps) in the `PythonCall` documentation.

### Example 5: Load Triqs Temporal Mesh
First we show how to import an imaginary-time mesh from TRIQS.
```julia
    using PythonCall, GreenFunc
    gf = pyimport("triqs.gf")
    np = pyimport("numpy")

    mt = gf.MeshImTime(beta=1.0, S="Fermion", n_max=3)
    mjt = from_triqs(mt)
    for (i, x) in enumerate([p for p in mt.values()])
        @assert mjt[i] ≈ pyconvert(Float64, x) # make sure mjt is what we want
    end
    
```
- With the `PythonCall` package, one can import python packages with `pyimport` and directly exert python code in Julia. Here we import the Green's function module `triqs.gf` and generate a uniform imaginary-time mesh with `MeshImTime`. The user has to specify the inverse temperature,  whether the particle is fermion or boson, and the number of grid points.

- Once a TRIQS object is prepared, one can simply convert it to an equivalent object in our package with `from_triqs`. The object can be a mesh, a Green's function, or a block Green's function. In this example, the TRIQS imaginary time grid is converted to an identical `ImTime` grid.

### Example 6: Load Triqs BrillouinZone

In this example, we show how the Brillouin zone mesh from TRIQS can be converted to a UniformMesh from the `BrillouinZoneMeshes` package and clarify the convention we adopted to convert a Python data structure to its Julia counterpart.

```julia
    using PythonCall, GreenFunc

    # construct triqs Brillouin zone mesh
    lat = pyimport("triqs.lattice")
    gf = pyimport("triqs.gf")
    BL = lat.BravaisLattice(units=((2, 0, 0), (1, sqrt(3), 0))) 
    BZ = lat.BrillouinZone(BL)
    nk = 4
    mk = gf.MeshBrillouinZone(BZ, nk)

    # load Triqs mesh and construct 
    mkj = from_triqs(mk)

    for p in mk
        pval = pyconvert(Array, p.value)
        # notice that TRIQS always return a 3D point, even for 2D case(where z is always 0)
        # notice also that Julia index starts from 1 while Python from 0
        # points of the same linear index has the same value
        ilin = pyconvert(Int, p.linear_index) + 1
        @assert pval[1:2] ≈ mkj[ilin]
        # points with the same linear index corresponds to REVERSED cartesian index
        inds = pyconvert(Array, p.index)[1:2] .+ 1
        @assert pval[1:2] ≈ mkj[reverse(inds)...]
    end
```

- Julia uses column-major layout for multi-dimensional array similar as Fortran and matlab, whereas python uses row-major layout. The converted Julias Brillouin zone mesh wll be indexed differently from that in TRIQS.
- We adopted the convention so that the grid point and linear index are consistent with TRIQS counterparts, while the orders of Cartesian index
and lattice vector are reversed.
- Here's a table of 2D converted mesh v.s. the Triqs counterpart:

| Object          | TRIQS             | GreenFunc.jl   |
| --------------- | ----------------- | -------------- |
| Linear index    | mk[i]=(x, y, 0)   | mkj[i]= (x, y) |
| Cartesian index | mk[i,j]=(x, y, 0) | mkj[j,i]=(x,y) |
| Lattice vector  | (a1, a2)          | (a2, a1)       |

### Example 7: Load Triqs Greens function of a Hubbard Lattice

A TRIQS Green's function is defined on a set of meshes of continuous variables, together with the discrete inner states specified by the `target_shape`. The structure is immediately representable by `MeshArray`. In the following example, we reimplement the example 3 to first show how to generate a TRIQS Green's function of a Hubbard lattice within Julia, then convert the TRIQS Green's function to a julia `MeshArray` object. The Green's function is given by $G(q, \omega_n) = \frac{1}{i\omega_n - \epsilon_q}$ with $\epsilon_q = -2t(\cos(q_x)+\cos(q_y))$. 

```julia
    using PythonCall, GreenFunc
    
    np = pyimport("numpy")
    lat = pyimport("triqs.lattice")
    gf = pyimport("triqs.gf")
    
    BL = lat.BravaisLattice(units=((2, 0, 0), (1, sqrt(3), 0))) # testing with a triangular lattice so that exchanged index makes a difference
    BZ = lat.BrillouinZone(BL)
    nk = 20
    mk = gf.MeshBrillouinZone(BZ, nk)
    miw = gf.MeshImFreq(beta=1.0, S="Fermion", n_max=100)
    mprod = gf.MeshProduct(mk, miw)

    G_w = gf.GfImFreq(mesh=miw, target_shape=[1, 1]) #G_w.data.shape will be [201, 1, 1]
    G_k_w = gf.GfImFreq(mesh=mprod, target_shape = [2, 3] ) #target_shape = [2, 3] --> innerstate = [3, 2]

    # Due to different cartesian index convention in Julia and Python, the data g_k_w[n, m, iw, ik] corresponds to G_k_w.data[ik-1, iw-1, m-1, n-1])

    t = 1.0
    for (ik, k) in enumerate(G_k_w.mesh[0])
        G_w << gf.inverse(gf.iOmega_n - 2 * t * (np.cos(k[0]) + np.cos(k[1])))
        G_k_w.data[ik-1, pyslice(0, nk^2), pyslice(0, G_k_w.target_shape[0]) , pyslice(0,G_k_w.target_shape[1])] = G_w.data[pyslice(0, nk^2), pyslice(0, G_w.target_shape[0]) , pyslice(0,G_w.target_shape[1])] #pyslice = :      
    end

    g_k_w = from_triqs(G_k_w)
    
    #alternatively, you can use the MeshArray constructor to convert TRIQS Green's function to a MeshArray
    g_k_w2 = MeshArray(G_k_w) 
    @assert g_k_w2 ≈ g_k_w

    #Use the << operator to import python data into an existing MeshArray 
    g_k_w2 << G_k_w
    @assert g_k_w2 ≈ g_k_w
    
```
- When converting a TRIQS Green's function into a `MeshArray` julia object, the `MeshProduct` from TRIQS is decomposed into separate meshes and converted to the corresponding Julia meshes. The `MeshArray` stores the meshes as a tuple object, not as a `MeshProduct`.
- The `target_shape` in TRIQS Green's function is converted to a tuple of `UnitRange{Int64}` objects that represents the discrete degrees of freedom. Data slicing with `:` is not available in `PythonCall`. One needs to use `pyslice` instead.
- As explained in Example 6, the cartesian index order of data has to be inversed during the conversion.
- We support three different interfaces for the conversion of TRIQS Green's function. One can construct a new MeshArray with `from_triqs` or `MeshArray` constructor. One can also load TRIQS Green's function into an existing `MeshArray` with the `<<` operator.

### Example 8: Load Triqs block Greens function

The block Greens function in TRIQS can be converted to a dictionary of `MeshArray` objects in julia. 

```julia
    using PythonCall, GreenFunc

    gf = pyimport("triqs.gf")
    np = pyimport("numpy")
    mt = gf.MeshImTime(beta=1.0, S="Fermion", n_max=3)
    lj = pyconvert(Int, @py len(mt))
    G_t = gf.GfImTime(mesh=mt, target_shape=[2,3]) #target_shape = [2, 3] --> innerstate = [3, 2]
    G_w = gf.GfImTime(mesh=mt, target_shape=[2,3]) #target_shape = [2, 3] --> innerstate = [3, 2]

    blockG = gf.BlockGf(name_list=["1", "2"], block_list=[G_t, G_w], make_copies=false)

    jblockG = from_triqs(blockG) 
    #The converted block Green's function is a dictionary of MeshArray corresponding to TRIQS block Green's function. The mapping between them is: jblockG["name"][i1, i2, t] = blockG["name"].data[t-1, i2-1, i1-1]

```

