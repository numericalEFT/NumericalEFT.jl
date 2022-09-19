using PythonCall

gf = pyimport("triqs.gf")
lat = pyimport("triqs.lattice")
tb = pyimport("triqs.lattice.tight_binding")
np = pyimport("numpy")
tpl = pyimport("triqs.plot.mpl_interface")
plt = pyimport("matplotlib.pyplot")

BL = lat.BravaisLattice(units=((1, 0, 0), (0, 1, 0))) #square lattice
# the following also works
# BL = lat.BravaisLattice(units=pylist([(1, 0, 0), (0, 1, 0)])) #square lattice
# but this will not work (you can not directly pass a julia vector as a python-list argument
# BL = lat.BravaisLattice(units=[(1, 0, 0), (0, 1, 0)]) #square lattice

nk = 20
mk = gf.MeshBrillouinZone(lat.BrillouinZone(BL), nk)
miw = gf.MeshImFreq(beta=1.0, S="Fermion", n_max=100) #grid number : 201
mprod = gf.MeshProduct(mk, miw)

G_w = gf.GfImFreq(mesh=miw, target_shape=[1, 1]) #G_w.data.shape will be [201, 1, 1]
G_k_w = gf.GfImFreq(mesh=mprod, target_shape=[1, 1]) #G_k_w.data.shape will be [400, 201, 1, 1]

t = 1.0
U = 4.0

G_k_w.data.fill(0.0)

####### fill the Green's function with data ################
## only make small number of allocations: 0.534508 seconds (28.01 k allocations: 522.039 KiB)
@time for (ik, k) in enumerate(G_k_w.mesh[0])
    G_w << gf.inverse(gf.iOmega_n - 2 * t * (np.cos(k[0]) + np.cos(k[1])))
    # G_k_w.data[ik-1, pyslice(nothing), 0, 0] = G_w.data[pyslice(nothing), 0, 0] #pyslice(nothing) == :, maybe there is a better way to do this
    G_k_w.data[ik-1, pyslice(0, -1), 0, 0] = G_w.data[pyslice(0, -1), 0, 0] #pyslice(nothing) == :, maybe there is a better way to do this
end

# plt.figure()
# gs = plt.GridSpec(2, 2)
# plt.subplot(gs[0])
tpl.oplot(G_w, '-')
# plt.show()  # figures pop up, but freeze and can't be closed.

# the following plots throw an error
# @py tpl.oplot(G_k_w[0, 0], path=pylist([(0, 0), (np.pi, np.pi), (np.pi, 0), (0, 0)]), method="cubic", mode="I")
# as well as,
# tpl.oplot(G_k_w[0, 0], path=pylist([(0, 0), (np.pi, np.pi), (np.pi, 0), (0, 0)]), method="cubic", mode="I")