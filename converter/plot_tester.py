import matplotlib.pyplot as plt
from scipy import stats
import numpy as np
np.random.seed(12345678)
x = np.random.random(10)
y = np.random.random(10)
slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)

print("x:", x)
print("y:", y)

print("type of x:", type(x))
print("type of y:", type(y))
print("type of slope:", type(slope))
print("type of intercept:", type(intercept))

plt.plot(x, y, 'o', label='original data')
plt.plot(x, intercept + slope*x, 'r', label='fitted line')
plt.legend()
plt.show()
plt.close()