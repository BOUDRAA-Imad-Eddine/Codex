from qiskit import QuantumCircuit, qasm2
import math

N = 156

def build_ghz_binary_fan_out(n):
    qc = QuantumCircuit(n, n, name=f"GHZ-{n}-binary-fan-out")
    qc.h(0)
    entangled = 1
    while entangled < n:
        new = min(entangled, n - entangled)
        for i in range(new):
            qc.cx(i, entangled + i)
        entangled += new
    qc.measure(range(n), range(n))
    return qc

qc = build_ghz_binary_fan_out(N)

# Save the QASM file
filename = f"ghz_{N}_qubits.qasm"
with open(filename, "w") as f:
    f.write(qasm2.dumps(qc))

print(f"Saved: {filename}")
print(f"Size: {len(qasm2.dumps(qc)):,} bytes")