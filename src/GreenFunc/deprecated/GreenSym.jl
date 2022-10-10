"""
General Green's function.
"""

"""
Symmetrized Green's function with two external legs that has in-built Discrete Lehmann Representation.
The real and imaginary parts are saved separately on corresponding symmetrized DLR grids. 
#Parameters:
- 'T': type of data
- 'TType': type of time domain, TType<:TimeDomain
- 'TGT': type of time grid
- 'SGT': type of space grid

#Members:
- 'name': Name of green's function
- 'color': Number of different species of Green's function (such as different spin values)
- 'dlrGrid': In-built Discrete Lehmann Representation
- 'timeGrid': Time or Frequency grid
- 'spaceType': Whether the Green's function is in coordinate space/momentum space
- 'spaceGrid': Coordinate or momentum grid
- 'instant': Instantaneous part of Green's function that is proportional to δ(τ) in τ space.
- 'dynamic': Dynamic part of Green's function
- 'instantError': Error of instantaneous part
- 'dynamicError': Error of dynamic part
"""
mutable struct GreenSym2DLR{T<:Number,Type<:TimeDomain,TGT,SGT}
    name::Symbol
    # isFermi::Bool
    # β::Float64
    color::Int
    dlrGrid::DLRGrid
    dlrReGrid::DLRGrid
    dlrImGrid::DLRGrid
    #########    Mesh   ##############

    # timeType::DataType
    # timeSymmetry::Symbol
    timeGrid::TGT

    spaceType::Symbol
    spaceGrid::SGT

    ###########     data   ###########
    instant::Array{T,3}
    dynamic::Array{T,4}
    dynamicRe::Array{T,4}
    dynamicIm::Array{T,4}
    """
        function Green2DLR{T}(name::Symbol, timeType::TT, β, isFermi::Bool, Euv, spaceGrid, color::Int = 1;
            timeSymmetry::Symbol = :none, rtol = 1e-8, kwargs...
        ) where {T<:Number, TT<:TimeDomain}

    Create two-leg Green's function on timeGrid, spaceGrid and color, with in-built DLR.

    #Arguements
    - 'name': Name of green's function.
    - 'timeType': type of time domain, TT<:TimeDomain
    - 'β': Inverse temperature
    - 'isFermi': Particle is fermi or boson
    - 'Euv': the UV energy scale of the spectral density
    - 'spaceGrid': k/x grid
    - 'color': Number of different species of Green's function (such as different spin values). Default: 1 color
    - 'timeSymmetry': Whether the Green's function has particle-hole symmetry, anti-particle-hole symmetry or none of them
    - 'rtol': tolerance absolute error
    #Optional Arguments
    - 'timeGrid': τ/n/ω  grid. Default: DLR grid in timeType (:τ/:n/:ω) 
    - 'instant': Instantaneous part of Green's function that is proportional to δ(τ) in τ space. Default: 0 everywhere
    - 'dynamic': Dynamic part of Green's function. Default: 0 everywhere
    - 'instantError': Error of instantaneous part. Default: 0 everywhere
    - 'dynamicError': Error of dynamic part. Default: 0 everywhere
    """
    function GreenSym2DLR{T}(name::Symbol, timeType::TT, β, isFermi::Bool, Euv, spaceGrid, color::Int = 1;
                          timeReSymmetry::Symbol = :pha,  timeImSymmetry::Symbol = :ph, rtol = 1e-8, kwargs...
    ) where {T<:Number, TT<:TimeDomain}
        # @assert spaceType == :k || spaceType == :x
        @assert timeReSymmetry == :ph || timeReSymmetry == :pha
        @assert timeImSymmetry == :ph || timeImSymmetry == :pha

        spaceType = :k #TODO: replace it with spaceGrid.type after we have a spaceGrid package

        dlrGrid = DLRGrid(Euv, β, rtol, isFermi, :none)        
        dlrReGrid = DLRGrid(Euv, β, rtol, isFermi, timeReSymmetry)
        dlrImGrid = DLRGrid(Euv, β, rtol, isFermi, timeImSymmetry)

        # println(keys(kwargs))

        if :timeGrid in keys(kwargs)
            givenTimeGrid = kwargs[:timeGrid]
            if givenTimeGrid isa AbstractVector
                timeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(givenTimeGrid)}(givenTimeGrid)
            elseif givenTimeGrid isa CompositeGrids.AbstractGrid
                timeGrid = givenTimeGrid
            else
                error("Input timeGrid has to be Vector or Composite grid")
            end
        else
            if TT == ImFreq
                bareTimeGrid = dlrGrid.n
            elseif TT == ImTime
                bareTimeGrid = dlrGrid.τ
            elseif TT == DLRFreq
                bareTimeGrid = dlrGrid.ω
            else
                error("$TimeType is not supported!")
            end
            timeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(bareTimeGrid)}(bareTimeGrid)
        end
        #if timeGrid is nothing, then set it to be the DLR grid, which is a vector of Integer or Float64

        if TT == DLRFreq
            #@assert length(timeGrid.grid) == dlrGrid.size "The size of the DLR grid should match the DLR rank = $(dlrGrid.size)."
        elseif TT == ImFreq
            @assert eltype(timeGrid.grid) <: Int "Matsubara frequency grid is expected to be integers!"
        end

        if spaceGrid isa AbstractVector
            compSpaceGrid = CompositeGrids.SimpleG.Arbitrary{eltype(spaceGrid)}(spaceGrid)
        elseif spaceGrid isa CompositeGrids.AbstractGrid
            compSpaceGrid = spaceGrid
        else
            error("Input spaceGrid has to be Vector or Composite grid")
        end

        instant = Array{T,3}(undef, 0, 0, 0)
        dynamic = Array{T,4}(undef, 0, 0, 0, 0)
        dynamicRe = Array{T,4}(undef, 0, 0, 0, 0)
        dynamicIm = Array{T,4}(undef, 0, 0, 0, 0)

        gnew = new{T,TT,typeof(timeGrid),typeof(compSpaceGrid)}(
            name, color, dlrGrid, dlrReGrid,dlrImGrid,
            timeGrid,
            spaceType, compSpaceGrid,
            instant, dynamic, dynamicRe,dynamicIm)
        return set!(gnew; kwargs...)
    end
end

function Base.getproperty(obj::GreenSym2DLR{T,TT,TGT,SGT}, sym::Symbol) where {T,TT,TGT,SGT}
    if sym === :isFermi
        return obj.dlrReGrid.isFermi
    elseif sym === :β
        return obj.dlrReGrid.β
    elseif sym === :timeType
        return TT        
    elseif sym === :timeReSymmetry
        return obj.dlrReGrid.symmetry
    elseif sym === :timeImSymmetry
        return obj.dlrImGrid.symmetry
    else # fallback to getfield
        return getfield(obj, sym)
    end
end

function Base.size(green::GreenSym2DLR)
    return (green.color, green.color, size(green.spaceGrid), size(green.timeGrid), green.dlrGrid.size, green.dlrReGrid.size, green.dlrImGrid.size)
end

function set!(green::GreenSym2DLR; kwargs...)
    size_green = size(green)
    dynamicSize = size_green[1:4]
    dynamicReSize = [size_green[1:3], size_green[6]]
    dynamicImSize = [size_green[1:3], size_green[7]]
    instantSize = size_green[1:3]
    if :dynamic in keys(kwargs) && isempty(kwargs[:dynamic]) == false
        green.dynamic = reshape(kwargs[:dynamic], Tuple(dynamicSize))
        if(green.timeType == ImTime)
            dynamicRe = real.(tau2matfreq(green.dlrGrid, green.dynamic, green.dlrReGrid.n, green.timeGrid.grid; axis = 4))
            dynamicIm = im * imag.(tau2matfreq(green.dlrGrid, green.dynamic, green.dlrImGrid.n, green.timeGrid.grid; axis = 4))            
            green.dynamicRe = matfreq2tau(green.dlrReGrid, dynamicRe, green.dlrReGrid.τ; axis = 4)
            green.dynamicIm = matfreq2tau(green.dlrImGrid, dynamicIm, green.dlrImGrid.τ; axis = 4)
        elseif(green.timeType == ImFreq)
            green.dynamicRe = real.(matfreq2matfreq(green.dlrGrid, green.dynamic, green.dlrReGrid.n, green.timeGrid.grid; axis = 4))
            green.dynamicIm = im * imag.(matfreq2matfreq(green.dlrGrid, green.dynamic, green.dlrImGrid.n, green.timeGrid.grid; axis = 4))
        elseif(green.timeType == DLRFreq)
            dynamicRe = real.(dlr2matfreq(green.dlrGrid, green.dynamic, green.dlrReGrid.n; axis = 4))
            dynamicIm = im * imag.(dlr2matfreq(green.dlrGrid, green.dynamic, green.dlrImGrid.n; axis = 4))            
            green.dynamicRe = matfreq2dlr(green.dlrReGrid, dynamicRe; axis = 4)
            green.dynamicIm = matfreq2dlr(green.dlrImGrid, dynamicIm; axis = 4)            
        end
    elseif :dynamicRe in keys(kwargs) && isempty(kwargs[:dynamicRe]) == false && :dynamicIm in keys(kwargs) && isempty(kwargs[:dynamicIm]) == false
        green.dynamicRe = reshape(kwargs[:dynamicRe], Tuple(dynamicReSize))
        green.dynamicIm = reshape(kwargs[:dynamicIm], Tuple(dynamicImSize))
        if(green.timeType == ImTime)
            green.dynamic = tau2tau(green.dlrReGrid, green.dynamicRe, green.timeGrid.grid; axis = 4)+tau2tau(green.dlrImGrid, green.dynamicIm, green.timeGrid.grid; axis = 4)
        elseif(green.timeType == ImFreq)
            green.dynamic = matfreq2matfreq(green.dlrReGrid, green.dynamicRe, green.timeGrid.grid; axis = 4)+matfreq2matfreq(green.dlrImGrid, green.dynamicIm, green.timeGrid.grid; axis = 4)
        elseif(green.timeType == DLRFreq)
            dynamic = dlr2tau(green.dlrReGrid, green.dynamicRe, green.dlrGrid.τ; axis = 4) + dlr2tau(green.dlrImGrid, green.dynamicIm, green.dlrGrid.τ; axis = 4)
            green.dynamic = tau2dlr(green.dlrGrid, dynamic; axis = 4)
        end
    end


    if :instant in keys(kwargs) && isempty(kwargs[:instant]) == false
        green.instant = reshape(kwargs[:instant], Tuple(instantSize))
    end

    return green
end

"""
    function toTau(green::Green2DLR, targetGrid =  green.dlrGrid.τ)
Convert Green's function to τ space by Fourier transform.
If green is already in τ space then it will be interpolated to the new grid.

#Arguements
- 'green': Original Green's function
- 'targetGrid': Grid of outcome Green's function. Default: DLR τ grid
"""
function toTau(green::GreenSym2DLR, targetGrid = green.dlrGrid.τ)

    if targetGrid isa AbstractVector
        targetGrid = CompositeGrids.SimpleG.Arbitrary{eltype(targetGrid)}(targetGrid)
    end

    # do nothing if the domain and the grid remain the same
    if green.timeType == ImTime && length(green.timeGrid.grid) ≈ length(targetGrid.grid) && green.timeGrid.grid ≈ targetGrid.grid
        return green
    end
    if isempty(green.dynamicRe)||isempty(green.dynamicIm) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImTime)
        dynamicRe = green.dynamicRe
        dynamicIm = green.dynamicIm
        dynamic = tau2tau(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + tau2tau(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)        
    elseif (green.timeType == ImFreq)
        dynamicRe = matfreq2tau(green.dlrReGrid, green.dynamicRe; axis = 4)
        dynamicIm = matfreq2tau(green.dlrImGrid, green.dynamicIm; axis = 4)
        dynamic = matfreq2tau(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + matfreq2tau(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)           
    elseif (green.timeType == DLRFreq)
        dynamicRe = dlr2tau(green.dlrReGrid, green.dynamic; axis = 4)
        dynamicIm = dlr2tau(green.dlrImGrid, green.dynamic; axis = 4)
        dynamic = dlr2tau(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + dlr2tau(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)          
    end

    return GreenSym2DLR{eltype(dynamic)}(
        green.name, IMTIME,green.β, green.isFermi, green.dlrReGrid.Euv, green.spaceGrid, green.color;
        timeReSymmetry = green.timeReSymmetry, timeImSymmetry = green.timeImSymmetry, timeGrid = targetGrid, rtol = green.dlrReGrid.rtol,
        dynamic = dynamic, dynamicRe = dynamicRe, dynamicIm = dynamicIm, instant = green.instant)
end

"""
    function toMatFreq(green::Green2DLR, targetGrid =  green.dlrGrid.n)
Convert Green's function to matfreq space by Fourier transform.
If green is already in matfreq space then it will be interpolated to the new grid.

#Arguements
- 'green': Original Green's function
- 'targetGrid': Grid of outcome Green's function. Default: DLR n grid
"""
function toMatFreq(green::GreenSym2DLR, targetGrid = green.dlrGrid.n)

    if targetGrid isa AbstractVector
        targetGrid = CompositeGrids.SimpleG.Arbitrary{eltype(targetGrid)}(targetGrid)
    end

    # do nothing if the domain and the grid remain the same
    if green.timeType == ImFreq && length(green.timeGrid.grid) ≈ length(targetGrid.grid) && green.timeGrid.grid ≈ targetGrid.grid
        return green
    end
    if isempty(green.dynamicRe)||isempty(green.dynamicIm) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImFreq)
        dynamicRe = green.dynamicRe
        dynamicIm = green.dynamicIm
        dynamic = matfreq2matfreq(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + matfreq2matfreq(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)  
    elseif (green.timeType == ImTime)
        dynamicRe = tau2matfreq(green.dlrReGrid, green.dynamicRe; axis = 4)
        dynamicIm = tau2matfreq(green.dlrImGrid, green.dynamicIm; axis = 4)
        dynamic = tau2matfreq(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + tau2matfreq(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)     
    elseif (green.timeType == DLRFreq)
        dynamicRe = dlr2matfreq(green.dlrReGrid, green.dynamic; axis = 4)
        dynamicIm = dlr2matfreq(green.dlrImGrid, green.dynamic; axis = 4)
        dynamic = dlr2freq(green.dlrReGrid, green.dynamicRe, targetGrid.grid; axis = 4) + dlr2freq(green.dlrImGrid, green.dynamicIm, targetGrid.grid; axis = 4)   
    end

    return GreenSym2DLR{eltype(dynamic)}(
        green.name, IMFREQ, green.β, green.isFermi, green.dlrGrid.Euv, green.spaceGrid, green.color;
        timeReSymmetry = green.timeReSymmetry, timeImSymmetry = green.timeImSymmetry, timeGrid = targetGrid, rtol = green.dlrReGrid.rtol,
        dynamic = dynamic, dynamicRe = dynamicRe, dynamicIm = dynamicIm, instant = green.instant)

end

"""
    function toDLR(green::Green2DLR)
Convert Green's function to dlr space.

#Arguements
- 'green': Original Green's function
"""
function toDLR(green::GreenSym2DLR)

    # do nothing if the domain and the grid remain the same
    if green.timeType == DLRFreq
        return green
    end
    if isempty(green.dynamicRe)||isempty(green.dynamicIm) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImTime)
        dynamicRe = tau2dlr(green.dlrReGrid, green.dynamicRe; axis = 4)
        dynamicIm = tau2dlr(green.dlrImGrid, green.dynamicIm; axis = 4)
        dynamic = tau2dlr(green.dlrGrid, green.dynamic,  green.timeGrid.grid; axis = 4)
    elseif (green.timeType == ImFreq)
        dynamicRe = matfreq2dlr(green.dlrReGrid, green.dynamicRe; axis = 4)
        dynamicIm = matfreq2dlr(green.dlrImGrid, green.dynamicIm; axis = 4)        
        dynamic = matfreq2dlr(green.dlrGrid, green.dynamic,  green.timeGrid.grid; axis = 4)
    end

    return Green2DLR{eltype(dynamic)}(
        green.name,DLRFREQ, green.β, green.isFermi, green.dlrGrid.Euv, green.spaceGrid, green.color;
        timeReSymmetry = green.timeReSymmetry, timeImSymmetry = green.timeImSymmetry, timeGrid = green.dlrGrid.ω, rtol = green.dlrGrid.rtol,
        dynamic = dynamic,  dynamicRe = dynamicRe, dynamicIm = dynamicIm, instant = green.instant)

end


"""
    function instant(green::Green2DLR{DT,TT,TGT,SGT}, space, color1::Int, color2::Int=color1; spaceMethod::SM = DEFAULTINTERP) where {DT,TT,TGT,SGT,SM}

Find value of Green's function's instant part at given color and k/x by interpolation.
Interpolation method is by default depending on the grid, but could also be chosen to be linear.

#Argument
- 'green': Green's function
- 'space': Target k/x point
- 'color1': Target color1
- 'color2': Target color2
- 'spaceMethod': Method of interpolation for space. 
"""
function instant(; green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, space, color1::Int, color2::Int, spaceMethod::SM) where {DT,TT,TGT,SGT,SM}
    if isempty(green.instant)
        error("Instant Green's function can not be empty!")
    else
        IM = InterpMethod(SGT,SM)
        nei = CompositeGrids.Interp.findneighbor(IM, green.spaceGrid,space)
        instant_x = view(green.instant, color1, color2, nei.index)
        return CompositeGrids.Interp.interpsliced(nei,instant_x)
    end
end

function instant(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, space, color1::Int=1, color2::Int=color1, spaceMethod::SM = DEFAULTINTERP) where {DT,TT,TGT,SGT,SM}
    return instant(; green=green, space = space, color1 = color1 , color2 = color2, spaceMethod = spaceMethod)
end

function instant(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, space, spaceMethod) where {DT,TT,TGT,SGT}
    return instant(; green=green, space = space, color1 = 1 , color2 = 1, spaceMethod = spaceMethod)
end


"""
    function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int, color2::Int, timeMethod::TM , spaceMethod::SM) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}

Find value of Green's function's dynamic part at given color and k/x by interpolation.
Interpolation method is by default depending on the grid, but could also be chosen to be linear.

#Argument
- 'green': Green's function
- 'time': Target τ/ω_n point
- 'space': Target k/x point
- 'color1': Target color1
- 'color2': Target color2
- 'timeMethod': Method of interpolation for time
- 'spaceMethod': Method of interpolation for space 
"""
function _dynamic(TIM, SIM; green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int, color2::Int) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
    # for double composite
    if isempty(green.dynamic)
        error("Dynamic Green's function can not be empty!")
    else
        spaceNeighbor = CompositeGrids.Interp.findneighbor(SIM, green.spaceGrid, space)
        #println(TIM)
        if green.timeType == ImFreq && TIM != DLRInterp
            timeGrid = (green.timeGrid.grid * 2 .+ 1) * π / green.β
            comTimeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(timeGrid)}(timeGrid)
            comTime = (2*time+1)*π/green.β
        else
            comTimeGrid = green.timeGrid
            comTime = time
        end

        timeNeighbor = CompositeGrids.Interp.findneighbor(TIM, comTimeGrid, comTime)
        dynamic_slice = view(green.dynamic, color1, color2, spaceNeighbor.index, timeNeighbor.index)
        dynamic_slice_xint = CompositeGrids.Interp.interpsliced(spaceNeighbor,dynamic_slice, axis=1)
        result = CompositeGrids.Interp.interpsliced(timeNeighbor,dynamic_slice_xint, axis=1)
    end
    return result
end

function _dynamic( ::LinearInterp , ::LinearInterp
    ;green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int, color2::Int,
    ) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
    # for double composite and double linear
    if isempty(green.dynamic)
        error("Dynamic Green's function can not be empty!")
    else
        if green.timeType == ImFreq
            timeGrid = (green.timeGrid.grid * 2 .+ 1) * π / green.β
            comTimeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(timeGrid)}(timeGrid)            
            comTime = (2*time+1)*π/green.β
        else
            comTimeGrid = green.timeGrid
            comTime = time
        end
        dynamic_slice = view(green.dynamic, color1, color2, :,:)
        result = CompositeGrids.Interp.linear2D(dynamic_slice, green.spaceGrid, comTimeGrid,space,comTime)
    end
    return result
end


function _dynamic(::DLRInterp, SIM; green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int, color2::Int) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
    # for composite space and dlr time
    if isempty(green.dynamic)
        error("Dynamic Green's function can not be empty!")
    else
        spaceNeighbor = CompositeGrids.Interp.findneighbor(SIM, green.spaceGrid, space)
        dynamic_slice = view(green.dynamic, color1, color2, spaceNeighbor.index,:)
        dynamic_slice_xint = CompositeGrids.Interp.interpsliced(spaceNeighbor,dynamic_slice, axis=1)
        if green.timeType == ImFreq
            result = (matfreq2matfreq(green.dlrGrid, dynamic_slice_xint, [time,], green.timeGrid.grid))[1]
        elseif green.timeType == ImTime
            result = (tau2tau(green.dlrGrid, dynamic_slice_xint, [time,], green.timeGrid.grid))[1]
        end
    end
    return result
end

dynamic(;timeMethod::TM, spaceMethod::SM ,green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space , color1::Int, color2::Int) where {TM,SM,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,DT,TT} = _dynamic(InterpMethod(TGT,TM), InterpMethod(SGT, SM); green, time, space, color1, color2)

function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int=1, color2::Int=color1, timeMethod::TM=DEFAULTINTERP , spaceMethod::SM=DEFAULTINTERP) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}
    return  dynamic(; timeMethod = timeMethod, spaceMethod = spaceMethod, green = green, time=time, space=space, color1=color1, color2 =color2)
end

function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, timeMethod::TM, spaceMethod::SM) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}
    return  dynamic(; timeMethod = timeMethod, spaceMethod = spaceMethod, green = green, time=time, space=space, color1=1, color2 =1)
end

# function _dynamic(TIM, SIM; green::GreenSym2DLR{DT,TT,TGT,SGT}, time, space, color1::Int, color2::Int) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
#     # for double composite
#     if isempty(green.dynamic)
#         error("Dynamic Green's function can not be empty!")
#     else
#         spaceNeighbor = CompositeGrids.Interp.findneighbor(SIM, green.spaceGrid, space)
#         println(TIM)
#         if green.timeType == ImFreq && TIM != DLRInterp
#             timeGrid = (green.timeGrid.grid * 2 .+ 1) * π / green.β
#             comTimeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(timeGrid)}(timeGrid)
#             comTime = (2*time+1)*π/green.β
#         else
#             comTimeGrid = green.timeGrid
#             comTime = time
#         end

#         timeNeighbor = CompositeGrids.Interp.findneighbor(TIM, comTimeGrid, comTime)
#         dynamic_slice = view(green.dynamic, color1, color2, spaceNeighbor.index, timeNeighbor.index)
#         dynamic_slice_xint = CompositeGrids.Interp.interpsliced(spaceNeighbor,dynamic_slice, axis=1)
#         result = CompositeGrids.Interp.interpsliced(timeNeighbor,dynamic_slice_xint, axis=1)
#     end
#     return result
# end

# function _dynamic( ::LinearInterp , ::LinearInterp
#     ;green::GreenSym2DLR{DT,TT,TGT,SGT}, time, space, color1::Int, color2::Int,
#     ) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
#     # for double composite and double linear
#     if isempty(green.dynamic)
#         error("Dynamic Green's function can not be empty!")
#     else
#         if green.timeType == ImFreq
#             timeGrid = (green.timeGrid.grid * 2 .+ 1) * π / green.β
#             comTimeGrid = CompositeGrids.SimpleG.Arbitrary{eltype(timeGrid)}(timeGrid)            
#             comTime = (2*time+1)*π/green.β
#         else
#             comTimeGrid = green.timeGrid
#             comTime = time
#         end
#         dynamic_slice = view(green.dynamic, color1, color2, :,:)
#         result = CompositeGrids.Interp.linear2D(dynamic_slice, green.spaceGrid, comTimeGrid,space,comTime)
#     end
#     return result
# end


# function _dynamic(::DLRInterp, SIM; green::GreenSym2DLR{DT,TT,TGT,SGT}, time, space, color1::Int, color2::Int) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
#     # for composite space and dlr time
#     if isempty(green.dynamic)
#         error("Dynamic Green's function can not be empty!")
#     else
#         spaceNeighbor = CompositeGrids.Interp.findneighbor(SIM, green.spaceGrid, space)
#         dynamic_slice = view(green.dynamic, color1, color2, spaceNeighbor.index,:)
#         dynamic_slice_xint = CompositeGrids.Interp.interpsliced(spaceNeighbor,dynamic_slice, axis=1)
#         if green.timeType == ImFreq
#             result = (matfreq2matfreq(green.dlrGrid, dynamic_slice_xint, [time,], green.timeGrid.grid))[1]
#         elseif green.timeType == ImTime
#             result = (tau2tau(green.dlrGrid, dynamic_slice_xint, [time,], green.timeGrid.grid))[1]
#         end
#     end
#     return result
# end

# dynamic(;timeMethod::TM, spaceMethod::SM ,green::Union{GreenSym2DLR{DT,TT,TGT,SGT}}, time, space , color1::Int, color2::Int) where {TM,SM,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,DT,TT} = _dynamic(InterpMethod(TGT,TM), InterpMethod(SGT, SM); green, time, space, color1, color2)

# function dynamic(green::GreenSym2DLR{DT,TT,TGT,SGT}, time, space, color1::Int=1, color2::Int=color1, timeMethod::TM=DEFAULTINTERP , spaceMethod::SM=DEFAULTINTERP) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}
#     return  dynamic(; timeMethod = timeMethod, spaceMethod = spaceMethod, green = green, time=time, space=space, color1=color1, color2 =color2)
# end

# function dynamic(green::GreenSym2DLR{DT,TT,TGT,SGT}, time, space, timeMethod::TM, spaceMethod::SM) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}
#     return  dynamic(; timeMethod = timeMethod, spaceMethod = spaceMethod, green = green, time=time, space=space, color1=1, color2 =1)
# end
