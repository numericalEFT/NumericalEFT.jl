@generated function replace_mesh_tuple(mesh::MT, N::Int, dim::Int, mesh_new::M) where {MT,M}
    if dim == 1
        m = :(mesh_new)
        for i in 2:N
            m = :($m, mesh[$i])
        end
    else
        m = :(mesh[1])
        for i in 2:N
            m = i == dim ? :($m, mesh_new) : :($m, mesh[$i])
        end
    end
    return :($m)
end

@generated function _replace_mesh(meshes::MT, mesh_old::OldM, mesh_new::NewM) where {MT,OldM,NewM}
    # return a new mesh tuple where mesh_old is replaced by mesh_new, if not found, the original meshes will be returned

    types = fieldtypes(MT)
    if types[1] == OldM
        m = :(mesh_new,)
    else
        m = :(meshes[1],)
    end

    for (i, t) in enumerate(types)
        # Core.println(t, ", ", M)
        if i <= 1
            continue
        else
            if t == OldM
                m = :($m..., mesh_new)
            else
                m = :($m..., meshes[$i])
            end
        end
    end
    # return :($m)

    # the following will always return a tuple
    if length(types) == 1
        return :($m)
    else
        return :($m)
    end
end

#TODO: we need a version with the type of mesh as the second argument
@generated function _find_mesh(::Type{MT}, ::Type{M}) where {MT,M}
    for (i, t) in enumerate(fieldtypes(MT))
        # type equality is implemented as t<:M and M<:t, 
        # see https://discourse.julialang.org/t/how-to-test-type-equality/14144/6?u=mrbug
        if t <: M
            return :($i)
        end
    end
    return :(0)
end

@generated function _find_mesh(::Type{MT}, ::Type{M1}, ::Type{M2}) where {MT,M1,M2}
    for (i, t) in enumerate(fieldtypes(MT))
        if t <: M1 || t <: M2
            return :($i)
        end
    end
    return :(0)
end

@generated function _find_mesh(::Type{MT}, ::Type{M1}, ::Type{M2}, ::Type{M3}) where {MT,M1,M2,M3}
    for (i, t) in enumerate(fieldtypes(MT))
        if t <: M1 || t <: M2 || t <: M3
            return :($i)
        end
    end
    return :(0)
end

"""
    Base.:<<(objL::MeshArray, objR::MeshArray)

DLR Fourier transform of functions on the first temporal grid (ImTime, ImFreq or DLRFreq). 

- If objL and objR have identical temporal grid, objL<<objR assign objR to objL.
- If objL and objR have different temporal grid, one of them has to be in DLR space.
    * If objL is in DLR space, objL<<objR calculates the DLR spectral density of data in objR
    * if objR is in DLR space, objL<<objR calculates the Green's function from the DLR spectral density in objR.
"""
function Base.:<<(objL::MeshArray{T,N,MT1}, objR::MeshArray{T,N,MT2}) where {T,N,MT1,MT2}
    dimL = _find_mesh(MT1, ImTime, ImFreq, DLRFreq)
    dimR = _find_mesh(MT2, ImTime, ImFreq, DLRFreq)
    @assert dimL == dimR "The temporal dimensions should be identical for source and target MeshArrays."

    typeL = typeof(objL.mesh[dimL])
    typeR = typeof(objR.mesh[dimR])
    meshL = objL.mesh[dimL]
    meshR = objR.mesh[dimR]

    objL.data .= _transform(objR.data, meshL, meshR, dimR)
end

_transform(data, meshL, meshR, axis) = error("One of the Grren's function has to be in DLRfreq space to do Fourier transform")
_transform(data, meshL::T, meshR::T, axis) where {T} = error("ImTime to ImTime or ImFreq to ImFreq is not supported yet!")
_transform(data, meshL::DLRFreq, meshR::ImFreq, axis) = matfreq2dlr(meshL.dlr, data, meshR.grid; axis=axis)
_transform(data, meshL::DLRFreq, meshR::ImTime, axis) = tau2dlr(meshL.dlr, data, meshR.grid; axis=axis)
_transform(data, meshL::ImFreq, meshR::DLRFreq, axis) = dlr2matfreq(meshR.dlr, data, meshL.grid; axis=axis)
_transform(data, meshL::ImTime, meshR::DLRFreq, axis) = dlr2tau(meshR.dlr, data, meshL.grid; axis=axis)


"""
    function dlr_to_imfreq(mesharray[, tgrid; dim])

Transform a Green's function in DLR to the imaginary-time domain. 
#Arguements
- 'mesharray': MeshArray in DLR space
- `tgrid`: The imaginary-time grid which the function transforms into. Default value is the imaginary-time grid from the `DLRGrid` from `mesharray.mesh[dim]`.
- `dim`: The dimension of the temporal mesh. Default value is the first ImTime mesh.
"""
function dlr_to_imtime(obj::MeshArray{T,N,MT},
    tgrid::Union{Nothing,AbstractGrid,AbstractVector,ImFreq}=nothing;
    dim::Int=_find_mesh(MT, DLRFreq)
) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to imtime."
    @assert dim <= N "Dimension must be <= $N."

    mesh = obj.mesh[dim]
    @assert mesh isa MeshGrids.DLRFreq "DLRFreq is expect for the dim = $dim."

    if isnothing(tgrid)
        tgrid = ImTime(mesh)
    elseif tgrid isa ImTime
        @assert tgrid.β ≈ mesh.β "Target grid has to have the same inverse temperature as the source grid."
        @assert tgrid.isFermi ≈ mesh.isFermi "Target grid has to have the same statistics as the source grid."
        # @assert ngrid.Euv ≈ mesh.Euv "Target grid has to have the same Euv as the source grid."
    else
        tgrid = ImTime(mesh, grid=tgrid)
    end

    mesh_new = _replace_mesh(obj.mesh, mesh, tgrid)
    # mesh_new = (obj.mesh[1:dim-1]..., tgrid, obj.mesh[dim+1:end]...)
    data = dlr2tau(mesh.dlr, obj.data, tgrid.grid; axis=dim)
    return MeshArray(mesh=mesh_new, dtype=eltype(data), data=data)
end

"""
    function dlr_to_imfreq(mesharray[, ngrid; dim])

Transform a Green's function in DLR to Matsubara frequency domain. 
#Arguements
- 'mesharray': MeshArray in DLR space
- `ngrid`: The Matsubara-frequency grid which the function transforms into. Default value is the Matsubara-frequency grid from the `DLRGrid` from `mesharray.mesh[dim]`.
- `dim`: The dimension of the temporal mesh. Default value is the first ImFreq mesh.
"""
# function dlr_to_imfreq(obj::MeshArray{T,N,MT}, ngrid=nothing; dim::Union{Nothing,Int}=nothing) where {T,N,MT}
function dlr_to_imfreq(obj::MeshArray{T,N,MT},
    ngrid::Union{Nothing,AbstractGrid,AbstractVector,ImFreq}=nothing;
    dim::Int=_find_mesh(MT, DLRFreq)) where {T,N,MT}
    ########################## generic interface #################################
    @assert dim > 0 "No temporal can be transformed to imfreq."
    @assert dim <= N "Dimension must be <= $N."

    mesh = obj.mesh[dim]::MeshGrids.DLRFreq
    @assert mesh isa MeshGrids.DLRFreq "DLRFreq is expect for the dim = $dim."

    if isnothing(ngrid)
        ngrid = ImFreq(mesh)
    elseif ngrid isa MeshGrids.ImFreq
        @assert ngrid.β ≈ mesh.β "Target grid has to have the same inverse temperature as the source grid."
        @assert ngrid.isFermi ≈ mesh.isFermi "Target grid has to have the same statistics as the source grid."
        # @assert ngrid.Euv ≈ mesh.Euv "Target grid has to have the same Euv as the source grid."
    else
        ngrid = MeshGrids.ImFreq(mesh, grid=ngrid)
    end

    mesh_new = _replace_mesh(obj.mesh, mesh, ngrid)
    # mesh_new = (obj.mesh[1:dim-1]..., ngrid, obj.mesh[dim+1:end]...)
    data = dlr2matfreq(mesh.dlr, obj.data, ngrid.grid.grid; axis=dim)
    return MeshArray(mesh=mesh_new, dtype=Base.eltype(data), data=data)
end

"""
    function imfreq_to_dlr(mesharray[; dim])

Calculate the DLR sepctral density of a Matsubara-frequency Green's function.
#Arguements
- 'mesharray': MeshArray in the Matsubara-frequency domain.
- `dim`: The dimension of the mesh to be transformed. Default value is the first dimension with mesh type ImFreq.
"""
function imfreq_to_dlr(obj::MeshArray{T,N,MT}; dim::Int=_find_mesh(MT, ImFreq)) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to dlr."
    @assert dim <= N "Dimension must be <= $N."

    mesh = obj.mesh[dim]
    @assert mesh isa MeshGrids.ImFreq "ImFreq is expect for the dim = $dim."
    isnothing(mesh.representation) && error("`ImFreq representation = $(mesh.representation)` is not a `DLRGrid`")
    dlrgrid = MeshGrids.DLRFreq(mesh.representation)

    mesh_new = _replace_mesh(obj.mesh, mesh, dlrgrid)
    # mesh_new = (obj.mesh[1:dim-1]..., dlrgrid, obj.mesh[dim+1:end]...)
    data = matfreq2dlr(dlrgrid.dlr, obj.data, mesh.grid.grid; axis=dim) # should be mesh.grid.grid here
    return MeshArray(mesh=mesh_new, dtype=eltype(data), data=data)
end

"""
    function imtime_to_dlr(mesharray[; dim])

Calculate the DLR sepctral density of an imaginary-time Green's function.

#Arguements
- 'mesharray': MeshArray in the imaginary-time domain.
- `dim`: The dimension of the mesh to be transformed. Default value is the first dimension with mesh type ImTime.
"""
function imtime_to_dlr(obj::MeshArray{T,N,MT}; dim::Int=_find_mesh(MT, ImTime)) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to dlr."
    @assert dim <= N "Dimension must be <= $N."

    mesh = obj.mesh[dim]
    @assert mesh isa MeshGrids.ImTime "ImTime is expect for the dim = $dim."
    isnothing(mesh.representation) && error("`ImFreq representation = $(mesh.representation)` is not a `DLRGrid`")
    dlrgrid = MeshGrids.DLRFreq(mesh.representation)

    mesh_new = _replace_mesh(obj.mesh, mesh, dlrgrid)
    # mesh_new = (obj.mesh[1:dim-1]..., dlrgrid, obj.mesh[dim+1:end]...)

    # println(mesh_new)
    data = tau2dlr(dlrgrid.dlr, obj.data, mesh.grid.grid; axis=dim)
    return MeshArray(mesh=mesh_new, dtype=eltype(data), data=data)
end

"""
    function to_dlr(mesharray[; dim])

Calculate the DLR sepctral density of an imaginary-time or Matsubara-frequency Green's function.

#Arguements
- 'mesharray': MeshArray in the imaginary-time or the Matsubara-frequency domain.
- `dim`: The dimension of the mesh to be transformed. Default value is the first dimension with mesh type ImTime or ImFreq.
"""
function to_dlr(obj::MeshArray{T,N,MT};
    dim::Int=_find_mesh(MT, ImTime, ImFreq)
) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to imtime."
    @assert dim <= N "Dimension must be <= $N."

    if obj.mesh[dim] isa MeshGrids.ImTime
        return imtime_to_dlr(obj; dim=dim)
    elseif obj.mesh[dim] isa MeshGrids.ImFreq
        return imfreq_to_dlr(obj; dim=dim)
    else
        error("ImTime or ImFreq is expect for the dim = $dim.")
    end
end

"""
    function to_imtime(mesharray[; dim])

Transform a Green's function to the imaginary-time domain.

#Arguements
- 'mesharray': MeshArray in the imaginary-time, the Matsubara-frequency or the DLR frequency domain.
- `dim`: The dimension of the mesh to be transformed. Default value is the first dimension with mesh type DLRFreq, ImTime or ImFreq.
"""
function to_imtime(obj::MeshArray{T,N,MT};
    dim::Int=_find_mesh(MT, ImTime, ImFreq, DLRFreq)
) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to imtime."
    @assert dim <= N "Dimension must be <= $N."

    if obj.mesh[dim] isa DLRFreq
        return dlr_to_imtime(obj; dim=dim)
    else
        error("Direct ImTime to ImTime or ImTime to ImFreq transform is not supported right now.")
    end
end

"""
    function to_imfreq(mesharray[; dim])

Transform a Green's function to the Matsubara-frequency domain.

#Arguements
- 'mesharray': MeshArray in the imaginary-time, the Matsubara-frequency or the DLR frequency domain.
- `dim`: The dimension of the mesh to be transformed. Default value is the first dimension with mesh type DLRFreq, ImTime or ImFreq.
"""
function to_imfreq(obj::MeshArray{T,N,MT};
    dim::Int=_find_mesh(MT, ImTime, ImFreq, DLRFreq)
) where {T,N,MT}
    @assert dim > 0 "No temporal can be transformed to imtime."
    @assert dim <= N "Dimension must be <= $N."

    if obj.mesh[dim] isa DLRFreq
        return dlr_to_imfreq(obj; dim=dim)
    else
        error("Direct ImTime to ImTime or ImTime to ImFreq transform is not supported right now.")
    end
end

# function Base.:<<(Obj::MeshArray, objSrc::Expr)
#     # more general version needed
#     for (id, d) in enumerate(Obj)
#         inds = ind2sub_gen(size(Obj), id)
#         p, ωn, n, τ = NaN, NaN, NaN, NaN
#         G = d
#         β = Obj.β
#         if Obj.domain == ImFreq
#             n = Obj.tgrid[inds[3]]
#             if Obj.isFermi
#                 ωn = π * (2 * n + 1) / β
#             else
#                 ωn = π * 2 * n * β
#             end
#         elseif Obj.domain == ImTime
#             τ = tgrid[inds[3]]
#         end
#         p = Obj.mesh[inds[2]]

#         m = Dict(
#             :G => G,
#             :p => p,
#             :ωn => ωn,
#             :n => n,
#             :τ => τ,
#             :β => β
#         )
#         Obj[id] = DictParser.evalwithdict(objSrc, m)
#     end

#     return nothing
# end

# Return the single-particle density matrix of the Green's function `obj`.
# """
# function density(obj::MeshArray; kwargs...)
#     G_ins = toTau(obj, [obj.β,]).data .* (-1)
#     return selectdim(G_ins, ndims(G_ins), 1)
# end

#rMatrix transform of the target space of a matrix valued Greens function.
#Sets the current Greens function :math:`g_{ab}` to the matrix transform of :math:`G_{cd}`
#using the left and right transform matrices :math:`L_{ac}` and :math:`R_{db}`.
#.. math::
#g_{ab} = \sum_{cd} L_{ac} G_{cd} R_{db}


# """
#     def from_L_G_R(self, L, G, R):
#         Parameters
#         ----------
#         L : (a, c) ndarray
#             Left side transform matrix.
#         G : Gf matrix valued target_shape == (c, d)
#             Greens function to transform.
#         R : (d, b) ndarray
#             Right side transform matrix.
#         Notes
#         -----
#         Only implemented for Greens functions with a single mesh.


#         assert self.rank == 1, "Only implemented for Greens functions with one mesh"
#         assert self.target_rank == 2, "Matrix transform only valid for matrix valued Greens functions"

#         assert len(L.shape) == 2, "L needs to be two dimensional"
#         assert len(R.shape) == 2, "R needs to be two dimensional"

#         assert L.shape[1] == G.target_shape[0], "Dimension mismatch between L and G"
#         assert R.shape[0] == G.target_shape[1], "Dimension mismatch between G and R"

#         assert L.shape[0] == self.target_shape[0], "Dimension mismatch between L and self"
#         assert R.shape[1] == self.target_shape[1], "Dimension mismatch between R and self"

#         if not L.strides == sorted(L.strides):
#             L = L.copy(order='C')

#         if not R.strides == sorted(R.strides):
#             R = R.copy(order='C')

#         wrapped_aux.set_from_gf_data_mul_LR(self.data, L, G.data, R)
# """
# function from_L_G_R(self,L,G::MeshArray,R)
#     return 1
# end