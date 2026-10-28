import numpy as np


z1 = 3 + 4j          # Python native complex
z2 = np.complex128(3 + 4j)  # NumPy complex

# Basic operations

print(z1.real)       # 3.0
print(z1.imag)       # 4.0
print(z1.conjugate())# (3-4j)
print(abs(z1))       # 5.0  (modulus)

# Euler's formula
theta = np.pi / 4
z = np.exp(1j * theta)
print(z)             # (0.70710678+0.70710678j)
print(np.cos(theta) + 1j * np.sin(theta))  # Same thing
