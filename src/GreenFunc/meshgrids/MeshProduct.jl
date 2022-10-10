"""
Cartisian product of 1 dimensional meshes 
"""

"""
The cartesian Mesh product:

#Parameters:
- 'MT': Type of meshes 
- 'N' : Number of meshes

#Members:
- 'meshes' : The list of Meshes in the MeshProduct
- 'dims' : A tuple of the length of the mesh factors
"""
struct MeshProduct{MT,N}
    meshes::MT
    dims::NTuple{N,Int}
    function MeshProduct(vargs...)
        #@assert all(v -> (v isa Mesh), vargs) "all arguments should variables"
        mprod = Tuple(v for v in vargs)
        mnew = new{typeof(mprod),length(mprod)}(mprod, Tuple(length(v) for v in vargs))
        return mnew
    end
end


"""
    function Base.length(obj::MeshProduct)
Return the number of grids of the MeshProduct.
"""
Base.length(obj::MeshProduct) = reduce(*, obj.dims)

"""
    function Base.size(obj::MeshProduct, I::Int)
Return the length of the specifict Ith mesh factor of the MeshProduct.
"""
Base.size(obj::MeshProduct, I::Int) = obj.dims[I]

"""
    function Base.size(obj::MeshProduct, I::Int)
Return the length of the specifict Ith mesh factor of the MeshProduct.
"""
Base.size(obj::MeshProduct) = obj.dims

# """
#     rank(obj::MeshProduct{MT,N})
# Return the number of the factor meshes.
# """
# rank(obj::MeshProduct{MT,N}) where {MT,N} = N

"""
    function index_to_linear(obj::MeshProduct, index...)
Convert a tuple of the indexes of each mesh to a single linear index of the MeshProduct.

# Argument:
- 'obj': The MeshProduct object
- 'index...': N indexes of the mesh factor, where N is the number of mesh factor
"""
@generated function index_to_linear(obj::MeshProduct{MT,N}, I...) where {MT,N}
    ex = :(I[$N] - 1)
    for i = (N-1):-1:1
        ex = :(I[$i] - 1 + obj.dims[$i] * $ex)
    end
    return :($ex + 1)
end
# function index_to_linear(obj::MeshProduct, index...)
#     bn = Tuple((prod(size(obj)[1:n-1]) for (n, sz) in enumerate(size(obj))))
#     li = 1
#     for (i, id) in enumerate(index)
#         li = li + (id - 1) * bn[i]
#     end
#     return li
# end

"""
    function linear_to_index(obj::MeshProduct, I::Int)
Convert the single linear index of the MeshProduct to a tuple of indexes of each mesh. 

# Argument:
- 'obj': The MeshProduct object
- 'I': The linear index of the MeshProduct 
"""
@generated function linear_to_index(obj::MeshProduct{MT,N}, I::Int) where {MT,N}
    inds, quotient = :((I - 1) % obj.dims[1] + 1), :((I - 1) ÷ obj.dims[1])
    for i = 2:N-1
        inds, quotient = :($inds..., $quotient % obj.dims[$i] + 1), :($quotient ÷ obj.dims[$i])
    end
    inds = :($inds..., $quotient + 1)
    return :($inds)
end

# function linear_to_index(obj::MeshProduct, I::Int)
#     d = rank(obj)
#     bn = reverse(Tuple(prod(size(obj)[1:n-1]) for (n, sz) in enumerate(size(obj))))
#     index = zeros(Int32, d)
#     index[1] = (I - 1) ÷ bn[1] + 1
#     for k in 2:d
#         index[k] = ((I - 1) % bn[k-1]) ÷ bn[k] + 1
#     end
#     return Tuple(reverse(index))
# end


#TODO:for all n meshes in meshes, return [..., (meshes[i])[index[i]], ...] 
# Base.getindex(obj::MeshProduct, index...) = Tuple(obj.meshes[i][id] for (i, id) in enumerate(index))
# Base.getindex(obj::MeshProduct, index...) = Tuple(m[index[i]] for (i, m) in enumerate(obj.meshes))

"""
    function Base.getindex(mp::MeshProduct, index...)

Get a mesh point of the MeshProduct at the given index. Return a tuple as `(mp.meshes[1], mp.meshes[2], ...)`.
"""
# use generated function to make sure the return type is Tuple{eltype(obj.meshes[1]), eltype(obj.meshes[2]), ...}
@generated function Base.getindex(obj::MeshProduct{MT,N}, index...) where {MT,N}
    m = :(obj.meshes[1][index[1]])
    for i in 2:N
        m = :(($m, obj.meshes[$i][index[$i]]))
    end
    return :($m)
end
Base.getindex(obj::MeshProduct, I::Int) = Base.getindex(obj, linear_to_index(obj, I)...)
# return Tuple(obj.meshes[i][id] for (i, id) in enumerate(index))
# return Base.getindex(obj.meshes, I)

#TODO:return the sliced pieces of 
# function Base.view(obj::MeshProduct,inds...)
#     return Tuple(view(obj, i) for i in inds)
#     #return 1
# end
# t[1] --> view of the first mesh

# Check https://docs.julialang.org/en/v1/manual/interfaces/ for details on how to implement the following functions:
Base.firstindex(obj::MeshProduct) = 1
Base.lastindex(obj::MeshProduct) = length(obj)
# iterator
Base.iterate(obj::MeshProduct) = (obj[1], 1)
Base.iterate(obj::MeshProduct, state) = (state >= length(obj)) ? nothing : (obj[state+1], state + 1)
# Base.IteratorSize(obj)
Base.IteratorSize(::Type{MeshProduct{MT,N}}) where {MT,N} = Base.HasLength()
Base.IteratorEltype(::Type{MeshProduct{MT,N}}) where {MT,N} = Base.HasEltype()
Base.eltype(::Type{MeshProduct{MT,N}}) where {MT,N} = tuple(eltype.(fieldtypes(MT))...) # fieldtypes (typeof(mesh1), typeoof(mesh2), ...)

"""
    function Base.show(io::IO, obj::MeshProduct)
Print the MeshProduct.
"""
Base.show(io::IO, obj::MeshProduct) = print(io, "MeshProduct of: $(obj.meshes)")


"""
 All meshes in meshes should have locate and volume functions. Here in meshproduct we just delegate these functions to the meshes, and return the proper array of returned values.
"""
# function locate(obj::MeshProduct, index...)
#     return Tuple(locate(obj, index[mi]) for (mi, m) in enumerate(obj))
# end

@generated function locate(obj::MeshProduct{MT,N}, pos...) where {MT,N}
    m = :(locate(obj.meshes[1], pos[1]))
    for i in 2:N
        m = :($m, locate(obj.meshes[$i], pos[$i]))
    end
    return :($m)
end

function volume(obj::MeshProduct, index...)
    return reduce(*, volume(m, index[mi]) for (mi, m) in enumerate(obj.meshes))
end

volume(obj::MeshProduct, I::Int) = volume(obj, linear_to_index(obj, I)...)
#Note: this should be implemented to obtain the total volume
volume(obj::MeshProduct) = reduce(*, volume(m) for m in obj.meshes)

# locate(m::AbstractMesh, pos) = BZMeshes.BaseMesh.locate(m, pos)
# volume(m::AbstractMesh, index) = BZMeshes.BaseMesh.locate(m, index)
