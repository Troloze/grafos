import matplotlib.pyplot as plt
import matplotlib.scale as mscale
import matplotlib.transforms as mtransforms
import matplotlib.ticker as ticker
from mpl_toolkits import mplot3d
import numpy as np
from statistics import mean 
import re

class SquareRootScale(mscale.ScaleBase):
    """
    ScaleBase class for generating square root scale.
    """
 
    name = 'squareroot'
 
    def __init__(self, axis, **kwargs):
        # note in older versions of matplotlib (<3.1), this worked fine.
        # mscale.ScaleBase.__init__(self)

        # In newer versions (>=3.1), you also need to pass in `axis` as an arg
        mscale.ScaleBase.__init__(self, axis)
 
    def set_default_locators_and_formatters(self, axis):
        axis.set_major_locator(ticker.AutoLocator())
        axis.set_major_formatter(ticker.ScalarFormatter())
        axis.set_minor_locator(ticker.NullLocator())
        axis.set_minor_formatter(ticker.NullFormatter())
 
    def limit_range_for_scale(self, vmin, vmax, minpos):
        return  max(0., vmin), vmax
 
    class SquareRootTransform(mtransforms.Transform):
        input_dims = 1
        output_dims = 1
        is_separable = True
 
        def transform_non_affine(self, a): 
            return np.array(a)**0.5
 
        def inverted(self):
            return SquareRootScale.InvertedSquareRootTransform()
 
    class InvertedSquareRootTransform(mtransforms.Transform):
        input_dims = 1
        output_dims = 1
        is_separable = True
 
        def transform(self, a):
            return np.array(a)**2
 
        def inverted(self):
            return SquareRootScale.SquareRootTransform()
 
    def get_transform(self):
        return self.SquareRootTransform()

mscale.register_scale(SquareRootScale)

f = open("Results.txt", "r")
reg = r"<order:(?P<order>[0-9]+), edges:(?P<edges>[0-9]+), delta:(?P<delta>[0-9]+), colors: (?P<colors>[0-9]+), graph:(?P<graph>[0-9]+), type:(?P<type>[0-3]), time:(?P<time>[0-9]+\.[0-9]+)>"
matches = re.findall(reg, f.read())
f.close()

f = open("Results2.txt", 'r')
matches2 = re.findall(reg, f.read())
f.close()
complete = [[],[]]

for i in matches2:
    app_time = float(i[6])
    match int(i[5]):
        case 0:
            complete[0].append(app_time)
        case 1:
            complete[1].append(app_time)

print(complete)

ct0 = 0
ct1 = 0
ct2 = 0
ct3 = 0
type_0_delta = []
type_1_delta = []
type_2_delta = []
type_3_delta = []
type_2_delta_total = []
type_3_delta_total = []
for i in range(0, 3):
    type_0_delta.append([])
    type_1_delta.append([])
    type_2_delta.append([])
    type_3_delta.append([])
    type_2_delta_total.append([])
    type_3_delta_total.append([])

type_0_order_m = []
type_1_order_m = []
type_2_order_m = []
type_3_order_m = []
for i in range(0, 7):
    type_0_order_m.append([])
    type_1_order_m.append([])
    type_2_order_m.append([])
    type_3_order_m.append([])
    
type_2_total_m = []
type_3_total_m = []
for i in range(0, 14):
    type_2_total_m.append([])
    type_3_total_m.append([])


max_delta = 0
min_delta = 1000
max_order = 0
min_order = 1000
for i in matches:
    app_time = float(i[6])
    app_order = int(i[0])
    app_total = int(i[0]) + int(i[1])
    app_delta = int(i[2])
    if app_order > max_order:
        max_order = app_order
    if app_order < min_order:
        min_order = app_order
    if app_delta > max_delta:
        max_delta = app_delta
    if app_delta < min_delta:
        min_delta = app_delta
    match int(i[5]):
        case 0:
            ct0 += 1
            type_0_order_m[app_order - 2].append(app_time)
            type_0_delta[0].append(app_order)
            type_0_delta[1].append(app_delta)
            type_0_delta[2].append(app_time)
        case 1:
            ct1 += 1
            type_1_order_m[app_order - 2].append(app_time)
            type_1_delta[0].append(app_order)
            type_1_delta[1].append(app_delta)
            type_1_delta[2].append(app_time)
        case 2:
            ct2 += 1
            type_2_order_m[app_order - 2].append(app_time)
            type_2_total_m[app_total - 2].append(app_time)
            type_2_delta[0].append(app_order)
            type_2_delta[1].append(app_delta)
            type_2_delta[2].append(app_time)
            type_2_delta_total[0].append(app_total)
            type_2_delta_total[1].append(app_delta)
            type_2_delta_total[2].append(app_time)
        case 3:
            ct3 += 1
            type_3_order_m[app_order - 2].append(app_time)  
            type_3_total_m[app_total - 2].append(app_time) 
            type_3_delta[0].append(app_order)
            type_3_delta[1].append(app_delta)
            type_3_delta[2].append(app_time)
            type_3_delta_total[0].append(app_total)
            type_3_delta_total[1].append(app_delta)
            type_3_delta_total[2].append(app_time)

type_0_mean = []
type_1_mean = []
type_2_mean = []
type_3_mean = []
type_2t_mean = []
type_3t_mean = []

type_0_delta_m = []

type_1_delta_m = []
type_2_delta_m = []
type_3_delta_m = []
type_2t_delta_m = []
type_3t_delta_m = []
delta_order = max_order - min_order + 1
delta_delta = max_delta - min_delta + 1
for i in range(0, delta_order):
    type_0_delta_m.append([])
    type_1_delta_m.append([])
    for j in range(0, delta_delta):
        type_0_delta_m[i].append([.0, 0])
        type_1_delta_m[i].append([.0, 0])

for i in range(0, ct0):
    type_0_delta_m[type_0_delta[0][i] - min_order][type_0_delta[1][i] - min_delta][0] += type_0_delta[2][i]
    type_0_delta_m[type_0_delta[0][i] - min_order][type_0_delta[1][i] - min_delta][1] += 1

for i in range(0, ct1):
    type_1_delta_m[type_1_delta[0][i] - min_order][type_1_delta[1][i] - min_delta][0] += type_1_delta[2][i]
    type_1_delta_m[type_1_delta[0][i] - min_order][type_1_delta[1][i] - min_delta][1] += 1
 
#--

for i in range(0, delta_order):
    for j in range(0, delta_delta):
        if type_0_delta_m[i][j][1] != 0:
            type_0_delta_m[i][j] = type_0_delta_m[i][j][0]/type_0_delta_m[i][j][1]
        else:
            type_0_delta_m[i][j] = 0
        if type_1_delta_m[i][j][1] != 0:
            type_1_delta_m[i][j] = type_1_delta_m[i][j][0]/type_1_delta_m[i][j][1]
        else: 
            type_1_delta_m[i][j] = 0

t0Arr = np.array(type_0_delta_m)
t1Arr = np.array(type_1_delta_m)


for i in range(0, 7):
    type_0_mean.append(mean(type_0_order_m[i]))
    type_1_mean.append(mean(type_1_order_m[i]))
    type_2_mean.append((mean(type_2_order_m[i])))
    type_3_mean.append((mean(type_3_order_m[i])))

for i in range(0, 14):
    if len(type_2_total_m[i]) == 0:
        type_2t_mean.append(0)
        type_3t_mean.append(0)
        continue
    type_2t_mean.append((mean(type_2_total_m[i])))
    type_3t_mean.append((mean(type_3_total_m[i])))


orders = [2, 3, 4, 5, 6, 7, 8]
total = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#"""
plt.plot(orders, type_1_mean, label = "Iterador de configuração única")
plt.plot(orders, type_0_mean, label = "Iterador com bloqueio")
plt.xlabel("Ordem")
plt.ylabel("Tempo")
plt.title("Coloração de Vértices")
plt.legend()
plt.show()

plt.plot(orders, type_3_mean, label = "Iterador de configuração única")
plt.plot(orders, type_2_mean, label = "Iterador com bloqueio")
plt.xlabel("Ordem")
plt.ylabel("Tempo")
plt.title("Coloração total")
plt.legend()
plt.show()

plt.plot(total, type_3t_mean, label = "Iterador de configuração única")
plt.plot(total, type_2t_mean, label = "Iterador com bloqueio")
plt.xlabel("Elementos (nº de Vértices + Arestas)")
plt.ylabel("Tempo")
plt.title("Coloração total")
plt.legend()
plt.show()


t1 = [3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
t2 = [3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]

plt.plot(t2, complete[1], label = "Iterador de configuração única")
plt.plot(t1, complete[0], label = "Iterador com bloqueio")
plt.xlabel("Ordem")
plt.ylabel("Tempo")
plt.title("Coloração de vértices (grafos completos)")
plt.legend()
plt.show()



"""
fig = plt.figure(figsize = (12,10))
ax = plt.axes(projection='3d')

X = np.array(range(min_order, max_order + 1))
Y = np.array(range(min_delta, max_delta + 1))
X, Y = np.meshgrid(X, Y)
Z = t0Arr
#Z = t1Arr - t0Arr
surf = ax.plot_surface(X, Y, Z, cmap = plt.cm.cividis)

# Set axes label
ax.set_xlabel('Order', labelpad=20)
ax.set_ylabel('Delta', labelpad=20)
ax.set_zlabel('Time', labelpad=20)
plt.title("Coloração Vértices (Iterador com Bloqueio)")
plt.show()

fig = plt.figure(figsize = (12,10))
ax = plt.axes(projection='3d')

Z = t1Arr
surf = ax.plot_surface(X, Y, Z, cmap = plt.cm.cividis)

# Set axes label
ax.set_xlabel('Order', labelpad=20)
ax.set_ylabel('Delta', labelpad=20)
ax.set_zlabel('Time', labelpad=20)
plt.title("Coloração Vértices (Iterador de Configuração Única)")

plt.show()

#"""

