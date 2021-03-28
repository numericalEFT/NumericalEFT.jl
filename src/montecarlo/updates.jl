
function increaseOrder(config)
    new=rand(config.rng, config.groups)
    curr = config.curr

    (new.internal == curr.internal) && return #new and curr groups should not be the same order

    diff = new.internal - curr.internal
    for num in diff
        if num < 0 || num > 2
            return #the internal variables of the new and curr groups should be either the same or a lot more 
        end
    end

    prop = 1.0
    for (idx, v) in enumerate(config.var)
        for pos = curr.internal[idx]+1:new.internal[idx]
            prop *= create!(v, pos, config.rng)
        end
    end

    newAbsWeight = abs(new.eval(config))
    R = prop * newAbsWeight * new.reWeightFactor / curr.absWeight / curr.reWeightFactor

    # println(prop, ", ", newAbsWeight)
    curr.propose[Symbol(increaseOrder)]+=1.0
    if rand(config.rng) < R
        curr.accept[Symbol(increaseOrder)]+=1.0
        new.absWeight = newAbsWeight
        config.curr = new
    end
end

function decreaseOrder(config)
    new=rand(config.rng, config.groups)
    curr = config.curr
    (new.internal == curr.internal) && return #new and curr groups should not be the same order

    diff = curr.internal - new.internal
    for num in diff
        if num < 0 || num > 2
            return #the internal variables of the new and curr groups should be either the same or a lot more 
        end
    end
    prop = 1.0
    for (idx, v) in enumerate(config.var)
        for pos = new.internal[idx]+1:curr.internal[idx]
            prop *= remove(v, pos, config.rng)
        end
    end
    newAbsWeight = abs(new.eval(config))
    R = prop * newAbsWeight * new.reWeightFactor / curr.absWeight / curr.reWeightFactor
    curr.propose[Symbol(decreaseOrder)]+=1.0
    if rand(config.rng) < R
        curr.accept[Symbol(decreaseOrder)]+=1.0
        new.absWeight = newAbsWeight
        config.curr = new
    end
end

# function changeTau()
#     # Proposed[CHANGE_TAU, curr.order + 1] += 1
#     # Accepted[CHANGE_TAU, curr.order + 1] += 1
#     return
# end

function changeInternal(config)
    curr=config.curr
    varidx=rand(config.rng, 1:length(config.var))
    var=config.var[varidx] #get the internal variable table
    varnum=curr.internal[varidx] #number of var of the current group
    (varnum<=0) && return #return if the var number is less than 1
    idx=rand(config.rng, 1:varnum) #randomly choose one var to update
    oldvar=var[idx] 
    prop=shift!(var, idx, config.rng)

    newAbsWeight = abs(curr.eval(config))
    R = prop * newAbsWeight / curr.absWeight
    curr.propose[Symbol(changeInternal)]+=1.0
    if rand(config.rng) < R
        curr.accept[Symbol(changeInternal)]+=1.0
        curr.absWeight = newAbsWeight
    else
        var[idx] = oldvar
    end
end