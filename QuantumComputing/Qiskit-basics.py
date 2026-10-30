from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator
from qiskit.transpiler.preset_passmanagers import generate_preset_pass_manager

# 1. Create a quantum circuit with 2 qubits and 2 classical bits
# (Classical bits are used to store the measurement results)
qc = QuantumCircuit(2, 2)

# 2. Apply gates
# Hadamard gate on qubit 0 (puts it into a superposition state)
qc.h(0)

# CNOT (Controlled-NOT) gate with control qubit 0 and target qubit 1 (entangles the qubits)
qc.cx(0, 1)

# 3. Measure both qubits
qc.measure([0, 1], [0, 1])

# Draw the circuit
print(qc.draw())


# Initialize the local simulator backend
backend = AerSimulator()

# Transpile the circuit for the backend
pass_manager = generate_preset_pass_manager(optimization_level=1, backend=backend)
isa_circuit = pass_manager.run(qc)

# Run the sampler primitive
from qiskit_ibm_runtime import SamplerV2 as Sampler
# For local simulation without IBM cloud setup, we can use Aer's sampler:
from qiskit.primitives import StatevectorSampler

sampler = StatevectorSampler()
job = sampler.run([isa_circuit])
result = job.result()

# Extract and print counts/probabilities
pub_result = result[0]
print(pub_result.data.meas.get_counts())