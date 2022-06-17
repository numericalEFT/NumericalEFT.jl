import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import Delaunay
from scipy.interpolate import NearestNDInterpolator
from scipy import optimize

plt.gca().set_aspect('equal')
plt.style.use('science')

grid = np.loadtxt("basis.dat")
finegrid = np.loadtxt("finegrid.dat")
Nfine = len(finegrid)
residual = np.loadtxt("residual.dat")

print(len(residual))
print(len([residual[i]
           for i in range(len(residual)) if np.sqrt(residual[i]) > 0.2e-9]))

# print("grid: ", list(zip(finegrid, finegrid)))
test = [np.array([x, y])
        for x in finegrid for y in finegrid]
interp = NearestNDInterpolator(test, -residual)


def interplate(x0):
    # print("inter ", x0)
    # print("got ", interp([np.array([x0[0], x0[1]])]))
    return interp(x0[0], x0[1])


residual = np.sqrt(np.reshape(residual, (Nfine, Nfine)))

xv, yv = np.meshgrid(finegrid, finegrid)
# plt.imshow(xv, yv, residual)
plt.contourf(xv, yv, residual, 16)
plt.colorbar()


<<<<<<< HEAD
# tri = Delaunay(np.log(grid), qhull_options="QJ")
=======
tri = Delaunay(np.log(grid), qhull_options="QJ")
>>>>>>> b7b189f8832d73d71f65147bc5276a44071e4823
# plt.triplot(grid[:, 0], grid[:, 1], tri.simplices)

# print(len(grid))
# print(len(tri.simplices))
# print(tri.simplices[0])

# coordx = []
# coordy = []

<<<<<<< HEAD
# for sim in tri.simplices:
#     initial = [grid[s] for s in sim]
#     x = (initial[0][0]+initial[1][0]+initial[2][0])/3
#     y = (initial[0][1]+initial[1][1]+initial[2][1])/3
#     # initial.append([x, y])
#     x0 = np.array([x, y])
#     initial = np.array(initial)
#     # print(initial.shape)
#     mimum = optimize.fmin(interplate, x0,
#                           xtol=0.05, ftol=1e-10, initial_simplex=initial)
#     coordx.append(mimum[0])
#     coordy.append(mimum[1])
# print(initial)


plt.scatter(grid[:, 0], grid[:, 1], c="yellow", alpha=0.5, s=6)
# plt.scatter(coordx, coordy, c="red", alpha=0.5, s=8)
=======
for sim in tri.simplices:
    initial = [grid[s] for s in sim]
    x = (initial[0][0]+initial[1][0]+initial[2][0])/3
    y = (initial[0][1]+initial[1][1]+initial[2][1])/3
    # initial.append([x, y])
    x0 = np.array([x, y])
    initial = np.array(initial)
    # print(initial.shape)
    # mimum = optimize.fmin(interplate, x0,
    #                       xtol=0.05, ftol=1e-10, initial_simplex=initial)
    # coordx.append(mimum[0])
    # coordy.append(mimum[1])
    # print(initial)


plt.scatter(grid[:, 0], grid[:, 1], c="red", alpha=0.5, s=6)
plt.scatter(coordx, coordy, c="red", alpha=0.5, s=15)
>>>>>>> b7b189f8832d73d71f65147bc5276a44071e4823

plt.xscale("log")
plt.yscale("log")
plt.xlabel("$\\omega_1$")
plt.ylabel("$\\omega_2$")
plt.savefig("residual.pdf")
plt.show()
