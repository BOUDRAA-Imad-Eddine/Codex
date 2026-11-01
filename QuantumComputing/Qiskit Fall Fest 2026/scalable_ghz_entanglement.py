
# GHZ Challenge - Honest Noisy Simulator Solution


# --- Initalize Variables ---
OPTIMAL_N      = None       
SHOTS          = 8192       # shots for the main GHZ run
CAL_SHOTS      = 4096       # shots for readout calibration
SCAN_SHOTS     = 4096       # shots used during N-search (faster)
SEARCH_RANGE   = range(60, 130, 4)   # N values to scan when OPTIMAL_N is None

import math
import sys
from qiskit import QuantumCircuit, qasm2
from qiskit_aer import AerSimulator
from qiskit_aer.noise import NoiseModel, depolarizing_error



# 1. Noise Model
def make_noise_model() -> NoiseModel:
    """
    Depolarizing noise model matching the challenge notebook:
      - 1-qubit gates (h, x, sx, rz) : error rate 0.1%
      - 2-qubit gates (cx, swap)     : error rate 1.5%
    """
    nm = NoiseModel()
    nm.add_all_qubit_quantum_error(depolarizing_error(0.001, 1), ["x", "sx", "rz", "h"])
    nm.add_all_qubit_quantum_error(depolarizing_error(0.015, 2), ["cx", "swap"])
    return nm


# 2. GHZ Circuit Construction
def build_ghz(n: int) -> QuantumCircuit:
    """
    Builds an N-qubit GHZ state via a binary fan-out tree.
    Each round doubles the set of entangled qubits, so the gate depth is
    exactly 1 + ceil(log2(n)) -- the theoretical minimum.
    """
    qc = QuantumCircuit(n, n, name=f"GHZ-{n}-fan-out")
    qc.h(0)
    entangled = 1
    while entangled < n:
        new = min(entangled, n - entangled)
        for i in range(new):
            qc.cx(i, entangled + i)
        entangled += new
    qc.measure(range(n), range(n))
    return qc


# 3. Readout-Error Mitigation
def calibrate_readout(sim: AerSimulator, shots: int) -> tuple[float, float]:
    """
    Calibrate the per-qubit readout probabilities.
    Assumes the noise is uniform across qubits, so one pair of circuits
    is enough.

    Returns:
        p0g0 : P(read 0 | prepared |0>)
        p1g1 : P(read 1 | prepared |1>)
    """
    qc0 = QuantumCircuit(1, 1)
    qc0.measure(0, 0)

    qc1 = QuantumCircuit(1, 1)
    qc1.x(0)
    qc1.measure(0, 0)

    c0 = sim.run(qc0, shots=shots).result().get_counts()
    c1 = sim.run(qc1, shots=shots).result().get_counts()

    p0g0 = c0.get("0", 0) / shots
    p1g1 = c1.get("1", 0) / shots
    return p0g0, p1g1


def mitigate_correlation(counts: dict, N: int, p0g0: float, p1g1: float) -> float:
    """
    Apply tensor-product readout correction to the raw GHZ correlation.
    For an N-qubit GHZ state, the only two relevant outcomes are |00...0> and |11...1>.
    Under a tensor-product readout model:
        P_raw(00...0) = p0g0^N * P_ideal(00...0)
        P_raw(11...1) = p1g1^N * P_ideal(11...1)
    We divide each raw probability by the attenuation factor, clip to [0, 1],
    and sum to get the mitigated correlation C.
    """
    total  = sum(counts.values())
    raw_p0 = counts.get("0" * N, 0) / total
    raw_p1 = counts.get("1" * N, 0) / total

    att0 = p0g0 ** N
    att1 = p1g1 ** N

    corr_p0 = (raw_p0 / att0) if att0 > 1e-15 else raw_p0
    corr_p1 = (raw_p1 / att1) if att1 > 1e-15 else raw_p1

    # Normalize if correction overshoots 1
    total_corr = corr_p0 + corr_p1
    if total_corr > 1.0:
        corr_p0 /= total_corr
        corr_p1 /= total_corr

    return min(corr_p0 + corr_p1, 1.0)


# ==============================================================================
# 4. Score Calculation
# ==============================================================================
def compute_score(circuit: QuantumCircuit, C: float, verbose: bool = False) -> int:
    """
    Calculates the hackathon score based on the challenge formula.
    """
    N          = circuit.num_qubits
    ops        = dict(circuit.count_ops())
    has_meas   = ops.get("measure", 0) > 0
    gate_depth = circuit.depth() - (1 if has_meas else 0)
    swap_count = ops.get("swap", 0)

    C_rand = 1.0 / (2.0 ** (N - 1))
    base   = 1000.0 + 50.0 * (N ** 2 - 4)
    D_min  = 1 + math.ceil(math.log2(N))

    if C <= C_rand:
        return 0

    norm_fidelity   = (C - C_rand) / (1.0 - C_rand)
    fidelity_factor = norm_fidelity ** 2
    swap_penalty    = 1.0 / (1.0 + 0.25 * swap_count)
    depth_eff       = D_min / max(gate_depth, D_min)
    total           = round(base * fidelity_factor * swap_penalty * depth_eff)

    if verbose:
        width = 54
        print(f"\n{'='*width}")
        print(f"  SCORE BREAKDOWN  --  {N}-qubit GHZ")
        print(f"{'='*width}")
        print(f"  Base potential          {base:>18,.0f} pts")
        print(f"  Correlation C           {C:>18.6f}")
        print(f"  Random baseline C_rand  {C_rand:>18.2e}")
        print(f"  Normalised fidelity     {norm_fidelity:>18.6f}")
        print(f"  Fidelity factor  (^2)   {fidelity_factor:>18.6f}")
        print(f"  SWAP count              {swap_count:>18d}")
        print(f"  SWAP penalty factor     {swap_penalty:>18.6f}")
        print(f"  Gate depth (actual)     {gate_depth:>18d}")
        print(f"  Gate depth (D_min)      {D_min:>18d}")
        print(f"  Depth efficiency        {depth_eff:>18.6f}")
        print(f"  {'-'*width}")
        print(f"  TOTAL SCORE             {total:>18,} pts")
        print(f"{'='*width}")

    return total


# ==============================================================================
# 5. Optimal N Search
# ==============================================================================
def find_optimal_n(sim: AerSimulator, p0g0: float, p1g1: float,
                   search_range, shots: int) -> tuple[int, int]:
    """
    Run GHZ circuits for each N in search_range and return the (N, score) pair
    that gives the highest mitigated score. Noise kills coherence faster than N^2
    raises the base, so there is a sweet spot.
    """
    print(f"\n  Scanning N in {list(search_range)[0]}...{list(search_range)[-1]} "
          f"({len(list(search_range))} values, {shots} shots each) ...\n")
    print(f"  {'N':>5}  {'C_raw':>7}  {'C_mit':>7}  {'score':>9}")
    print(f"  {'-'*36}")

    best_score = 0
    best_n     = list(search_range)[0]

    for N in search_range:
        qc     = build_ghz(N)
        counts = sim.run(qc, shots=shots).result().get_counts()
        total  = sum(counts.values())
        C_raw  = (counts.get("0"*N, 0) + counts.get("1"*N, 0)) / total
        C_mit  = mitigate_correlation(counts, N, p0g0, p1g1)
        score  = compute_score(qc, C_mit)
        marker = "  <- best" if score > best_score else ""
        print(f"  {N:>5}  {C_raw:>7.4f}  {C_mit:>7.4f}  {score:>9,}{marker}")

        if score > best_score:
            best_score = score
            best_n     = N

    print(f"\n  Optimal N = {best_n}  (score {best_score:,})")
    return best_n, best_score



# Main Execution

def main():
    sep = "=" * 56

    print(f"\n{sep}")
    print("  GHZ Challenge - Honest Competitive Solution")
    print(sep)

    noise_model = make_noise_model()
    sim = AerSimulator(noise_model=noise_model, seed_simulator=42)
    sim.set_max_qubits(OPTIMAL_N) 
    print("  Noise model  : depolarizing (1q=0.1%, 2q=1.5%)")

    # Calibrate readout
    print(f"\n  Calibrating readout errors ({CAL_SHOTS} shots) ...")
    p0g0, p1g1 = calibrate_readout(sim, CAL_SHOTS)
    print(f"  P(read 0 | prep |0>) = {p0g0:.4f}")
    print(f"  P(read 1 | prep |1>) = {p1g1:.4f}")
    print(f"  Per-qubit readout error = {(1-p0g0+1-p1g1)/2*100:.2f} %")

    # Find optimal N
    if OPTIMAL_N is None:
        N, _ = find_optimal_n(sim, p0g0, p1g1, SEARCH_RANGE, SCAN_SHOTS)
    else:
        N = OPTIMAL_N
        print(f"\n  Using pre-set OPTIMAL_N = {N}")

    # Main run
    print(f"\n{sep}")
    print(f"  Running {N}-qubit GHZ circuit ({SHOTS} shots) ...")
    print(sep)

    qc     = build_ghz(N)
    counts = sim.run(qc, shots=SHOTS).result().get_counts()
    total  = sum(counts.values())

    all_zeros = "0" * N
    all_ones  = "1" * N
    C_raw  = (counts.get(all_zeros, 0) + counts.get(all_ones, 0)) / total
    C_mit  = mitigate_correlation(counts, N, p0g0, p1g1)

    print(f"\n  |{'0'*min(N,8)}{'...' if N>8 else ''}> : {counts.get(all_zeros,0):,} shots")
    print(f"  |{'1'*min(N,8)}{'...' if N>8 else ''}> : {counts.get(all_ones,0):,} shots")
    print(f"  Other              : {total - counts.get(all_zeros,0) - counts.get(all_ones,0):,} shots")
    print(f"\n  C (raw)      = {C_raw:.6f}")
    print(f"  C (mitigated)= {C_mit:.6f}")

    score = compute_score(qc, C_mit, verbose=True)

    # Save QASM
    qasm_file = f"ghz_{N}_honest.qasm"
    with open(qasm_file, "w") as f:
        f.write(qasm2.dumps(qc))
    print(f"\n  QASM saved  {qasm_file}")

    print(f"\n{sep}\n")


if __name__ == "__main__":
    main()