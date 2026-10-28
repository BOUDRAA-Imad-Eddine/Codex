import numpy as np

# bra-ket notation and basic representation

# State |0> as a column vector (shape: 2x1)
ket_0 = np.array([[1], 
                  [0]], dtype=complex)

# State |1> as a column vector
ket_1 = np.array([[0], 
                  [1]], dtype=complex)


bra_0 = ket_0.T
bra_1 = np.array([1,0],dtype=complex)

print("Shape of ket_0:", ket_0.shape)  # Output: (2, 1)
print("Shape of bra_0:", bra_0.shape)
print("Shape of ket_1:", ket_1.shape)
print("Shape of bra_1:", bra_1.shape)


#operations on vectors
# Define a state with complex amplitudes: |psi> = (3 + 4i)|0> + i|1>
psi = np.array([[3 + 4j], 
                [0 + 1j]], dtype=complex)

# Hermitian conjugate: <psi|
psi_dagger = psi.conj().T

print("Psi Dagger Shape:", psi_dagger.shape)  # Output: (1, 2)
print("Psi Dagger Values:", psi_dagger)       # Output: [[ 3.-4.j -0.-1.j]]

# Let's calculate the inner product <0|psi>
# bra_0 shape is (1, 2), psi shape is (2, 1) -> result is (1, 1) scalar matrix
inner_prod = bra_0 @ psi
print("Inner Product:", inner_prod[0, 0])

# Outer product |0> <1|
# ket_0 shape (2,1) @ bra_1 shape (1,2) -> result shape (2,2)
bra_1 = np.array([[0, 1]], dtype=complex)
outer_prod = ket_0 @ bra_1

print("Outer Product Matrix:\n", outer_prod)
# Output:
# [[0 1]
#  [0 0]]