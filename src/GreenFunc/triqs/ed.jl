using PythonCall
gf = pyimport("triqs.gf")
op = pyimport("triqs.operators")
atom = pyimport("triqs.atom_diag")
# ham  from triqs.operators.util.hamiltonians import h_int_kanamori
ham = pyimport("triqs.operators.util.hamiltonians")
h5 = pyimport("h5")
np = pyimport("numpy")
iter = pyimport("itertools")

# Definition of a 3-orbital atom
spin_names = ("up", "dn")
orb_names = (0, 1, 2)
# Set of fundamental operators
fops = [(sn, on) for (sn, on) in Iterators.product(spin_names, orb_names)]
println(fops)

# Numbers of particles with spin up/down
N_up = op.n("up", 0) + op.n("up", 1) + op.n("up", 2)
N_dn = op.n("dn", 0) + op.n("dn", 1) + op.n("dn", 2)

# Construct Hubbard-Kanamori Hamiltonian
U = 3.0 * np.ones((3, 3))
Uprime = 2.0 * np.ones((3, 3))
J_hund = 0.5

H = ham.h_int_kanamori(spin_names, orb_names, U, Uprime, J_hund, true)

# Add chemical potential
H += -4.0 * (N_up + N_dn)

# Add hopping terms between orbitals 0 and 1 with some complex amplitude
H += 0.1im * (op.c_dag("up", 0) * op.c("up", 1) - op.c_dag("up", 1) * op.c("up", 0))
H += 0.1im * (op.c_dag("dn", 0) * op.c("dn", 1) - op.c_dag("dn", 1) * op.c("dn", 0))

# Split H into blocks and diagonalize it using N_up and N_dn quantum numbers
ad = atom.AtomDiag(H, fops, [N_up, N_dn])
println(ad.n_subspaces) # Number of invariant subspaces, 4 * 4 = 16

# Now split using the total number of particles, N = N_up + N_dn
ad = atom.AtomDiag(H, fops, [N_up + N_dn])
println(ad.n_subspaces) # 7

# Split the Hilbert space automatically
ad = atom.AtomDiag(H, fops)
print(ad.n_subspaces) # 28

# Partition function for inverse temperature \beta=3
beta = 3
println(atom.partition_function(ad, beta))

# Equilibrium density matrix
dm = atom.atomic_density_matrix(ad, beta)

# Expectation values of orbital double occupancies
println(atom.trace_rho_op(dm, op.n("up", 0) * op.n("dn", 0), ad))
println(atom.trace_rho_op(dm, op.n("up", 1) * op.n("dn", 1), ad))
println(atom.trace_rho_op(dm, op.n("up", 2) * op.n("dn", 2), ad))

# Atomic Green's functions
# gf_struct = pylist([("dn", pylist(orb_names)), ("up", pylist(orb_names))])
@py gf_struct = [["dn", 3], ["up", 3]] #triqs new api only require one index of the target_shape
@py G_w = atom.atomic_g_w(ad, beta, gf_struct, (-2, 2), 400, 0.01)

G_tau = atom.atomic_g_tau(ad, beta, gf_struct, 400)
G_iw = atom.atomic_g_iw(ad, beta, gf_struct, 100)
G_l = atom.atomic_g_l(ad, beta, gf_struct, 20)

# Finally, we save our AtomDiag object for later use
pywith(h5.HDFArchive("atom_diag_example.h5")) do ar
    ar["ad"] = ad
end