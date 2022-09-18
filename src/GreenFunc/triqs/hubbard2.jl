using PyCall

gf = pyimport("triqs.gf")
lat = pyimport("triqs.lattice")
tb = pyimport("triqs.lattice.tight_binding")
np = pyimport("numpy")
tpl = pyimport("triqs.plot.mpl_interface")
plt = pyimport("matplotlib.pyplot")

BL = lat.BravaisLattice(units=[(1, 0, 0), (0, 1, 0)]) #square lattice

nk = 20
mk = gf.MeshBrillouinZone(lat.BrillouinZone(BL), nk)
miw = gf.MeshImFreq(beta=1.0, S="Fermion", n_max=100) #grid number : 201
mprod = gf.MeshProduct(mk, miw)

G_w = gf.GfImFreq(mesh=miw, target_shape=(1, 1)) #G_w.data.shape will be [201, 1, 1]
G_k_w = gf.GfImFreq(mesh=mprod, target_shape=(1, 1)) #G_k_w.data.shape will be [400, 201, 1, 1]

t = 1.0
U = 4.0

# create julia reference array to the python array, otherwise, PyCall will eagerly copy narray to a julia array
Gkw_jll = PyArray(G_k_w."data")
Gw_jll = PyArray(G_w."data")

fill!(Gkw_jll, 0.0)

####### fill the Green's function with data ################

# more allocations, cost 0.780893 seconds (28.01 k allocations: 2.100 MiB)
@time for (ik, k) in enumerate(G_k_w.mesh[1])
    G_w << gf.inverse(gf.iOmega_n - 2 * t * (np.cos(k[1]) + np.cos(k[2])))
    # G_k_w.data[ik-1, pyslice(nothing), 0, 0] = G_w.data[pyslice(nothing), 0, 0] #pyslice(nothing) == :, maybe there is a better way to do this
    Gkw_jll[ik, :, 1, 1] = Gw_jll[:, 1, 1]
    # G_k_w."data"[ik, :, 1, 1] = G_w."data"[:, 1, 1] #pyslice(nothing) == :, maybe there is a better way to do this
end

## works without any problem
tpl.oplot(G_w, "-")
plt.show()  # figures pop up, and doesn't freeze.

## the following plot raises an error: ERROR: PyError ($(Expr(:escape, :(ccall(#= /Users/kunchen/.julia/packages/PyCall/ygXW2/src/pyfncall.jl:43 =# @pysym(:PyObject_Call), PyPtr, (PyPtr, PyPtr, PyPtr), o, pyargsptr, kw))))) <class 'TypeError'>
## TypeError('exceptions must derive from BaseException')
# tpl.oplot(G_k_w[0, 0], path=[(0, 0), (np.pi, np.pi), (np.pi, 0), (0, 0)], method="cubic", mode="I")
