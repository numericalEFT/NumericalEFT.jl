import matplotlib.pyplot as plt
import numpy as np

plt.gca().set_aspect('equal')
plt.style.use('science')

grid = np.loadtxt("basis.dat")
finegrid = np.loadtxt("finegrid.dat")
Nfine = len(finegrid)
residual = np.loadtxt("residual.dat")
residual = np.sqrt(np.reshape(residual, (Nfine, Nfine)))

xv, yv = np.meshgrid(finegrid, finegrid)
# plt.imshow(xv, yv, residual)
plt.contourf(xv, yv, residual, 16)
plt.colorbar()

plt.scatter(grid[:, 0], grid[:, 1], c="yellow", alpha=0.5, s=6)
plt.xscale("log")
plt.yscale("log")
plt.xlabel("$\\omega_1$")
plt.ylabel("$\\omega_2$")
plt.savefig("residual.pdf")
plt.show()
