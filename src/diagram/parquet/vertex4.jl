struct Para
    chan::Vector{Int}
    chantype::Vector{Symbol} # channel types, :T, :S, or :U
    bubble::Dict{Int,Tuple{Vector{Int},Vector{Int}}} 
    # key: channels that are bubbles; 
    # value: (lver, rver) where lver is the lists of allowed channels in the left sub-vertex
    # rver is the lists of allowed channels in the right sub-vertex
    interactionTauNum::Vector{Int} # list of possible τ degrees of freedom of the bare interaction 0, 2, or 4
    function Para(chantype, bubble, interactionTauNum)

        for tnum in interactionTauNum
            @assert tnum == 1 || tnum == 2 || tnum == 4
        end

        chan = [i for i in 1:length(chantype)]
        for k in keys(bubble) # check validity of the bubble dictionary
            @assert issubset(k, chan) "$k isn't in the channel list $chan"
            lver, rver = bubble[k]
            @assert issubset(lver, chan) "$lver isn't in the channel list $chan"
            @assert issubset(rver, chan) "$rver isn't in the channel list $chan"
        end
        return new(chan, chantype, bubble, interactionTauNum)
    end
end

struct Green{W}
    Tpair::Vector{Tuple{Int,Int}}
    weight::Vector{W}
    function Green{W}() where {W} 
        return new{W}([], [])
    end
end

# add Tpairs to Green's function (in, out) or vertex4 (inL, outL, inR, outR)
function addTidx(obj, _Tidx)
    for (i, Tidx) in enumerate(obj.Tpair)
        if Tidx == _Tidx
            return i
        end
    end
    push!(obj.Tpair, _Tidx)
    push!(obj.weight, zero(eltype(obj.weight))) # add zero to the weight table of the object
    return length(obj.Tpair)
end

struct IdxMap
    lv::Int # left sub-vertex index
    rv::Int # right sub-vertex index
    G0::Int # shared Green's function index
    Gx::Int # channel specific Green's function index
    ver::Int # composite vertex index
end

struct Bubble{_Ver4,W} # template Bubble to avoid mutually recursive struct
    chan::Int
    Lver::_Ver4
    Rver::_Ver4
    map::Vector{IdxMap}

    function Bubble{_Ver4,W}(ver4::_Ver4, chan::Int, oL::Int, para::Para, level::Int) where {_Ver4,W}
        @assert chan in keys(para.bubble) "$chan isn't a bubble channels!"
        @assert oL < ver4.loopNum "LVer loopNum must be smaller than the ver4 loopNum"

        oR = ver4.loopNum - 1 - oL # loopNum of the right vertex
        LTidx = ver4.Tidx  # the first τ index of the left vertex
        maxTauNum = maximum(para.interactionTauNum) # maximum tau number for each bare interaction
        RTidx = LTidx + (oL + 1) * maxTauNum + 1  # the first τ index of the right sub-vertex
        LsubVer, RsubVer = para.bubble[chan]

        Lver = _Ver4{W}(oL, LTidx, para; chan=LsubVer, level=level + 1)
        Rver = _Ver4{W}(oR, RTidx, para; chan=RsubVer, level=level + 1)

        @assert Lver.Tidx == ver4.Tidx "Lver Tidx must be equal to vertex4 Tidx! LoopNum: $(ver4.loopNum), LverLoopNum: $(Lver.loopNum), chan: $chan"

        ############## construct IdxMap ########################################
        map = []
        G = ver4.G
        for (lt, LvT) in enumerate(Lver.Tpair)
            for (rt, RvT) in enumerate(Rver.Tpair)
                GT0idx = addTidx(G[1], (LvT[OUTR], RvT[INL]))
                GTxidx, VerTidx = 0, 0

                if para.chantype[chan] == :T
                    VerTidx = addTidx(ver4, (LvT[INL], LvT[OUTL], RvT[INR], RvT[OUTR]))
                    GTxidx = addTidx(G[2], (RvT[OUTL], LvT[INR]))
                elseif para.chantype[chan] == :U
                    VerTidx = addTidx(ver4, (LvT[INL], RvT[OUTR], RvT[INR], LvT[OUTL]))
                    GTxidx = addTidx(G[3], (RvT[OUTL], LvT[INR]))
                elseif para.chantype[chan] == :S
                    VerTidx = addTidx(ver4, (LvT[INL], RvT[OUTL], LvT[INR], RvT[OUTR]))
                    GTxidx = addTidx(G[4], (LvT[OUTL], RvT[INR]))
                else
                    throw("This channel is invalid!")
                end

                for tpair in ver4.Tpair
                    @assert tpair[1] == ver4.Tidx "InL Tidx must be the same for all Tpairs in the vertex4"
                end
                push!(map, IdxMap(lt, rt, GT0idx, GTxidx, VerTidx))
            end
        end
        return new(chan, Lver, Rver, map)
    end
end

struct Ver4{W}
    ###### vertex topology information #####################
    level::Int
    
    #######  vertex properties   ###########################
    loopNum::Int
    chan::Vector{Int} # list of channels
    Tidx::Int # inital Tidx

    ######  components of vertex  ##########################
    G::SVector{4,Green}
    bubble::Vector{Bubble{Ver4}}

    ####### weight and tau table of the vertex  ###############
    Tpair::Vector{Tuple{Int,Int,Int,Int}}
    weight::Vector{W}

    function Ver4{W}(loopNum, tidx, para::Para; chan=nothing, level=1) where {W}
        if isnothing(chan)
            chan = para.chan
        end
        g = @SVector [Green{W}() for i = 1:4]
        ver4 = new{W}(level, loopNum, chan, tidx, g, [], [], [])
        @assert loopNum >= 0
        if loopNum == 0
            # bare interaction may have one, two or four independent tau variables
            if 1 in para.interactionTauNum  # instantaneous interaction
                addTidx(ver4, (tidx, tidx, tidx, tidx)) 
            end
            if 2 in para.interactionTauNum  # interaction with incoming and outing τ varibales
                addTidx(ver4, (tidx, tidx, tidx + 1, tidx + 1))  # direct dynamic interaction
                addTidx(ver4, (tidx, tidx + 1, tidx + 1, tidx))  # exchange dynamic interaction
            end
            if 4 in para.interactionTauNum  # interaction with incoming and outing τ varibales
                addTidx(ver4, (tidx, tidx + 1, tidx + 2, tidx + 3))  # direct dynamic interaction
                addTidx(ver4, (tidx, tidx + 3, tidx + 2, tidx + 1))  # exchange dynamic interaction
            end
            return ver4
        end
        for c in keys(para.bubble)
            for ol = 0:loopNum - 1
                bubble = Bubble{Ver4,W}(ver4, c, ol, para, level)
                if length(bubble.map) > 0  # if zero, bubble diagram doesn't exist
                    push!(ver4.bubble, bubble)
                end
            end
        end
        # TODO: add envolpe diagrams
        # for c in II
        # end
        return ver4
    end
end

function showTree(ver4, para::Para; verbose=0)

    pushfirst!(PyVector(pyimport("sys")."path"), @__DIR__)
    ete = pyimport("ete3")
    tree = pyimport("tree")

    function tpair(ver4)
        s = ""
        for T in ver4.Tpair
            s *= "($(T[1]), $(T[2]), $(T[3]), $(T[4]))" 
        end
        return s
    end

    function treeview(ver4, t=nothing)
        if isnothing(t)
            t = ete.Tree(name=" ")
        end

        if ver4.loopNum != 0
            prefix = "$(ver4.loopNum) lp, $(length(ver4.Tpair)) elem"
            # if verbose > 0
            #     prefix *= ": $(tpair(ver4))" 
            # end
            nt = t.add_child(name=prefix * ", ⨁")
            # tgf = nt.add_child(name="bubble")
            # tgf = nt
        else
            nt = t.add_child(name=tpair(ver4))
            # tgf = nt
            return t
        end

        for bub in ver4.bubble
            nnt = nt.add_child(name="$(para.chantype[bub.chan])$(ver4.loopNum)Ⓧ")
            treeview(bub.Lver, nnt)
            treeview(bub.Rver, nnt)

            # nnnt = nnt.add_child(name(bub.Lver)
            # nnt.add_sister(treeview(bub.Rver, nnt))
            # nnnt = nnt.add_child(treeview(bub.Lver, nnt))
            # nnt.add_sister(treeview(bub.Rver, nnt))
        end

        # for bub in ver4.bubble
        #     nnt = tgf.add_sister(name="Ⓧ")
        #     nnnt = treeview(bub.Lver, nnt)
        #     nnnt.add_sister(name=str(p.right))
        # end
        return t
    end

    println("start tree builded")
    t = treeview(ver4)
    println("tree builded")
    tree.plot(t)
end

# function eval(ver4::Ver4, KinL, KoutL, KinR, KoutR, Kidx::Int, fast=false)
#     if ver4.loopNum == 0
#         DiagType == POLAR ?
#         ver4.weight[1] = interaction(KinL, KoutL, KinR, KoutR, ver4.inBox, norm(varK[0])) :
#         ver4.weight[1] = interaction(KinL, KoutL, KinR, KoutR, ver4.inBox)
#         return
#     end

#     # LoopNum>=1
#     for w in ver4.weight
#         w .= 0.0 # initialize all weights
#     end
#     G = ver4.G
#     K, Kt, Ku, Ks = (varK[Kidx], ver4.K[1], ver4.K[2], ver4.K[3])
#     eval(G[1], K, varT)
#     bubWeight = counterBubble(K)

#     for c in ver4.chan
#         if c == T || c == TC
#             Kt .= KoutL .+ K .- KinL
#             if (!ver4.inBox)
#                 eval(G[T], Kt)
#             end
#         elseif c == U || c == UC
#             # can not be in box!
#             Ku .= KoutR .+ K .- KinL
#             eval(G[U], Ku)
#         else
#             # S channel, and cann't be in box!
#             Ks .= KinL .+ KinR .- K
#             eval(G[S], Ks)
#         end
#     end
#     for b in ver4.bubble
#         c = b.chan
#         Factor = SymFactor[c] * PhaseFactor
#         Llopidx = Kidx + 1
#         Rlopidx = Kidx + 1 + b.Lver.loopNum

#         if c == T || c == TC
#             eval(b.Lver, KinL, KoutL, Kt, K, Llopidx)
#             eval(b.Rver, K, Kt, KinR, KoutR, Rlopidx)
#         elseif c == U || c == UC
#             eval(b.Lver, KinL, KoutR, Ku, K, Llopidx)
#             eval(b.Rver, K, Ku, KinR, KoutL, Rlopidx)
#         else
#             # S channel
#             eval(b.Lver, KinL, Ks, KinR, K, Llopidx)
#             eval(b.Rver, K, KoutL, Ks, KoutR, Rlopidx)
#         end

#         rN = length(b.Rver.weight)
#         gWeight = 0.0
#         for (l, Lw) in enumerate(b.Lver.weight)
#             for (r, Rw) in enumerate(b.Rver.weight)
#                 map = b.map[(l - 1) * rN + r]

#                     if ver4.inBox || c == TC || c == UC
#                     gWeight = bubWeight * Factor
#                 else
#                     gWeight = G[1].weight[map.G] * G[c].weight[map.Gx] * Factor
#                 end

#                 if fast && ver4.level == 0
#                     pair = ver4.Tpair[map.ver]
#                     dT =
#                         varT[pair[INL]] - varT[pair[OUTL]] + varT[pair[INR]] -
#                         varT[pair[OUTR]]
#                     gWeight *= cos(2.0 * pi / Beta * dT)
#                     w = ver4.weight[ChanMap[c]]
#                 else
#                     w = ver4.weight[map.ver]
#                 end

#                 if c == T || c == TC
#                     w[DI] +=
#                         gWeight *
#                         (Lw[DI] * Rw[DI] * SPIN + Lw[DI] * Rw[EX] + Lw[EX] * Rw[DI])
#                     w[EX] += gWeight * Lw[EX] * Rw[EX]
#                 elseif c == U || c == UC
#                     w[DI] += gWeight * Lw[EX] * Rw[EX]
#                     w[EX] +=
#                         gWeight *
#                         (Lw[DI] * Rw[DI] * SPIN + Lw[DI] * Rw[EX] + Lw[EX] * Rw[DI])
#                 else
#                     # S channel,  see the note "code convention"
#                     w[DI] += gWeight * (Lw[DI] * Rw[EX] + Lw[EX] * Rw[DI])
#                     w[EX] += gWeight * (Lw[DI] * Rw[DI] + Lw[EX] * Rw[EX])
#                 end

#             end
#         end

#     end
# end

# function _expandBubble(children, text, style, bub::Bubble, parent)
#     push!(children, zeros(Int, 0))
#     @assert parent == length(children)
#     dict = Dict(
#         I => ("I", "yellow"),
#         T => ("T", "red"),
#         TC => ("Tc", "pink"),
#         U => ("U", "blue"),
#         UC => ("Uc", "navy"),
#         S => ("S", "green"),
#     )
#     push!(text, "$(dict[bub.chan][1])\n$(bub.Lver.loopNum)-$(bub.Rver.loopNum)")
#     push!(style, "fill:$(dict[bub.chan][2])")

#     current = length(children) + 1
#     push!(children[parent], current)
#     _expandVer4(children, text, style, bub.Lver, current) # left vertex 

#     current = length(children) + 1
#     push!(children[parent], current)
#     _expandVer4(children, text, style, bub.Rver, current) # right vertex
# end

# function _expandVer4(children, text, style, ver4::Ver4, parent)
#     push!(children, zeros(Int, 0))
#     @assert parent == length(children)
#     # println("Ver4: $(ver4.level), Bubble: $(length(ver4.bubble))")
#     if ver4.loopNum > 0
#         info = "O$(ver4.loopNum)\nT[$(length(ver4.Tpair))]\n"
#         for t in ver4.Tpair
#             info *= "[$(t[1]) $(t[2]) $(t[3]) $(t[4])]\n"
#         end
#     else
#         info = "O$(ver4.loopNum)"
#     end
#     push!(text, info)

#     ver4.inBox ? push!(style, "stroke-dasharray:3,2") : push!(style, "")

#     for bub in ver4.bubble
#         # println(bub.chan)
#         current = length(children) + 1
#         push!(children[parent], current)
#         _expandBubble(children, text, style, bub, current)
#     end
# end

# function visualize(ver4::Ver4)
#     children, text, style = (Vector{Vector{Int}}(undef, 0), [], [])
#     _expandVer4(children, text, style, ver4, 1)

#     # text = ["one\n(second line)", "2", "III", "four"]
#     # style = ["", "fill:red", "r:14", "opacity:0.7"]
#     # link_style = ["", "stroke:blue", "", "stroke-width:10px"]
#     tooltip = ["pops", "up", "on", "hover"]
#     t = D3Trees.D3Tree(
#         children,
#         text=text,
#         style=style,
#         tooltip=tooltip,
#         # link_style = link_style,
#         title="Vertex4 Tree",
#         init_expand=2,
#     )

#     D3Trees.inchrome(t)
# end