import math
from qiskit import QuantumCircuit, transpile
from qiskit_ibm_runtime import QiskitRuntimeService, SamplerV2 as Sampler


# ─── CONFIGURATION ────────────────────────────────────────────────────────────
TARGET_N       = 156                          # Number of qubits
SHOTS          = 4096                         # Number of shots for real hardware
BACKEND_NAME   = None                         # Set to a specific backend name (e.g. "ibm_brisbane") or None to find least busy
# ──────────────────────────────────────────────────────────────────────────────

def build_ghz_binary_fan_out(n: int) -> QuantumCircuit:
    """Builds the N-qubit GHZ state via a binary fan-out tree."""
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

def main():
    # 1. Initialize IBM Quantum Service (requires saved account credentials)
    service = QiskitRuntimeService(channel="ibm_quantum_platform")

    # 2. Build circuit
    print(f"Building {TARGET_N}-qubit GHZ circuit...")
    qc = build_ghz_binary_fan_out(TARGET_N)

    # 3. Select a real hardware backend
    if BACKEND_NAME:
        backend = service.backend(BACKEND_NAME)
    else:
        # Automatically find the least busy operational real quantum computer with enough qubits
        backend = service.least_busy(simulator=False, operational=True, min_num_qubits=TARGET_N)
    
    print(f"Selected backend: {backend.name}")

    # 4. Transpile circuit for the hardware coupling map and basis gates
    # Note: Optimization level 3 helps minimize depth and SWAP counts where possible
    print("Transpiling circuit for real hardware topology...")
    transpiled_qc = transpile(qc, backend=backend, optimization_level=3)

    # Inspect transpilation metrics
    ops = dict(transpiled_qc.count_ops())
    swap_count = ops.get("swap", 0)
    print(f"Transpiled gate depth: {transpiled_qc.depth()}")
    print(f"Inserted SWAP count: {swap_count}")

    # 5. Set up Sampler V2 primitive
    sampler = Sampler(mode=backend)

    # 6. Run job on real hardware
    print(f"Submitting job to real hardware ({backend.name}) with {SHOTS} shots...")
    job = sampler.run([transpiled_qc], shots=SHOTS) 
    print(f"Job ID: {job.job_id()}")
    print("Job submitted successfully! Waiting for results (this may take time depending on queue length)...")

    # 7. Fetch results
    result = job.result()
    pub_result = result[0]
    counts = pub_result.data.c.get_counts()

    # 8. Calculate experimental correlation (C)
    all_zeros = "0" * TARGET_N
    all_ones = "1" * TARGET_N
    total_shots = sum(counts.values())
    correlated = counts.get(all_zeros, 0) + counts.get(all_ones, 0)
    C = correlated / total_shots

    print(f"\nExecution Results:")
    print(f"Total Shots: {total_shots}")
    print(f"Experimental Correlation (C): {C:.6f}")
    print(f"Counts snippet: {list(counts.items())[:5]}")

if __name__ == "__main__":
    main()