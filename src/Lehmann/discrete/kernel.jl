struct CompositeChebyshevGrid
    degree::Int # Chebyshev degree
    x::Vector{Float64} # Chebyshev nodes
    w::Vector{Float64} # Chebyshev node weights

    np::Int # number of panel
    panel::Vector{Float64}

    ngrid::Int # size of the grid = (np-1)*degree
    grid::Vector{Float64}  # fine grid
    function CompositeChebyshevGrid(degree, panel)
        # fill each panel with N order Chebyshev nodes
        np = length(panel) # number of panels break points
        xc, wc = Interp.barychebinit(degree)
        fineGrid = zeros(Float64, (np - 1) * degree) # np break points have np-1 panels
        for i = 1:np-1
            a, b = panel[i], panel[i+1]
            fineGrid[(i-1)*degree+1:i*degree] = a .+ (b - a) .* (xc .+ 1.0) ./ 2
        end
        return new(degree, xc, wc, np, panel, (np - 1) * degree, fineGrid)
    end
end

function ωChebyGrid(dlrGrid, degree, print = true)
    Λ, rtol = dlrGrid.Λ, dlrGrid.rtol

    npo = Int(ceil(log(Λ) / log(2.0))) # subintervals on [0,lambda] in omega space (subintervals on [-lambda,lambda] is 2*npo)

    if dlrGrid.symmetry == :ph || dlrGrid.symmetry == :pha
        # Panel break points for the real frequency ∈ [0, Λ]
        # get exponentially dense near 0⁺
        pbpo = zeros(Float64, npo + 1)
        pbpo[1] = 0.0
        for i = 1:npo
            pbpo[i+1] = Λ / 2^(npo - i)
        end
    else #τ in (0, 1)
        ############ ω discretization ##################
        # Panel break points for the real frequency ∈ [-Λ, Λ]
        # get exponentially dense near 0⁻ and 0⁺
        pbpo = zeros(Float64, 2npo + 1)
        pbpo[npo+1] = 0.0
        for i = 1:npo
            pbpo[npo+i+1] = Λ / 2^(npo - i)
        end
        pbpo[1:npo] = -pbpo[2npo+1:-1:npo+2]
    end
    return CompositeChebyshevGrid(degree, pbpo)
end

function τChebyGrid(dlrGrid, degree, print = true)
    Λ, rtol = dlrGrid.Λ, dlrGrid.rtol

    npt = Int(ceil(log(Λ) / log(2.0))) - 2 # subintervals on [0,1/2] in tau space (# subintervals on [0,1] is 2*npt)

    if dlrGrid.symmetry == :ph || dlrGrid.symmetry == :pha
        ############# Tau discretization ##############
        # Panel break points for the imaginary time ∈ (0, 1)
        # get exponentially dense near 0⁺ 
        pbpt = zeros(Float64, npt + 1)
        pbpt[1] = 0.0
        for i = 1:npt
            pbpt[i+1] = 0.5 / 2^(npt - i)
        end

    else #τ in (0, 1)
        ############# Tau discretization ##############
        # Panel break points for the imaginary time ∈ (0, 1)
        # get exponentially dense near 0⁺ and 1⁻ 
        pbpt = zeros(Float64, 2npt + 1)
        pbpt[1] = 0.0
        for i = 1:npt
            pbpt[i+1] = 1.0 / 2^(npt - i + 1)
        end
        pbpt[npt+2:2npt+1] = 1 .- pbpt[npt:-1:1]
    end

    return CompositeChebyshevGrid(degree, pbpt)
end

"""
function preciseKernelT(dlrGrid, τ, ω, print::Bool = true)

    Calculate the kernel matrix(τ, ω) for given τ, ω grids

# Arguments
- τ: a CompositeChebyshevGrid struct or a simple one-dimensional array
- ω: a CompositeChebyshevGrid struct or a simple one-dimensional array
- print: print information or not
"""
function preciseKernelT(dlrGrid, τ, ω, print::Bool = true)
    # Assume τ.grid is particle-hole symmetric!!!
    # if (τ isa CompositeChebyshevGrid)
    #     @assert (τ.np - 1) * τ.degree == τ.ngrid
    # end
    τGrid = (τ isa CompositeChebyshevGrid) ? τ.grid : τ
    ωGrid = (ω isa CompositeChebyshevGrid) ? ω.grid : ω
    kernel = zeros(Float64, (length(τGrid), length(ωGrid)))
    symmetry = dlrGrid.symmetry


    if symmetry == :none && (τ isa CompositeChebyshevGrid) && (ω isa CompositeChebyshevGrid)
        #symmetrize K(τ, ω)=K(β-τ, -ω) for τ>0 
        @assert isodd(τ.np) #symmetrization is only possible for odd τ panels
        halfτ = ((τ.np - 1) ÷ 2) * τ.degree
        kernel[1:halfτ, :] = Spectral.kernelT(Val(true), Val(symmetry), τ.grid[1:halfτ], ω.grid, 1.0, true)
        kernel[end:-1:(halfτ+1), :] = Spectral.kernelT(Val(true), Val(symmetry), τ.grid[1:halfτ], ω.grid[end:-1:1], 1.0, true)
        # use the fermionic kernel for both the fermionic and bosonic propagators
    else
        kernel = Spectral.kernelT(Val(dlrGrid.isFermi), Val(symmetry), τGrid, ωGrid, 1.0, true)
    end

    # print && println("=====  Kernel Discretization =====")
    # print && println("fine grid points for τ     = ", τGrid)
    # print && println("fine grid points for ω     = ", ωGrid)
    return kernel
end

function testInterpolation(dlrGrid, τ, ω, kernel, print = true)
    ############# test interpolation accuracy in τ #######
    τGrid = (τ isa CompositeChebyshevGrid) ? τ.grid : τ
    ωGrid = (ω isa CompositeChebyshevGrid) ? ω.grid : ω
    if τ isa CompositeChebyshevGrid
        τ2 = CompositeChebyshevGrid(τ.degree * 2, τ.panel)
        kernel2 = preciseKernelT(dlrGrid, τ2, ω, print)
        err = 0.0
        for ωi = 1:length(ωGrid)
            tmp = 0.0
            for i = 1:τ2.np-1
                for k = 1:τ2.degree
                    τidx = (i - 1) * τ2.degree + k
                    kaccu = kernel2[τidx, ωi]
                    kinterp = Interp.barycheb(τ.degree, τ2.x[k], kernel[(i-1)*τ.degree+1:i*τ.degree, ωi], τ.w, τ.x)
                    tmp = max(tmp, abs(kaccu - kinterp))
                end
            end
            err = max(err, tmp / maximum(kernel[:, ωi]))
        end
        print && println("Max relative L∞ error of kernel discretization in τ = ", err)
        @assert err < dlrGrid.rtol "Discretization error is too big! $err >= $(dlrGrid.rtol). Increase polynominal degree."
    end

    if ω isa CompositeChebyshevGrid
        ω2 = CompositeChebyshevGrid(ω.degree * 2, ω.panel)
        kernel2 = preciseKernelT(dlrGrid, τ, ω2)
        err = 0.0
        for τi = 1:length(τGrid)
            tmp = 0.0
            for i = 1:ω2.np-1
                for k = 1:ω2.degree
                    idx = (i - 1) * ω2.degree + k
                    kaccu = kernel2[τi, idx]
                    kinterp = Interp.barycheb(ω.degree, ω2.x[k], kernel[τi, (i-1)*ω.degree+1:i*ω.degree], ω.w, ω.x)
                    tmp = max(tmp, abs(kaccu - kinterp))
                end
            end
            err = max(err, tmp / maximum(kernel[τi, :]))
        end
        print && println("Max relative L∞ error of kernel discretization in ω = ", err)
        @assert err < dlrGrid.rtol "Discretization error is too big! $err >= $(dlrGrid.rtol). Increase polynominal degree."
    end
end

function preciseKernelΩn(dlrGrid, ω, print::Bool = true)
    function Freq2Index(isFermi, ωnList)
        if isFermi
            # ωn=(2n+1)π
            return [Int(round((ωn / π - 1) / 2)) for ωn in ωnList]
        else
            # ωn=2nπ
            return [Int(round(ωn / π / 2)) for ωn in ωnList]
        end
    end

    function nGrid(isFermi, symmetry, Λ)
        # generate n grid from a logarithmic fine grid
        degree = 100
        np = Int(round(log(10 * Λ) / log(2)))
        xc = [(i - 1) / degree for i = 1:degree]
        panel = [2^(i - 1) - 1 for i = 1:(np+1)]
        nGrid = zeros(Int, np * degree)
        for i = 1:np
            a, b = panel[i], panel[i+1]
            nGrid[(i-1)*degree+1:i*degree] = Freq2Index(isFermi, a .+ (b - a) .* xc)
        end
        unique!(nGrid)
        return symmetry == :none ? vcat(-nGrid[end:-1:2], nGrid) : nGrid
    end

    ωGrid = (ω isa CompositeChebyshevGrid) ? ω.grid : ω
    symmetry = dlrGrid.symmetry
    n = nGrid(dlrGrid.isFermi, symmetry, dlrGrid.Λ)

    nkernelFermi = Spectral.kernelΩ(Val(true), Val(symmetry), n, Float64.(ωGrid), 1.0, true)
    nkernelBose = Spectral.kernelΩ(Val(false), Val(symmetry), n, Float64.(ωGrid), 1.0, true)

    return n, nkernelFermi, nkernelBose
end