from qiskit import QuantumCircuit
from qiskit.quantum_info import Statevector

# Create a circuit with 1 qubit
qc = QuantumCircuit(1)


qc.h(0)
qc.z(0)

# Extract and inspect the statevector
state = Statevector(qc)

print("Initial State (|0⟩):", state)

print(state.draw(output = "text"))
