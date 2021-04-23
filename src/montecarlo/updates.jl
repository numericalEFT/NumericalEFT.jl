function changeDiagram(config, integrand)
    # update to change a diagram to its neighbors. 
    # The degrees of freedom could be increase, decrease or remain the same.

    curr = config.curr
    new = rand(config.rng, config.neighbor[curr]) # jump to a randomly picked neighboring diagram

    newdiag = config.diagrams[new]
    currdiag = config.diagrams[curr]

    # propse probability caused by the selection of neighbors
    prop = length(config.neighbor[curr]) / length(config.neighbor[new])

    # create/remove variables if there are more/less degrees of freedom
    for vi in 1:length(config.var)
        if (currdiag.nvar[vi] < newdiag.nvar[vi]) # more degrees of freedom
            for pos = currdiag.nvar[vi] + 1:newdiag.nvar[vi]
                prop *= create!(config.var[vi], pos, config.rng)
            end
        elseif (currdiag.nvar[vi] > newdiag.nvar[vi]) # less degrees of freedom
            for pos = newdiag.nvar[vi] + 1:currdiag.nvar[vi]
                prop *= remove(config.var[vi], pos, config.rng)
            end
        end
    end

    config.curr = new
    currAbsWeight = config.absWeight
    newAbsWeight = abs(integrand(config))
    R = prop * newAbsWeight * newdiag.reWeightFactor / currAbsWeight / currdiag.reWeightFactor

    currdiag.proposeDiag[new] += 1.0
    if rand(config.rng) < R  # accept the change
        currdiag.acceptDiag[new] += 1.0
        config.absWeight = newAbsWeight
    else # reject the change
        config.curr = curr # reset the current diagram index
        config.absWeight = currAbsWeight
    end
end

# function increaseOrder(config, integrand)
#     # idx = rand(config.rng, 1:length(config.diagrams))
#     curr = config.curr
#     new = rand(config.rng, config.neighbor[curr])
#     if new <= curr # order must increase, so that there are more degrees of freedom
#         return
#     end
#     newdiag = config.diagrams[new]
#     currdiag = config.diagrams[curr]

#     prop = length(config.neighbor[curr]) / length(config.neighbor[new])
#     for vi in 1:length(config.var)
#         for pos = currdiag.nvar[vi] + 1:newdiag.nvar[vi]
#             prop *= create!(config.var[vi], pos, config.rng)
#         end
#     end

#     config.curr = new
#     currAbsWeight = config.absWeight
#     newAbsWeight = abs(integrand(config))
#     R = prop * newAbsWeight * newdiag.reWeightFactor / currAbsWeight / currdiag.reWeightFactor

#     currdiag.propose[1] += 1.0
#     if rand(config.rng) < R
#         currdiag.accept[1] += 1.0
#         config.absWeight = newAbsWeight
#     else
#         config.curr = curr
#         config.absWeight = currAbsWeight
#     end
# end

# function decreaseOrder(config, integrand)
#     idx = rand(config.rng, 1:length(config.diagrams))
#     new = config.diagrams[idx]
#     curr = config.curr

#     if (new.order != curr.order - 1)
#         return
#     end

#     prop = 1.0
#     for vi in 1:length(config.var)
#         for pos = new.nvar[vi] + 1:curr.nvar[vi]
#             prop *= remove(config.var[vi], pos, config.rng)
#         end
#     end

#     config.curr = new
#     currAbsWeight = config.absWeight
#     newAbsWeight = abs(integrand(config))
#     R = prop * newAbsWeight * new.reWeightFactor / currAbsWeight / curr.reWeightFactor
#     curr.propose[2] += 1.0
#     if rand(config.rng) < R
#         curr.accept[2] += 1.0
#         config.absWeight = newAbsWeight
#         config.curr = new
#     else
#         # in case the user modifies config.absWeight when calculate integrand(config)
#         config.absWeight = currAbsWeight
#         config.curr = curr
#     end
# end
"""
update 
"""
function changeVar(config, integrand)
    # update to change the variables of the current diagrams

    currdiag = config.diagrams[config.curr]
    vi = rand(config.rng, 1:length(currdiag.nvar)) # update the variable type of the index vi
    var = config.var[vi]
    (currdiag.nvar[vi] <= 0) && return # return if the var has zero degree of freedom
    idx = rand(config.rng, 1:currdiag.nvar[vi]) # randomly choose one var to update
    oldvar = copy(var[idx])
    prop = shift!(var, idx, config.rng)

    newAbsWeight = abs(integrand(config))
    currAbsWeight = config.absWeight
    R = prop * newAbsWeight / currAbsWeight
    currdiag.proposeVar[vi] += 1.0
    if rand(config.rng) < R
        currdiag.acceptVar[vi] += 1.0
        config.absWeight = newAbsWeight
    else
        var[idx] = oldvar
        config.absWeight = currAbsWeight 
    end
end

# function changeVar(config, integrand)
#     curr = config.curr
#     vi = rand(config.rng, 1:length(curr.nvar))
#     var = config.var[vi]
#     (curr.nvar[vi] <= 0) && return # return if the var number is less than 1
#     idx = rand(config.rng, 1:curr.nvar[vi]) # randomly choose one var to update
#     oldvar = copy(var[idx])
#     # var[end] = var[idx]
#     prop = shift!(var, idx, config.rng)

#     currAbsWeight = config.absWeight
#     newAbsWeight = abs(integrand(config))
#     R = prop * newAbsWeight / currAbsWeight
#     curr.propose[2 + vi] += 1.0
#     if rand(config.rng) < R
#         curr.accept[2 + vi] += 1.0
#         config.absWeight = newAbsWeight
#     else
#         var[idx] = oldvar
#         # var[idx] = var[end]
#         # in case the user modifies config.absWeight when calculate integrand(config)
#         config.absWeight = currAbsWeight 
#     end
# end