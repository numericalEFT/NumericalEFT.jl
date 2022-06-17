[[https://numericaleft.github.io/CompositeGrids.jl/dev/][https://img.shields.io/badge/docs-dev-blue.svg]]
[[https://github.com/numericaleft/CompositeGrids.jl/actions][https://github.com/numericaleft/CompositeGrids.jl/workflows/CI/badge.svg]]
[[https://codecov.io/gh/numericaleft/CompositeGrids.jl][https://codecov.io/gh/numericalEFT/CompositeGrids.jl/branch/main/graph/badge.svg?token=WN6HO1XASY]]

#+OPTIONS: toc:2

* Introduction

  CompositeGrids gives a unified interface to generate various common 1D grids
  and also the composite grids that is a combination of basic grids,
  together with the floor function, interpolation function and also integration function
  that is optimized for some of the grids.
  
* Table of Contents :TOC_2_gh:
- [[#introduction][Introduction]]
- [[#quick-start][Quick Start]]
- [[#installation][Installation]]
- [[#manual][Manual]]
  - [[#basics][Basics]]
  - [[#simple-grids][Simple Grids]]
  - [[#composite-grids][Composite Grids]]
  - [[#interpolation-and-integration][Interpolation and Integration]]

* Quick Start
  
  In the following example we show how to generate a \tau grid from 0 to \beta, log-densed at 0 and \beta,
  and optimized for integration. The description is attached in the comments in the code.
  
  #+begin_src julia :session :results output replace :exports both
    using CompositeGrids
    β = 10
    
    # Generating a log densed composite grid with LogDensedGrid()
    tgrid = CompositeGrid.LogDensedGrid(
        :gauss,# The top layer grid is :gauss, optimized for integration. For interpolation use :cheb
        [0.0, β],# The grid is defined on [0.0, β]
        [0.0, β],# and is densed at 0.0 and β, as given by 2nd and 3rd parameter.
        5,# N of log grid
        0.005, # niminum interval length of log grid
        5 # N of bottom layer
    )
    # The grid has 3 layers.
    # The top layer is defined by the boundary and densed points. In this case its:
    println("Top layer:",tgrid.panel.grid)
    # The middle layer is a log grid with 4 points and minimum interval length 0.001:
    println("First subgrid of middle layer:",tgrid.subgrids[1].panel.grid)
    # The bottom layer is a Gauss-Legendre grid with 5 points:
    println("First subgrid of bottom layer:",tgrid.subgrids[1].subgrids[1].grid)
    
    # function to be integrated:
    f(t) = exp(t)+exp(β-t)
    # numerical value on grid points:
    data = [f(t) for (ti, t) in enumerate(tgrid.grid)]
    
    # integrate with integrate1D():
    int_result = Interp.integrate1D(data, tgrid)
    
    println("result=",int_result)
    println("comparing to:",2*(exp(β)-1))
  #+end_src

  #+RESULTS:
  : Top layer:[0.0, 5.0, 10.0]
  : First subgrid of middle layer:[0.0, 0.005000000000000001, 0.05000000000000001, 0.5, 5.0]
  : First subgrid of bottom layer:[0.00023455038515334025, 0.0011538267247357924, 0.0025000000000000005, 0.0038461732752642086, 0.004765449614846661]
  : result=44050.91248775534
  : comparing to:44050.931589613436
  
* Installation
  
  Static version could be installed via standard package manager with Pkg.add("CompositeGrids").

  For developing version, git clone this repo and add with Pkg.develop("directory/of/the/repo").
  
* Manual

** Basics

   The grids are provided in two modules, SimpleGrid and CompositeGrid. SimpleGrid consists of several
   common 1D grids that is defined straightforward and has simple structure. CompositeGrid defines a
   general type of grids composed by a panel grid and a set of subgrids. The common interface of grids
   are the following:
   - g.bound gives the boundary of the interval of the grid.
   - g.size gives the total number of grid points.
   - g.grid gives the array of grid points.
   - g[i] returns the i-th grid point, same as g.grid[i].
   - floor(g, x) returns the largest index of grid point where g[i]<x. Return 1 for x<g[1] and (grid.size-1) for x>g[end], so that both floor() and (floor()+1) are valid grid indices.

   Interpolation and integration are also provided, with different implemented functions for different grids.

** Simple Grids

   Various basic grids are designed for use and also as components of composite grids, including:
   Arbitrary, Uniform, Log, BaryCheb, and GaussLegendre.

   Arbitrary grid is the most general basic grid, which takes an array and turn it into a grid.
   An O(\ln(N)) floor function based on searchsortedfirst() is provided.

   Uniform grid is defined by the boundary and number of grid points.
   An O(1) floor function is provided.

   Log grid is defined by the boundary, number of grid points, minimum interval, and also the direction.
   A log densed grid is generated according to the parameters provided.
   For example:
   #+begin_src julia :session :results output replace :exports both
     using CompositeGrids
     loggrid = SimpleGrid.Log{Float64}([0.0,1.0], 6, 0.0001, true)
     println(loggrid.grid)
   #+end_src

   #+RESULTS:
   : [0.0, 0.00010000000000000005, 0.0010000000000000002, 0.010000000000000002, 0.1, 1.0]
   An O(1) floor function is provided.

   BaryCheb grid is designed for interpolation. It's defined by the boundary and number of grid points,
   but the grid points are not distributed uniformly. The floor function is not optimized
   so the O(\ln(N)) function will be used, but the interpolation is based on an optimized algorithm.

   GaussLegendre grid is designed for integration. It's defined by the boundary and number of grid points,
   but the grid points are not distributed uniformly. The floor function is not optimized
   so the O(\ln(N)) function will be used. The 1D integration is optimized.

   Also notice that there's open grids and closed grids. Closed grids means that the boundary points are
   also grid points, while open grids means the opposite. Only BaryCheb and GaussLegendre are open.
   
   A detailed manual can be found [[https://numericaleft.github.io/CompositeGrids.jl/dev/lib/simple/][here]].

** Composite Grids

   Composite grid is a general type of grids where the whole interval is first divided by a panel grid,
   then each interval of a panel grid is divided by a smaller grid in subgrids. Subgrid could also be
   composite grid.

   LogDensedGrid is a useful generator of CompositeGrid which gives a general solution when an 1D grid on an
   interval is needed to be log-densed around several points. For example, \tau grids need to be densed around
   0 and \beta, and momentum grids need to be densed around Fermi momentum.
   The grid is defined as a three-layer composite grid with the top layer being an Arbitrary grid defined by
   the boundary and densed points, the middle layer a Log grid which is densed at the points required, and the
   bottom layer a grid of three options. Three types are :cheb, :gauss, and :uniform, which corresponds to
   BaryCheb grid for interpolation, GaussLegendre grid for integration, and Uniform grid for general use.
   The floor function is defined recursively, i.e. the floor function of the panel grid is called to find the
   corresponding subgrid, and then the floor function of the subgrid is called to find the result. Since the
   subgrids could also be CompositeGrid, this process continues until the lowest level of the subgrids is reached.

   A detailed manual can be found [[https://numericaleft.github.io/CompositeGrids.jl/dev/lib/composite/][here]].
      
** Interpolation and Integration

   Interpolation gives an estimate of the function value at x with given grid and function value on the grid.
   For most of the simple grids the interpolation is given by linear interpolation with the floor function to find
   the corresponding grid points. BaryCheb uses an optimized algorithm for interpolation which makes use of the information
   of all grid points, and thus gives a more precise interpolation with the same number of grid points, given the condition that
   the function itself is smooth enough. For composite grids, the interpolation is done recursively, so that the final result
   depends on the type of lowest level grid. Interpolation for higher dimension where the data is defined on a list of grids is also
   given, but only linear interpolation is implemented, even when some of the grids are BaryCheb.

   Integration over 1D grid is also provided. For most of simple grids it's given by linear integral, while for GaussLegendre grid it's
   optimized. For composite grids it's again recursively done so that the method depends on the type of lowest level grids.
   
   A detailed manual can be found [[https://numericaleft.github.io/CompositeGrids.jl/dev/lib/interpolate/][here]].

