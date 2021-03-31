
function increaseOrder(config, integrand)
    idx = rand(config.rng, 1:length(config.diagrams))
    curr = config.curr
    new = config.diagrams[idx]
    if (new.order != curr.order + 1)
        return
    end

    prop = 1.0
    for pos = curr.nX + 1:new.nX
        prop *= create!(config.X, pos, config.rng)
    end
    for pos = curr.nK + 1:new.nK
        prop *= create!(config.K, pos, config.rng)
    end

    currAbsWeight = config.absWeight

    config.curr = new
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight * new.reWeightFactor / currAbsWeight / curr.reWeightFactor

    curr.propose[1] += 1.0
    if rand(config.rng) < R
        curr.accept[1] += 1.0
        config.curr = new
        config.absWeight = newAbsWeight
    else
        config.curr = curr
        # in case the user modifies config.absWeight when calculate integrand(config)
        config.absWeight = currAbsWeight
    end
end

function decreaseOrder(config, integrand)
    idx = rand(config.rng, 1:length(config.diagrams))
    new = config.diagrams[idx]
    curr = config.curr

    if (new.order != curr.order - 1)
        return
    end

    prop = 1.0
    for pos = new.nX + 1:curr.nX
        prop *= remove(config.X, pos, config.rng)
    end
    for pos = new.nK + 1:curr.nK
        prop *= remove(config.K, pos, config.rng)
    end

    config.curr = new
    currAbsWeight = config.absWeight
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight * new.reWeightFactor / currAbsWeight / curr.reWeightFactor
    curr.propose[2] += 1.0
    if rand(config.rng) < R
        curr.accept[2] += 1.0
        config.absWeight = newAbsWeight
        config.curr = new
    else
        # in case the user modifies config.absWeight when calculate integrand(config)
        config.absWeight = currAbsWeight
        config.curr = curr
    end
end

function changeX(config, integrand)
    curr = config.curr
    (curr.nX <= 0) && return # return if the var number is less than 1
    idx = rand(config.rng, 1:curr.nX) # randomly choose one var to update
    oldvar = config.X[idx]
    prop = shift!(config.X, idx, config.rng)

    currAbsWeight = config.absWeight
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight / currAbsWeight
    curr.propose[3] += 1.0
    if rand(config.rng) < R
        curr.accept[3] += 1.0
        config.absWeight = newAbsWeight
    else
        config.X[idx] = oldvar
        # in case the user modifies config.absWeight when calculate integrand(config)
        config.absWeight = currAbsWeight 
    end
end

function changeK(config, integrand)
    curr = config.curr
    (curr.nK <= 0) && return # return if the var number is less than 1
    idx = rand(config.rng, 1:curr.nK) # randomly choose one var to update
    oldvar = config.K[idx]
    prop = shift!(config.K, idx, config.rng)

    currAbsWeight = config.absWeight
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight / currAbsWeight
    curr.propose[4] += 1.0
    if rand(config.rng) < R
        curr.accept[4] += 1.0
        config.absWeight = newAbsWeight
    else
        config.K[idx] = oldvar
        # in case the user modifies config.absWeight when calculate integrand(config)
        config.absWeight = currAbsWeight 
    end
end

function changeExt(config, integrand)
    ext = config.ext
    size = ext.size
    (length(size) == 1 && size[1] == 1) && return # return if there is only one external bin
    curr = config.curr
    i = rand(config.rng, 1:length(size)) # randomly choose one var to update
    oldidx = ext.idx[i]
    prop = shift!(ext, i, config.rng)

    currAbsWeight = config.absWeight
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight / currAbsWeight
    curr.propose[5] += 1.0
    # curr.propose[Symbol(changeInternal)]+=1.0
    if rand(config.rng) < R
        curr.accept[5] += 1.0
        config.absWeight = newAbsWeight
    else
        ext.idx[i] = oldidx
        # in case the user modifies config.absWeight when calculate integrand(config)
        config.absWeight = currAbsWeight 
    end
end