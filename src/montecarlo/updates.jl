function increaseOrder(config, new)
    curr=config.curr
    if (new.internal!=curr.internal+1)
        return
    end

    prop = 1.0
    if curr.order == 0
        # create new external Tau
        curr.extTidx, propT = createExtIdx(TauGridSize)
        varT[LastTidx] = Grid.tau.grid[curr.extTidx]

        curr.extKidx, propK = createExtIdx(KGridSize)
        varK[1][1] = Grid.K.grid[curr.extKidx]
        prop = propT * propK
    else
        # create new internal Tau
        varT[lastInnerTidx(newOrder)], prop = createTau()
    end
    # newOrder == 1 && println(lastInnerKidx(newOrder))
    # oldK = copy(varK[2])
    # prop *= createK!(varK[lastInnerKidx(newOrder)])
    prop *= createK!(varK[2])
    # @assert norm(oldK) != norm(varK[2]) "K remains the same"

    newAbsWeight = abs(eval(newOrder))
    # println(prop, ", ", newAbsWeight)
    R = prop * newAbsWeight * ReWeight[newOrder + 1] / curr.absWeight / ReWeight[curr.order + 1]
    propose(INCREASE_ORDER)
    if rand(rng) < R
        accept(INCREASE_ORDER)
        curr.order = newOrder
        curr.absWeight = newAbsWeight
    end
end