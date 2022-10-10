module Lehmann
using StaticArrays
using DelimitedFiles, LinearAlgebra
using Printf

include("utility/chebyshev.jl")

include("spectral.jl")
export Spectral

include("discrete/builder.jl")
include("functional/builder.jl")

include("dlr.jl")
export DLRGrid

include("sample/sample.jl")
export Sample

include("operation.jl")
export tau2dlr, dlr2tau, matfreq2dlr, dlr2matfreq, tau2matfreq, matfreq2tau, tau2tau, matfreq2matfreq

##################### precompile #######################
# precompile as the final step of the module definition:
if ccall(:jl_generating_output, Cint, ()) == 1   # if we're precompiling the package
    let
        #cover data type and symmetry
        # Float32, :none
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :none; dtype=Float32)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)


        # Float32, :pha
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :pha; dtype=Float32)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)

        # Float32, :ph
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :ph; dtype=Float32)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)

        # Float64, :none
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :none; dtype=Float64)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)

        # Float64, :pha
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :pha; dtype=Float64)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)

        # Float64, :ph
        dlr = DLRGrid(1.0, 1.0, 1e-6, true, :ph; dtype=Float64)
        g = Sample.SemiCircle(dlr, :τ, dlr.τ)
        coeff = tau2dlr(dlr, g)
        _g = dlr2tau(dlr, coeff)
        _g = tau2tau(dlr, g, dlr.τ)

        g = Sample.SemiCircle(dlr, :n, dlr.n)
        coeff = matfreq2dlr(dlr, g)
        _g = dlr2matfreq(dlr, coeff)
        _g = matfreq2matfreq(dlr, g, dlr.n)

    end
end

end
