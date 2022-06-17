function _tensor2matrix(tensor, axis)
    # internal function to move the axis dim to the first index, then reshape the tensor into a matrix
    dim = length(size(tensor))
    n1 = size(tensor)[axis]
    partialsize = deleteat!(collect(size(tensor)), axis) # the size of the tensor except the axis-th dimension
    n2 = reduce(*, partialsize)

    if axis == 1 #no need to permutate the axis
        return reshape(tensor, (n1, n2)), partialsize
    elseif axis == 2 && dim == 2 #for matrix, simply transpose, no copy is created
        return transpose(tensor), partialsize
    else
        permu = [i for i = 1:dim]
        permu[1], permu[axis] = axis, 1
        partialsize = collect(size(tensor)[permu][2:end])
        ntensor = permutedims(tensor, permu) # permutate the axis-th and the 1st dim, a copy of the tensor is created 
        # ntensor = nocopy ? PermutedDimsArray(tensor, permu) : permutedims(tensor, permu) # permutate the axis-th and the 1st dim
        ntensor = reshape(ntensor, (n1, n2)) # no copy is created
        return ntensor, partialsize
    end
end

function _matrix2tensor(mat, partialsize, axis)
    # internal function to reshape matrix to a tensor, then swap the first index with the axis-th dimension
    @assert size(mat)[2] == reduce(*, partialsize) # total number of elements of mat and the tensor must match
    tsize = vcat(size(mat)[1], partialsize)
    tensor = reshape(mat, Tuple(tsize))
    dim = length(partialsize) + 1

    if axis == 1
        return tensor
    elseif axis == 2 && dim == 2
        return transpose(tensor) #transpose do not create copy
    else
        permu = [i for i = 1:dim]
        permu[1], permu[axis] = axis, 1
        return permutedims(tensor, permu) # permutate the axis-th and the 1st dim, a copy of the tensor is created
        # ntensor = nocopy ? PermutedDimsArray(tensor, permu) : permutedims(tensor, permu) # permutate the axis-th and the 1st dim
        # return ntensor
    end
end

# function _weightedLeastSqureFit(dlr, Gτ, error, kernel, sumrule)
#     # Gτ: (Nτ, N), kernel: (Nτ, Nω)
#     # error: (Nτ, N), sumrule: (N, 1)
#     Nτ, Nω = size(kernel)
#     @assert size(Gτ)[1] == Nτ
#     N = size(Gτ)[2]
#     if isnothing(sumrule) == false
#         @assert dlr.symmetry == :none && dlr.isFermi "only unsymmetrized ferminoic sum rule has been implemented!"
#         # println(size(Gτ))
#         # M = Int(floor(dlr.size / 2))
#         M = dlr.size

#         # kernel = kernel[:, 1:Nω-1] #a copy of kernel submatrix will be created
#         kernelN = kernel[:, M]

#         # sign = dlr.isFermi ? -1 : 1
#         # ker0 = Spectral.kernelT(Val(dlr.isFermi), Val(dlr.symmetry), [0.0,], dlr.ω, dlr.β, true)
#         # kerβ = Spectral.kernelT(Val(dlr.isFermi), Val(dlr.symmetry), [dlr.β,], dlr.ω, dlr.β, true)

#         # ker = ker0[1:end] .- sign .* kerβ[1:end]
#         # ker = vcat(ker[1:M-1], ker[M+1:end])
#         # kerN = ker[M]

#         for i in 1:Nτ
#             # Gτ[i, :] .-= kernelN[i] * sumrule / kerN
#             Gτ[i, :] .-= kernelN[i] * sumrule
#         end

#         # for i = 1:Nω-1
#         #     kernel[:, i] .-= kernelN * ker[i] / kerN
#         # end
#         kernel = hcat(kernel[:, 1:M-1], kernel[:, M+1:end])
#     end

#     if isnothing(error)
#         B = kernel
#         C = Gτ
#     else
#         @assert size(error) == size(Gτ)
#         w = 1.0 ./ (error .+ 1e-16)

#         for i = 1:Nτ
#             w[i, :] /= sum(w[i, :]) / length(w[i, :])
#         end
#         B = w .* kernel
#         C = w .* Gτ
#     end
#     # ker, ipiv, info = LAPACK.getrf!(B) # LU factorization
#     # coeff = LAPACK.getrs!('N', ker, ipiv, C) # LU linear solvor for green=kernel*coeff
#     coeff = B \ C #solve C = B * coeff

#     # println("size", size(coeff), ", rank,", dlr.size, "...,", size(kernel))

#     if isnothing(sumrule) == false
#         #make sure Gτ doesn't get modified after the linear fitting
#         for i in 1:Nτ
#             # Gτ[i, :] .+= kernelN[i] * sumrule / kerN
#             Gτ[i, :] .+= kernelN[i] * sumrule
#         end
#         #add back the coeff that are fixed by the sum rule
#         coeffmore = sumrule' .- sum(coeff, dims = 1)
#         cnew = zeros(eltype(coeff), size(coeff)[1] + 1, size(coeff)[2])
#         cnew[1:M-1, :] = coeff[1:M-1, :]
#         cnew[M+1:end, :] = coeff[M:end, :]
#         cnew[M, :] = coeffmore
#         # for j in 1:N
#         #     cnew[:, j] = sumrule[j]
#         # end
#         # println(ker)
#         # println(coeff)
#         # println(dot(ker, coeff))
#         # cnew[M, 1] = (sumrule - dot(ker, coeff)) / kerN
#         return cnew
#     else
#         return coeff
#     end
# end

function _weightedLeastSqureFit(dlrGrid, Gτ, error, kernel, sumrule)
    Nτ, Nω = size(kernel)
    @assert size(Gτ)[1] == Nτ
    if isnothing(sumrule) == false
        @assert dlrGrid.symmetry == :none && dlrGrid.isFermi "only unsymmetrized ferminoic sum rule has been implemented!"
        # println(size(Gτ))
        M = Int(floor(dlrGrid.size / 2))
        # M = dlrGrid.size

        kernel_m0 = kernel[:, M]
        # kernel = kernel[:, 1:Nω-1] #a copy of kernel submatrix will be created
        kernel = hcat(kernel[:, 1:M-1], kernel[:, M+1:end])

        for i in 1:Nτ
            Gτ[i, :] .-= kernel_m0[i] * sumrule
        end

        for i = 1:Nω-1
            kernel[:, i] .-= kernel_m0
        end
        # kernel = view(kernel, :, 1:Nω-1)
    end

    if isnothing(error)
        B = kernel
        C = Gτ
    else
        @assert size(error) == size(Gτ)
        w = 1.0 ./ (error .+ 1e-16)

        for i = 1:Nτ
            w[i, :] /= sum(w[i, :]) / length(w[i, :])
        end
        B = w .* kernel
        C = w .* Gτ
    end
    # ker, ipiv, info = LAPACK.getrf!(B) # LU factorization
    # coeff = LAPACK.getrs!('N', ker, ipiv, C) # LU linear solvor for green=kernel*coeff
    coeff = B \ C #solve C = B * coeff

    if isnothing(sumrule) == false
        #make sure Gτ doesn't get modified after the linear fitting
        for i in 1:Nτ
            Gτ[i, :] .+= kernel_m0[i] * sumrule
        end
        #add back the coeff that are fixed by the sum rule
        coeffmore = sumrule' .- sum(coeff, dims = 1)
        cnew = zeros(eltype(coeff), size(coeff)[1] + 1, size(coeff)[2])
        cnew[1:M-1, :] = coeff[1:M-1, :]
        cnew[M+1:end, :] = coeff[M:end, :]
        cnew[M, :] = coeffmore
        return cnew
    else
        return coeff
    end
end

"""
function tau2dlr(dlrGrid::DLRGrid, green, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    imaginary-time domain to DLR representation

#Members:
- `dlrGrid`  : DLRGrid struct.
- `green`    : green's function in imaginary-time domain.
- `τGrid`    : the imaginary-time grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : the imaginary-time axis in the data `green`.
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function tau2dlr(dlrGrid::DLRGrid, green, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    @assert length(size(green)) >= axis "dimension of the Green's function should be larger than axis!"
    @assert size(green)[axis] == length(τGrid)
    ωGrid = dlrGrid.ω

    typ = promote_type(eltype(dlrGrid.kernel_τ), eltype(green))

    if length(τGrid) == dlrGrid.size && isapprox(τGrid, dlrGrid.τ; rtol = 1e-14)
        kernel = dlrGrid.kernel_τ
    else
        kernel = Spectral.kernelT(Val(dlrGrid.isFermi), Val(dlrGrid.symmetry), τGrid, ωGrid, dlrGrid.β, true; type = typ)
    end

    if typ != eltype(kernel)
        kernel = convert.(typ, kernel)
    end
    if typ != eltype(green)
        green = convert.(typ, green)
    end

    g, partialsize = _tensor2matrix(green, axis)

    if isnothing(sumrule) == false
        # if dlrGrid.symmetry == :ph || dlrGrid.symmetry == :pha
        #     sumrule = sumrule ./ 2.0
        # end
        if isempty(partialsize) == false
            sumrule = reshape(sumrule, size(g)[2])
        end
    end

    if isnothing(error) == false
        @assert size(error) == size(green)
        error, psize = _tensor2matrix(error, axis)
    end

    coeff = _weightedLeastSqureFit(dlrGrid, g, error, kernel, sumrule)

    if verbose && all(x -> abs(x) < 1e16, coeff) == false
        @warn("Some of the DLR coefficients are larger than 1e16. The quality of DLR fitting could be bad.")
    end

    if isnothing(sumrule) == false
        #check how exact is the sum rule
        coeffsum = sum(coeff, dims = 1) .- sumrule
        if verbose && all(x -> abs(x) < 1000 * dlrGrid.rtol * max(maximum(abs.(green)), 1.0), coeffsum) == false
            @warn("Sumrule error $(maximum(abs.(coeffsum))) is larger than the DLRGrid error threshold.")
        end
    end

    return _matrix2tensor(coeff, partialsize, axis)
end

"""
function dlr2tau(dlrGrid::DLRGrid, dlrcoeff, τGrid = dlrGrid.τ; axis = 1, verbose = true)

    DLR representation to imaginary-time representation

#Members:
- `dlrGrid`  : DLRGrid
- `dlrcoeff` : DLR coefficients
- `τGrid`    : expected fine imaginary-time grids 
- `axis`     : imaginary-time axis in the data `dlrcoeff`
- `verbose`  : true to print warning information
"""
function dlr2tau(dlrGrid::DLRGrid, dlrcoeff, τGrid = dlrGrid.τ; axis = 1, verbose = true)
    @assert length(size(dlrcoeff)) >= axis "dimension of the dlr coefficients should be larger than axis!"
    @assert size(dlrcoeff)[axis] == size(dlrGrid)

    β = dlrGrid.β
    ωGrid = dlrGrid.ω

    if length(τGrid) == dlrGrid.size && isapprox(τGrid, dlrGrid.τ; rtol = 1e-14)
        kernel = dlrGrid.kernel_τ
    else
        kernel = Spectral.kernelT(Val(dlrGrid.isFermi), Val(dlrGrid.symmetry), τGrid, ωGrid, dlrGrid.β, true)
    end

    coeff, partialsize = _tensor2matrix(dlrcoeff, axis)

    G = kernel * coeff # tensor dot product: \sum_i kernel[..., i]*coeff[i, ...]

    return _matrix2tensor(G, partialsize, axis)
end

"""
function matfreq2dlr(dlrGrid::DLRGrid, green, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    Matsubara-frequency representation to DLR representation

#Members:
- `dlrGrid`  : DLRGrid struct.
- `green`    : green's function in Matsubara-frequency domain
- `nGrid`    : the n grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : the Matsubara-frequency axis in the data `green`
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function matfreq2dlr(dlrGrid::DLRGrid, green, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    @assert length(size(green)) >= axis "dimension of the Green's function should be larger than axis!"
    @assert size(green)[axis] == length(nGrid)
    @assert eltype(nGrid) <: Integer
    ωGrid = dlrGrid.ω

    typ = promote_type(eltype(dlrGrid.kernel_n), eltype(green))

    if length(nGrid) == dlrGrid.size && isapprox(nGrid, dlrGrid.n; rtol = 1e-14)
        kernel = dlrGrid.kernel_n
    else
        kernel = Spectral.kernelΩ(Val(dlrGrid.isFermi), Val(dlrGrid.symmetry), nGrid, ωGrid, dlrGrid.β, true; type = typ)
    end

    if typ != eltype(green)
        green = convert.(typ, green)
    end

    if typ != eltype(kernel)
        dlrGrid.kernel_n = convert.(typ, dlrGrid.kernel_n)
    end

    g, partialsize = _tensor2matrix(green, axis)

    if isnothing(sumrule) == false
        # if dlrGrid.symmetry == :ph || dlrGrid.symmetry == :pha
        #     sumrule = sumrule ./ 2.0
        # end
        if isempty(partialsize) == false
            sumrule = reshape(sumrule, size(g)[2])
        end
    end

    if isnothing(error) == false
        @assert size(error) == size(green)
        error, psize = _tensor2matrix(error, axis)
    end
    coeff = _weightedLeastSqureFit(dlrGrid, g, error, kernel, sumrule)
    if verbose && all(x -> abs(x) < 1e16, coeff) == false
        @warn("Some of the DLR coefficients are larger than 1e16. The quality of DLR fitting could be bad.")
    end

    if isnothing(sumrule) == false
        #check how exact is the sum rule
        coeffsum = sum(coeff, dims = 1) .- sumrule
        if verbose && all(x -> abs(x) < 1000 * dlrGrid.rtol * max(maximum(abs.(green)), 1.0), coeffsum) == false
            @warn("Sumrule error $(maximum(abs.(coeffsum))) is larger than the DLRGrid error threshold.")
        end
    end
    return _matrix2tensor(coeff, partialsize, axis)
end

"""
function dlr2matfreq(dlrGrid::DLRGrid, dlrcoeff, nGrid = dlrGrid.n; axis = 1, verbose = true)

    DLR representation to Matsubara-frequency representation

#Members:
- `dlrGrid`  : DLRGrid
- `dlrcoeff` : DLR coefficients
- `nGrid`    : expected fine Matsubara-freqeuncy grids (integer)
- `axis`     : Matsubara-frequency axis in the data `dlrcoeff`
- `verbose`  : true to print warning information
"""
function dlr2matfreq(dlrGrid::DLRGrid, dlrcoeff, nGrid = dlrGrid.n; axis = 1, verbose = true)
    @assert length(size(dlrcoeff)) >= axis "dimension of the dlr coefficients should be larger than axis!"
    @assert size(dlrcoeff)[axis] == size(dlrGrid)
    @assert eltype(nGrid) <: Integer
    ωGrid = dlrGrid.ω

    if length(nGrid) == dlrGrid.size && isapprox(nGrid, dlrGrid.n; rtol = 1e-14)
        kernel = dlrGrid.kernel_n
    else
        kernel = Spectral.kernelΩ(Val(dlrGrid.isFermi), Val(dlrGrid.symmetry), nGrid, ωGrid, dlrGrid.β, true)
    end

    coeff, partialsize = _tensor2matrix(dlrcoeff, axis)

    G = kernel * coeff # tensor dot product: \sum_i kernel[..., i]*coeff[i, ...]

    return _matrix2tensor(G, partialsize, axis)
end

"""
function tau2matfreq(dlrGrid, green, nNewGrid = dlrGrid.n, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    Fourier transform from imaginary-time to Matsubara-frequency using the DLR representation

#Members:
- `dlrGrid`  : DLRGrid
- `green`    : green's function in imaginary-time domain
- `nNewGrid` : expected fine Matsubara-freqeuncy grids (integer)
- `τGrid`    : the imaginary-time grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : the imaginary-time axis in the data `green`
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function tau2matfreq(dlrGrid, green, nNewGrid = dlrGrid.n, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    coeff = tau2dlr(dlrGrid, green, τGrid; error = error, axis = axis, sumrule = sumrule, verbose = verbose)
    return dlr2matfreq(dlrGrid, coeff, nNewGrid, axis = axis, verbose = verbose)
end

"""
function matfreq2tau(dlrGrid, green, τNewGrid = dlrGrid.τ, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    Fourier transform from Matsubara-frequency to imaginary-time using the DLR representation

#Members:
- `dlrGrid`  : DLRGrid
- `green`    : green's function in Matsubara-freqeuncy repsentation
- `τNewGrid` : expected fine imaginary-time grids
- `nGrid`    : the n grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : Matsubara-frequency axis in the data `green`
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function matfreq2tau(dlrGrid, green, τNewGrid = dlrGrid.τ, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    coeff = matfreq2dlr(dlrGrid, green, nGrid; error = error, axis = axis, sumrule = sumrule, verbose = verbose)
    return dlr2tau(dlrGrid, coeff, τNewGrid, axis = axis, verbose = verbose)
end

"""
function tau2tau(dlrGrid, green, τNewGrid, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    Interpolation from the old imaginary-time grid to a new grid using the DLR representation

#Members:
- `dlrGrid`  : DLRGrid
- `green`    : green's function in imaginary-time domain
- `τNewGrid` : expected fine imaginary-time grids
- `τGrid`    : the imaginary-time grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : the imaginary-time axis in the data `green`
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function tau2tau(dlrGrid, green, τNewGrid, τGrid = dlrGrid.τ; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    coeff = tau2dlr(dlrGrid, green, τGrid; error = error, axis = axis, sumrule = sumrule, verbose = verbose)
    return dlr2tau(dlrGrid, coeff, τNewGrid, axis = axis, verbose = verbose)
end

"""
function matfreq2matfreq(dlrGrid, green, nNewGrid, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)

    Fourier transform from Matsubara-frequency to imaginary-time using the DLR representation

#Members:
- `dlrGrid`  : DLRGrid
- `green`    : green's function in Matsubara-freqeuncy repsentation
- `nNewGrid` : expected fine Matsubara-freqeuncy grids (integer)
- `nGrid`    : the n grid that Green's function is defined on. 
- `error`    : error the Green's function. 
- `axis`     : Matsubara-frequency axis in the data `green`
- `sumrule`  : enforce the sum rule 
- `verbose`  : true to print warning information
"""
function matfreq2matfreq(dlrGrid, green, nNewGrid, nGrid = dlrGrid.n; error = nothing, axis = 1, sumrule = nothing, verbose = true)
    coeff = matfreq2dlr(dlrGrid, green, nGrid; error = error, axis = axis, sumrule = sumrule, verbose = verbose)
    return dlr2matfreq(dlrGrid, coeff, nNewGrid, axis = axis, verbose = verbose)
end

# function convolution(dlrGrid, green1, green2; axis = 1)

# end