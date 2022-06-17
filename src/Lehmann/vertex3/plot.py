import matplotlib.pyplot as plt
# from scipy import interpolate
import numpy as np
from scipy.integrate import simps

plt.gca().set_aspect('equal')
plt.style.use('science')


grid = np.loadtxt("basis.dat")
finegrid = np.loadtxt("finegrid.dat")
Nfine = len(finegrid)
residual = np.loadtxt("residual.dat")
residual = np.reshape(residual, (Nfine, Nfine))

# f = interpolate.interp2d(finegrid, finegrid, residual, kind='linear')
total = simps(simps(residual, finegrid), finegrid)
print("total residual: ", np.sqrt(total))

error = np.sqrt(residual)

dg = grid[1, :]-grid[0, :]
shift = np.sqrt(sum(dg*dg))

xv, yv = np.meshgrid(finegrid+shift, finegrid+shift)
# plt.imshow(xv, yv, residual)
plt.contourf(xv, yv, error.transpose(), 16)
plt.colorbar()

plt.scatter(grid[:, 0]+shift, grid[:, 1]+shift, c="yellow", alpha=0.5, s=6)
plt.xlim([shift, finegrid[-1]+shift])
plt.ylim([shift, finegrid[-1]+shift])
# plt.xlim([1.0, finegrid[-1]+shift])
# plt.ylim([1.0, finegrid[-1]+shift])
plt.xscale("log")
plt.yscale("log")
plt.xlabel("$\\omega_1$")
plt.ylabel("$\\omega_2$")
# plt.savefig("residual.pdf")
plt.show()
