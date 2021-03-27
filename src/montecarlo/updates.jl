
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
            prop *= create!(v, pos)
        end
    end

    newAbsWeight = abs(new.eval(config))
    R = prop * newAbsWeight * new.reWeightFactor / curr.absWeight / curr.reWeightFactor
    curr.proposal[Symbol(increaseOrder)]+=1.0
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
            prop *= remove(v, pos)
        end
    end
    newAbsWeight = abs(new.eval(config))
    R = prop * newAbsWeight * new.reWeightFactor / curr.absWeight / curr.reWeightFactor
    curr.proposal[Symbol(decreaseOrder)]+=1.0
    if rand(config.rng) < R
        curr.accept[Symbol(decreaseOrder)]+=1.0
        new.absWeight = newAbsWeight
        config.curr = new
    end
end