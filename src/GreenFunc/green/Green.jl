"""
General Green's function. 
"""
#module Green2
#export TimeDomain, ImTime, ReTime, ImFreq, ReFreq, DLRFreq
#export Green2DLR, toTau, toMatFreq, toDLR, dynamic, instant

abstract type TimeDomain end
struct ImTime <: TimeDomain end
struct ReTime <: TimeDomain end
struct ImFreq <: TimeDomain end
struct ReFreq <: TimeDomain end
struct DLRFreq <: TimeDomain end
const IMTIME = ImTime()
const RETIME = ReTime()
const IMFREQ = ImFreq()
const REFREQ = ReFreq()
const DLRFREQ = DLRFreq()

abstract type InterpMethod end
struct DefaultInterp <: InterpMethod end
struct LinearInterp <: InterpMethod end
struct DLRInterp <: InterpMethod end
const DEFAULTINTERP = DefaultInterp()
const LINEARINTERP = LinearInterp()
const DLRINTERP = DLRInterp()


InterpMethod(GT::Type{<:CompositeGrids.AbstractGrid}, MT::Type{<:LinearInterp}) = CompositeGrids.Interp.LinearInterp()
InterpMethod(GT::Type{<:CompositeGrids.AbstractGrid}, MT::Type{<:DefaultInterp}) = CompositeGrids.Interp.InterpStyle(GT)
InterpMethod(GT, MT::Type{<:DLRInterp}) = DLRInterp()

"""
Green's function with two external legs that has in-built Discrete Lehmann Representation.
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
mutable struct Green2DLR{T<:Number,Type<:TimeDomain,TGT,SGT}
    name::Symbol
    # isFermi::Bool
    # β::Float64
    color::Int
    dlrGrid::DLRGrid

    #########    Mesh   ##############

    # timeType::DataType
    # timeSymmetry::Symbol
    timeGrid::TGT

    spaceType::Symbol
    spaceGrid::SGT

    ###########     data   ###########
    instant::Array{T,3}
    dynamic::Array{T,4}

    ####### statistical error handling #####
    instantError::Array{T,3}
    dynamicError::Array{T,4}

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
    function Green2DLR{T}(name::Symbol, timeType::TT, β, isFermi::Bool, Euv, spaceGrid, color::Int = 1;
        timeSymmetry::Symbol = :none, rtol = 1e-8, kwargs...
    ) where {T<:Number,TT<:TimeDomain}
        # @assert spaceType == :k || spaceType == :x
        @assert timeSymmetry == :ph || timeSymmetry == :pha || timeSymmetry == :none

        spaceType = :k #TODO: replace it with spaceGrid.type after we have a spaceGrid package

        dlrGrid = DLRGrid(Euv, β, rtol, isFermi, timeSymmetry)

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
        instantError = Array{T,3}(undef, 0, 0, 0)
        dynamicError = Array{T,4}(undef, 0, 0, 0, 0)

        gnew = new{T,TT,typeof(timeGrid),typeof(compSpaceGrid)}(
            name, color, dlrGrid,
            timeGrid,
            spaceType, compSpaceGrid,
            instant, dynamic,
            instantError, dynamicError)
        return set!(gnew; kwargs...)
    end
end

function Base.getproperty(obj::Green2DLR{T,TT,TGT,SGT}, sym::Symbol) where {T,TT,TGT,SGT}
    if sym === :isFermi
        return obj.dlrGrid.isFermi
    elseif sym === :β
        return obj.dlrGrid.β
    elseif sym === :timeType
        return TT
    elseif sym === :timeSymmetry
        return obj.dlrGrid.symmetry
    else # fallback to getfield
        return getfield(obj, sym)
    end
end

function Base.size(green::Green2DLR)
    return (green.color, green.color, size(green.spaceGrid), size(green.timeGrid))
end

function set!(green::Green2DLR; kwargs...)
    dynamicSize = size(green)
    instantSize = size(green)[1:3]
    if :dynamic in keys(kwargs) && isempty(kwargs[:dynamic]) == false
        green.dynamic = reshape(kwargs[:dynamic], Tuple(dynamicSize))
    end
    if :instant in keys(kwargs) && isempty(kwargs[:instant]) == false
        green.instant = reshape(kwargs[:instant], Tuple(instantSize))
    end
    if :dynamicError in keys(kwargs) && isempty(kwargs[:dynamicError]) == false
        green.dynamicError = reshape(kwargs[:dynamicError], Tuple(dynamicSize))
    end
    if :instantError in keys(kwargs) && isempty(kwargs[:instantError]) == false
        green.instantError = reshape(kwargs[:instantError], Tuple(instantSize))
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
function toTau(green::Green2DLR, targetGrid = green.dlrGrid.τ)

    if targetGrid isa AbstractVector
        targetGrid = CompositeGrids.SimpleG.Arbitrary{eltype(targetGrid)}(targetGrid)
    end

    # do nothing if the domain and the grid remain the same
    if green.timeType == ImTime && length(green.timeGrid.grid) ≈ length(targetGrid.grid) && green.timeGrid.grid ≈ targetGrid.grid
        return green
    end
    if isempty(green.dynamic) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImTime)
        dynamic = tau2tau(green.dlrGrid, green.dynamic, targetGrid.grid, green.timeGrid.grid; axis = 4)
    elseif (green.timeType == ImFreq)
        dynamic = matfreq2tau(green.dlrGrid, green.dynamic, targetGrid.grid, green.timeGrid.grid; axis = 4)
    elseif (green.timeType == DLRFreq)
        dynamic = dlr2tau(green.dlrGrid, green.dynamic, targetGrid.grid; axis = 4)
    end

    return Green2DLR{eltype(dynamic)}(
        green.name, IMTIME, green.β, green.isFermi, green.dlrGrid.Euv, green.spaceGrid, green.color;
        timeSymmetry = green.timeSymmetry, timeGrid = targetGrid, rtol = green.dlrGrid.rtol,
        dynamic = dynamic, instant = green.instant)
end

"""
    function toMatFreq(green::Green2DLR, targetGrid =  green.dlrGrid.n)
Convert Green's function to matfreq space by Fourier transform.
If green is already in matfreq space then it will be interpolated to the new grid.

#Arguements
- 'green': Original Green's function
- 'targetGrid': Grid of outcome Green's function. Default: DLR n grid
"""
function toMatFreq(green::Green2DLR, targetGrid = green.dlrGrid.n)

    if targetGrid isa AbstractVector
        targetGrid = CompositeGrids.SimpleG.Arbitrary{eltype(targetGrid)}(targetGrid)
    end

    # do nothing if the domain and the grid remain the same
    if green.timeType == ImFreq && length(green.timeGrid.grid) ≈ length(targetGrid.grid) && green.timeGrid.grid ≈ targetGrid.grid
        return green
    end
    if isempty(green.dynamic) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImFreq)
        dynamic = matfreq2matfreq(green.dlrGrid, green.dynamic, targetGrid.grid, green.timeGrid.grid; axis = 4)
    elseif (green.timeType == ImTime)
        dynamic = tau2matfreq(green.dlrGrid, green.dynamic, targetGrid.grid, green.timeGrid.grid; axis = 4)
    elseif (green.timeType == DLRFreq)
        dynamic = dlr2matfreq(green.dlrGrid, green.dynamic, targetGrid.grid; axis = 4)
    end

    return Green2DLR{eltype(dynamic)}(
        green.name, IMFREQ, green.β, green.isFermi, green.dlrGrid.Euv, green.spaceGrid, green.color;
        timeSymmetry = green.timeSymmetry, timeGrid = targetGrid, rtol = green.dlrGrid.rtol,
        dynamic = dynamic, instant = green.instant)

end

"""
    function toDLR(green::Green2DLR)
Convert Green's function to dlr space.

#Arguements
- 'green': Original Green's function
"""
function toDLR(green::Green2DLR)

    # do nothing if the domain and the grid remain the same
    if green.timeType == DLRFreq
        return green
    end
    if isempty(green.dynamic) # if dynamic data has not yet been initialized, there is nothing to do
        return green
    end


    if (green.timeType == ImTime)
        dynamic = tau2dlr(green.dlrGrid, green.dynamic, green.timeGrid.grid; axis = 4)
    elseif (green.timeType == ImFreq)
        dynamic = matfreq2dlr(green.dlrGrid, green.dynamic, green.timeGrid.grid; axis = 4)
    end

    return Green2DLR{eltype(dynamic)}(
        green.name, DLRFREQ, green.β, green.isFermi, green.dlrGrid.Euv, green.spaceGrid, green.color;
        timeSymmetry = green.timeSymmetry, timeGrid = green.dlrGrid.ω, rtol = green.dlrGrid.rtol,
        dynamic = dynamic, instant = green.instant)

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


"""
    function dynamic(green::Green2DLR{DT,TT,TGT,SGT}, time, space, color1::Int, color2::Int, timeMethod::TM , spaceMethod::SM) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}

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

# function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space,  timeMethod::TM , spaceMethod::SM) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid,TM,SM}
#     return  dynamic(; timeMethod = timeMethod, spaceMethod = spaceMethod, green = green, time=time, space=space, color1=1, color2 =1)
# end

# function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
#     return dynamic(; timeMethod =DEFAULTINTERP, spaceMethod = DEFAULTINTERP, green=green, time=time, space=space,color1=1, color2 =1)
# end

# function dynamic(green::Union{Green2DLR{DT,TT,TGT,SGT},GreenSym2DLR{DT,TT,TGT,SGT}}, time, space, color1::Int, color2::Int) where {DT,TT,TGT<:CompositeGrids.AbstractGrid,SGT<:CompositeGrids.AbstractGrid}
#     return dynamic(; timeMethod =DEFAULTINTERP, spaceMethod = DEFAULTINTERP, green = green, time=time, space=space, color1=color1, color2 =color2)
# end

