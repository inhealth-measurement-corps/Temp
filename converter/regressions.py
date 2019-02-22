import matplotlib 
import matplotlib.pyplot as plt

#matplotlib.interactive(True)
xs = [1, 2, 3, 4, 5,6,7,8,9,10,11,12,14]
ys = [10, 12, 14, 15, 17, 18, 20, 21, 22, 25, 26, 28, 30]

plt.scatter(xs, ys)
plt.ylabel("dv")
plt.xlabel("iv")
plt.show()