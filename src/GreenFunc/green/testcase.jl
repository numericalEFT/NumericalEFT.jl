
function semicircle!(g::MeshArray; dim::Union{Int,Nothing}=nothing, rtol=1e-12, sym=:none)
    if isnothing(dim)
        dim = findfirst(x -> (x isa MeshGrids.ImFreq || x isa MeshGrids.ImTime), g.mesh)
        @assert isnothing(dim) == false "No temporal can be transformed to imtime."
    end
    mesh = g.mesh[dim]
    @assert mesh isa MeshGrids.ImFreq || mesh isa MeshGrids.ImTime "Only imfreq or imtime can be initialized with semicircle spectral density."

    isFermi = mesh.isFermi
    type = mesh isa MeshGrids.ImFreq ? :n : :τ
    gs = Sample.SemiCircle(mesh.Euv, mesh.β, isFermi, mesh.grid, type, sym; rtol=rtol, degree=24, regularized=false)
    for ind in eachindex(g) # ind is a CartesianIndex, ind[dim] gives the index in the dimension dim
        g[ind] = gs[ind[dim]]
    end
end

function multipole!(g::MeshArray, poles::AbstractVector; dim::Union{Int,Nothing}=nothing, rtol=1e-12, sym=:none)
    if isnothing(dim)
        dim = findfirst(x -> (x isa MeshGrids.ImFreq || x isa MeshGrids.ImTime), g.mesh)
        @assert isnothing(dim) == false "No temporal can be transformed to imtime."
    end
    mesh = g.mesh[dim]
    @assert mesh isa MeshGrids.ImFreq || mesh isa MeshGrids.ImTime "Only imfreq or imtime can be initialized with semicircle spectral density."

    isFermi = mesh.isFermi
    type = mesh isa MeshGrids.ImFreq ? :n : :τ
    gs = Sample.MultiPole(mesh.β, isFermi, mesh.grid, type, poles, sym; regularized=false)
    for ind in eachindex(g) # ind is a CartesianIndex, ind[dim] gives the index in the dimension dim
        g[ind] = gs[ind[dim]]
    end
end