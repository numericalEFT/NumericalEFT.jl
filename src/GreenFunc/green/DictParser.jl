module DictParser

export evalwithdict

"""
evalwithdict(e::Union{Expr,Symbol,Number}, map::Dict{Symbol,Number})

Evaluate the result produced by Meta.parse, looking up the values of
user-defined variables in "map". Argument "e" is a Union, because
Meta.parse can produce values of type Expr, Symbol, or Number.
"""
function evalwithdict(e::Union{Expr,Symbol,Number}, map::Dict{Symbol,Number})
    try
        return f(e, map)
    catch ex
        println("Can't parse \"$e\"")
        rethrow(ex)
    end
end    

# Look up symbol and return value, or throw.
function f(s::Symbol, map::Dict{Symbol,Number})
    if haskey(map, s)
        return map[s]
    else
        throw(UndefVarError(s))
    end
end    

# Numbers are converted to type Number.
function f(x::Number, map::Dict{Symbol,Number})
    return Number(x)
end    

# To parse an expression, convert the head to a singleton
# type, so that Julia can dispatch on that type.
function f(e::Expr, map::Dict{Symbol,Number})
    return f(Val(e.head), e.args, map)
end

# Call the function named in args[1]
function f(::Val{:call}, args, map::Dict{Symbol,Number})
    return f(Val(args[1]), args[2:end], map)
end

# Addition
function f(::Val{:+}, args, map::Dict{Symbol,Number})
    x = 0.0
    for arg ∈ args
        x += f(arg, map)
    end
    return x
end

# Subtraction and negation
function f(::Val{:-}, args, map::Dict{Symbol,Number})
    len = length(args)
    if len == 1
        return -f(args[1], map)
    else
        return f(args[1], map) - f(args[2], map)
    end
end    

# Multiplication
function f(::Val{:*}, args, map::Dict{Symbol,Number})
    x = 1.0
    for arg ∈ args
        x *= f(arg, map)
    end
    return x
end    

# Division
function f(::Val{:/}, args, map::Dict{Symbol,Number})
    return f(args[1], map) / f(args[2], map)
end    

function f(::Val{:^}, args, map::Dict{Symbol,Number})
    return f(args[1], map) ^ f(args[2], map)
end    
end # module MyEval