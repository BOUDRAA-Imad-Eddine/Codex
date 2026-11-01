import math
import numpy as np
from qiskit import QuantumCircuit, qasm2
from qiskit_aer import AerSimulator
from qiskit_aer.noise import NoiseModel, depolarizing_error
import mthree

# Configuration
SEARCH_RANGE   = range(60, 130, 4)  # N values to test
SHOTS          = 8192
SCAN_SHOTS     = 4096               # Fewer shots for the fast scan
SEED           = 42

def build_ghz_binary_fan_out(n: int) -> QuantumCircuit:
    qc = QuantumCircuit(n, n, name=f"GHZ-{n}-tree")
    qc.h(0)
    entangled = 1
    while entangled < n:
        new = min(entangled, n - entangled)
        for i in range(new):
            qc.cx(i, entangled + i)
        entangled += new
    qc.measure(range(n), range(n))
    return qc

def build_noise_model(n: int) -> NoiseModel:
    nm = NoiseModel()
    # Exact noise levels from the challenge
    nm.add_all_qubit_quantum_error(depolarizing_error(0.001, 1), ["x", "sx", "rz", "h"])
    nm.add_all_qubit_quantum_error(depolarizing_error(0.015, 2), ["cx", "swap"])
    return nm

def compute_score(circuit, correlation):
    N        = circuit.num_qubits
    ops      = dict(circuit.count_ops())
    has_meas = ops.get("measure", 0) > 0
    gate_depth = circuit.depth() - (1 if has_meas else 0)
    swap_count = ops.get("swap", 0)
    C_rand     = 1.0 / (2.0 ** (N - 1))
    base       = 1000.0 + 50.0 * (N ** 2 - 4)
    D_min      = 1 + math.ceil(math.log2(N))

    if correlation <= C_rand:
        return 0

    F = ((correlation - C_rand) / (1.0 - C_rand)) ** 2
    S = 1.0 / (1.0 + 0.25 * swap_count)
    D = D_min / max(gate_depth, D_min)
    total = round(base * F * S * D)
    return total

def fast_mitigate(raw_counts, n):
    """
    Fast approximation for the scan. We estimate the baseline readout error 
    (roughly 2-3% per qubit) and apply it.
    """
    total = sum(raw_counts.values())
    r0 = raw_counts.get("0"*n, 0) / total
    r1 = raw_counts.get("1"*n, 0) / total
    
    # Estimate the readout attenuation factor (assume ~2.5% error per qubit)
    att = (1 - 0.025) ** n
    
    # Correct the probabilities
    c0 = r0 / att
    c1 = r1 / att
    
    return min(c0 + c1, 1.0)

def find_optimal_n():
    print(f"\nPhase 1: Scanning N from {list(SEARCH_RANGE)[0]} to {list(SEARCH_RANGE)[-1]} (Fast Approximation)...")
    print(f"  {'N':>4} | {'C_raw':>7} | {'C_approx':>8} | {'Score':>9}")
    print("-" * 38)
    
    best_score = 0
    best_n = list(SEARCH_RANGE)[0]

    for N in SEARCH_RANGE:
        qc = build_ghz_binary_fan_out(N)
        sim = AerSimulator(noise_model=build_noise_model(N), seed_simulator=SEED)
        raw_counts = sim.run(qc, shots=SCAN_SHOTS).result().get_counts()
        
        raw_C = (raw_counts.get("0"*N, 0) + raw_counts.get("1"*N, 0)) / SCAN_SHOTS
        approx_C = fast_mitigate(raw_counts, N)
        score = compute_score(qc, approx_C)
        
        marker = " <- best" if score > best_score else ""
        print(f"  {N:>4} | {raw_C:>7.4f} | {approx_C:>8.4f} | {score:>9,}{marker}")
        
        if score > best_score:
            best_score = score
            best_n = N

    print("-" * 38)
    print(f"Optimal N found: {best_n} (Approx Score: {best_score:,})")
    return best_n

def main():
    # --- Phase 1: Find Optimal N ---
    optimal_n = find_optimal_n()
    N = optimal_n
    
    # --- Phase 2: Full M3 Correction on the optimal N ---
    print(f"\n" + "="*50)
    print(f"Phase 2: Running full mthree correction on N = {N}")
    print("="*50)
    
    qc = build_ghz_binary_fan_out(N)
    
    # 1. Run noisy simulation
    sim = AerSimulator(noise_model=build_noise_model(N), seed_simulator=SEED)
    sim.set_max_qubits(N)
    print(f"\nRunning NOISY simulation ({SHOTS} shots) ...")
    raw_counts = sim.run(qc, shots=SHOTS).result().get_counts()
    
    raw_C = (raw_counts.get("0"*N, 0) + raw_counts.get("1"*N, 0)) / SHOTS
    print(f"Raw Correlation: {raw_C:.6f}")

    # 2. Apply M3 Mitigation
    print("\nCalibrating M3 readout error...")
    mit = mthree.M3Mitigation(sim)
    mit.cals_from_system(range(N), shots=SHOTS)
    
    print("Applying M3 correction...")
    quasis = mit.apply_correction(raw_counts, range(N))
    
    # Get corrected probability of 0...0 and 1...1
    mit_C = float(quasis.get("0"*N, 0.0)) + float(quasis.get("1"*N, 0.0))
    mit_C = max(0.0, min(1.0, mit_C))
    print(f"M3-Corrected Correlation: {mit_C:.6f}")

    # 3. Scores
    print("\n  --- RAW SCORE ---")
    score_raw = compute_score(qc, raw_C)
    print(f"  PROJECTED SCORE: {score_raw:,} pts")
    
    print("\n  --- M3-CORRECTED SCORE ---")
    score_mit = compute_score(qc, mit_C)
    print(f"  PROJECTED SCORE: {score_mit:,} pts")

    # 4. Generate and save QASM
    qasm_filename = f"ghz_optimal_m3_{N}.qasm"
    print(f"\nGenerating QASM file...")
    qasm_str = qasm2.dumps(qc)
    with open(qasm_filename, "w") as f:
        f.write(qasm_str)
    
    print(f"Successfully saved QASM circuit to: {qasm_filename}")
    print(f"  (File size: {len(qasm_str)} characters)")

if __name__ == "__main__":
    main()