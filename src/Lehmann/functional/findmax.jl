function shift3(a, b, c, d)
    # a = b
    # b = c
    # c = d
    return b, c, d
end

function shift2(a, b, c)
    a = b
    b = c
    return a, b
end

middle(x1, x2) = sqrt(x1 * x2)

"""
Golden Section Search for the maximum in a one dimensional interval
"""
# function findCandidate(basis, proj, gmin::Float, gmax::Float)
#     @assert gmax > gmin
#     if gmin < Float(0.01)
#         gmin = Float(0.01)
#     end

#     R::Float = (sqrt(Float(5)) - 1) / 2
#     C::Float = 1 - R
#     # tol = 0.01
#     # R = 0.4
#     # C = 0.6

#     ax, cx = log(gmin), log(gmax)
#     bx = (ax + cx) / 2
#     # bx = R * ax + C * cx
#     # bx = sqrt(ax * cx)  # if ax<1, then replace it with one 

#     # x0, x3 = gmin, gmax
#     # x2 = middle(gmin, gmax)
#     # x1 = middle(x0, x2)

#     x0, x3 = ax, cx
#     # at any given time, we will keep track of four points x0, x1, x2, x3
#     # make x0 to x1 the smaller segment and fill in the new point to be tried
#     if abs(cx - bx) < abs(bx - ax)
#         x1, x2 = bx, bx + C * (cx - bx)
#     else
#         x2, x1 = bx, bx - C * (bx - ax)
#     end
#     @assert x0 <= x1 <= x2 <= x3 "inital $(exp(x0))  $(exp(x1))  $(exp(x2))  $(exp(x3))"
#     # println("$x0, $x1, $x2, $x3")
#     # the initial function evaluations. Note that we never need to evaluate the function at the original endpoints.
#     f1, f2 = -Residual(basis, proj, exp(x1)), -Residual(basis, proj, exp(x2))
#     N = 2
#     # println("$f1  vs $f2")
#     # while abs(x3 - x0) > max(Float(1), ax) / Float(100)
#     while abs(x3 - x0) > 0.1
#         println("old $(exp(x0))  $(exp(x1))  $(exp(x2))  $(exp(x3))")
#         println("old $f1 vs $f2")
#         if f2 < f1
#             # println("this one: $x0, $x1, $x2, $(R * x1 + C * x3)")
#             println("one")
#             x0, x1, x2 = shift3(x0, x1, x2, R * x1 + C * x3)
#             # x0, x1, x2 = shift3(x0, x1, x2, x1^R * x3^C)
#             # println("$x1 x $x3 --> $(sqrt(x1 * x3))")
#             # x0, x1, x2 = shift3(x0, x1, x2, sqrt(x1 * x3))
#             f1, f2 = shift2(f1, f2, -Residual(basis, proj, exp(x2)))
#             # println("after one: $x0, $x1, $x2: $(x2 - x1)")
#         else
#             println("two")
#             x3, x2, x1 = shift3(x3, x2, x1, R * x2 + C * x0)
#             # x3, x2, x1 = shift3(x3, x2, x1, sqrt(x2 * x0))
#             f2, f1 = shift2(f2, f1, -Residual(basis, proj, exp(x1)))
#         end
#         println("new $(exp(x0))  $(exp(x1))  $(exp(x2))  $(exp(x3))")
#         println("new $f1 vs $f2")
#         @assert x0 <= x1 <= x2 <= x3 "$(exp(x0))  $(exp(x1))  $(exp(x2))  $(exp(x3))"
#         N += 1
#     end
#     println("$N eval")
#     if f1 < f2
#         return exp(x1)
#     else
#         return exp(x2)
#     end
# end

function findCandidate(basis, proj, gmin::Float, gmax::Float)
    @assert gmax > gmin
    if gmin < Float(0.01)
        gmin = Float(0.01)
    end

    R::Float = (sqrt(Float(5)) - 1) / 2
    C::Float = 1 - R
    # tol = 0.01
    # R = 0.4
    # C = 0.6

    a, b = log(gmin), log(gmax)

    h = b - a
    # if h <= tol:
    #     return (a, b)

    c = a + C * h
    d = a + R * h
    yc = -Residual(basis, proj, exp(c))
    yd = -Residual(basis, proj, exp(d))
    N = 2 

    # for k in range(n-1):
    # while abs(b - a) > 0.1 && abs(yc - yd) > Float(0) * 10
    while abs(b - a) > 0.1 
        # println("old $(exp(a))  $(exp(c))  $(exp(d))  $(exp(b))")
        # println("$yc  vs  $yd")
        if yc < yd
            b = d
            d = c
            yd = yc
            h = R * h
            c = a + C * h
            yc = -Residual(basis, proj, exp(c))
        else
            a = c
            c = d
            yc = yd
            h = R * h
            d = a + R * h
            yd = -Residual(basis, proj, exp(d))
        end
        # println("new $(exp(a))  $(exp(c))  $(exp(d))  $(exp(b))")
        # println("$yc  vs  $yd")
        N += 1
        @assert a <= c <= d <= b "$(exp(a))  $(exp(c))  $(exp(d))  $(exp(b))"
    end

    # println("max eval = $N")
    if yc < yd
        return exp(c)
    else
        return exp(d)
    end
end

        
# function findCandidate(basis, proj, gmin::Float, gmax::Float)
#     @assert gmax > gmin

#     if abs(gmin) < 100 * eps(Float(0)) && gmax > 100  
#         gmax = 100 # if the first grid is 0, then the maximum should be between (0, 100)
#     end
#     if abs(gmin) > 100 * eps(Float(0)) && gmax / gmin > 100
#         gmax = gmin * 100  # the maximum won't be larger than 100*gmin
#     end

#     N = 32
#     dg = (gmax - gmin) / N

#     ###################   if gmin/gmax are at the boundary 0/Λ, the maximum could be at the edge ##################
#     if abs(gmin) < 100 * eps(Float(0)) && Residual(basis, proj, gmin) > Residual(basis, proj, gmin + dg)
#         return gmin
#     end
#     if abs(gmax - basis.Λ) < 100 * eps(Float(gmax)) && Residual(basis, proj, gmax) > Residual(basis, proj, gmax - dg)
#         return gmax
#     end

#     ###################  the maximum must be between (gmin, gmax) for the remaining cases ##################
#     # check https://www.geeksforgeeks.org/find-the-maximum-element-in-an-array-which-is-first-increasing-and-then-decreasing/ for detail

#     l, r = 1, N - 1 # avoid the boundary gmin and gmax
#     while l <= r
#         m = l + Int(round((r - l) / 2))
#         g = gmin + m * dg

#         r1, r2, r3 = Residual(basis, proj, g - dg), Residual(basis, proj, g), Residual(basis, proj, g + dg)
#         if r2 >= r1 && r2 >= r3
#             # plotResidual(basis, proj, gmin, gmax)
#             return g
#         end

#         if r3 < r2 < r1
#             r = m - 1
#         elseif r1 < r2 < r3
#             l = m + 1
#         else
#             if abs(r1 - r2) < 1e-17 && abs(r2 - r3) < 1e-17
#                 return g
#             end
#             println("warning: illegl! ($l, $m, $r) with ($r1, $r2, $r3)")
#             # plotResidual(basis, proj, gmin, gmax)
#             exit(0)
#         end
#     end
#     # plotResidual(basis, proj, gmin, gmax)
#     throw("failed to find maximum between ($gmin, $gmax)!")
# end