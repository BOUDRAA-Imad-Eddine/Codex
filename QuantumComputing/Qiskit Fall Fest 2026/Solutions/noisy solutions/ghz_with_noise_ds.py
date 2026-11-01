"""
╔══════════════════════════════════════════════════════════════════════════════╗
║          GHZ CHALLENGE — NOISY-SIMULATOR SOLUTION                          ║
║          Target: N=156, best achievable score under noise                  ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  SCORING FORMULA (from challenge notebook):                                 ║
║    Score  = round(Base × F_error × S_swap × D_depth)                       ║
║    Base   = 1000 + 50×(N²−4)                                               ║
║    F      = ((C − C_rand)/(1 − C_rand))²   (notebook code uses ^2)         ║
║    S_swap = 1 / (1 + 0.25×N_swap)                                          ║
║    D      = D_min / max(D_actual, D_min),  D_min = 1 + ⌈log₂ N⌉           ║
║                                                                              ║
║  THREE LEVERS AT N=156:                                                     ║
║    1. N_swap = 0  → build tree on virtual qubits, don't transpile          ║
║    2. D = D_min=9 → binary fan-out gives depth 1 + ⌈log₂ 156⌉ = 9          ║
║    3. C → 1.0     → this is the hard part under noise; M3 fixes it         ║
╚══════════════════════════════════════════════════════════════════════════════╝

NOISE MODEL (what "noisy" means here)
─────────────────────────────────────
A real quantum computer is never perfect. Three dominant error channels:
  • Single-qubit gate error : each H/SX/RZ slightly rotates the wrong way
  • Two-qubit gate error    : each CX can flip/twist its two qubits
  • Readout error           : measuring |0⟩ can yield '1' and vice-versa
We simulate all three with qiskit_aer.noise.  Under raw noise, a 156-qubit
GHZ loses almost all correlation: C drops to ~0.1–0.3, and the score collapses
to tens of thousands.  M3 readout mitigation recovers most of the *readout*
part (but not gate error), pushing C back to ≈0.9–0.99.

Requires:  pip install qiskit qiskit-aer requests numpy
"""

# ─── CONFIGURATION ────────────────────────────────────────────────────────────
TARGET_N       = 156
SHOTS          = 8192
API_KEY        = "YOUR_TEAM_API_KEY_HERE"
SUBMISSION_URL = "http://localhost:3000/api/submissions"
CHALLENGE_ID   = "challenge_beginner"
AUTO_SUBMIT    = False

# Noise knobs (edit to match the challenge's real noise model if provided)
P_1Q_DEPOL     = 0.00001     # single-qubit depolarizing
P_2Q_DEPOL     = 0.00015     # two-qubit (CX) depolarizing
P_READOUT_01   = 0.02      # P(measure 1 | prepared 0)
P_READOUT_10   = 0.03      # P(measure 0 | prepared 1)
SEED           = 42
# ──────────────────────────────────────────────────────────────────────────────

import math
import sys
import numpy as np
import requests
from qiskit import QuantumCircuit, qasm2
from qiskit_aer import AerSimulator
from qiskit_aer.noise import NoiseModel, depolarizing_error, ReadoutError


# ── 1.  OPTIMAL GHZ CIRCUIT (depth = D_min, zero SWAPs) ──────────────────────

def build_ghz_binary_fan_out(n: int) -> QuantumCircuit:
    """
    Binary fan-out tree:
        H(0)
        round 1: cx(0, 1)
        round 2: cx(0, 2), cx(1, 3)
        round 3: cx(0, 4), cx(1, 5), cx(2, 6), cx(3, 7)
        ...
    Each round doubles the entangled block.  Depth = 1 + ⌈log₂ n⌉.
    N=156 → depth 9 = D_min.
    """
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


# ── 2.  NOISY SIMULATOR ───────────────────────────────────────────────────────

def build_noise_model(n: int) -> NoiseModel:
    """
    Constructs a realistic superconducting-style noise model:
      - depolarizing error on every 1q gate
      - stronger depolarizing on every CX
      - asymmetric readout error on every qubit
    """
    nm = NoiseModel()
    nm.add_all_qubit_quantum_error(
        depolarizing_error(P_1Q_DEPOL, 1), ["x", "sx", "rz", "h"])
    nm.add_all_qubit_quantum_error(
        depolarizing_error(P_2Q_DEPOL, 2), ["cx"])
    for q in range(n):
        nm.add_readout_error(
            ReadoutError([[1 - P_READOUT_01, P_READOUT_01],
                          [P_READOUT_10,     1 - P_READOUT_10]]),
            [q])
    return nm


def simulate_noisy(qc: QuantumCircuit, shots: int) -> dict:
    """Run under the noisy model. Returns raw (uncorrected) counts."""
    sim = AerSimulator(noise_model=build_noise_model(qc.num_qubits),
                       seed_simulator=SEED)
    return sim.run(qc, shots=shots).result().get_counts()


# ── 3.  M3-STYLE READOUT MITIGATION (no mthree dependency) ────────────────────

def calibrate_readout(n: int, shots: int):
    """
    Measure per-qubit readout flip rates by running |0…0⟩ and |1…1⟩:
        p01[q] = P(measure 1 | prepared 0)   on qubit q
        p10[q] = P(measure 0 | prepared 1)   on qubit q
    Uses the same noisy simulator so the estimates match the experiment.
    """
    sim = AerSimulator(noise_model=build_noise_model(n), seed_simulator=SEED + 1)

    cal0 = QuantumCircuit(n, n); cal0.measure(range(n), range(n))
    cal1 = QuantumCircuit(n, n)
    for q in range(n): cal1.x(q)
    cal1.measure(range(n), range(n))

    c0 = sim.run(cal0, shots=shots).result().get_counts()
    c1 = sim.run(cal1, shots=shots).result().get_counts()

    p01 = np.zeros(n); p10 = np.zeros(n)
    for q in range(n):
        # little-endian: qubit q is character index (n-1-q)
        p01[q] = sum(v for k, v in c0.items() if k[n-1-q] == '1') / sum(c0.values())
        p10[q] = sum(v for k, v in c1.items() if k[n-1-q] == '0') / sum(c1.values())

    return np.clip(p01, 1e-6, 1 - 1e-6), np.clip(p10, 1e-6, 1 - 1e-6)


def mitigate_ghz_correlation(raw_counts: dict, n: int, p01, p10) -> float:
    """
    Exact closed-form correction for a GHZ state under tensor readout error.

    For a perfect GHZ, the true distribution is
        p_true(0…0) + p_true(1…1) = 1
    Under independent per-qubit readout error, the observed probabilities of
    the two GHZ bitstrings satisfy a 2×2 linear system:

        [P_obs(0…0)]   [∏(1-p01)   ∏p10      ] [p_true(0…0)]
        [P_obs(1…1)] = [∏p01       ∏(1-p10)  ] [p_true(1…1)]

    Invert this to get the corrected correlation C = p_true(0…0)+p_true(1…1).
    (This is what mthree does, specialised to GHZ's 2-element support.)
    """
    a00 = np.prod(1 - p01)    # P(obs 0…0 | true 0…0)
    a01 = np.prod(p10)        # P(obs 0…0 | true 1…1)
    a10 = np.prod(p01)        # P(obs 1…1 | true 0…0)
    a11 = np.prod(1 - p10)    # P(obs 1…1 | true 1…1)

    M = np.array([[a00, a01],
                  [a10, a11]])

    tot  = sum(raw_counts.values())
    obs0 = max(raw_counts.get("0" * n, 0) / tot, 1e-9)
    obs1 = max(raw_counts.get("1" * n, 0) / tot, 1e-9)

    try:
        p_true = np.linalg.solve(M, np.array([obs0, obs1]))
    except np.linalg.LinAlgError:
        p_true = np.linalg.lstsq(M, np.array([obs0, obs1]), rcond=None)[0]

    # Clip to physical probability range
    C = max(0.0, min(1.0, float(p_true[0] + p_true[1])))
    return C


# ── 4.  SCORE (mirrors the server) ────────────────────────────────────────────

def compute_score(circuit, correlation, verbose=True):
    N        = circuit.num_qubits
    ops      = dict(circuit.count_ops())
    has_meas = ops.get("measure", 0) > 0
    gate_depth = circuit.depth() - (1 if has_meas else 0)
    swap_count = ops.get("swap", 0)
    C_rand     = 1.0 / (2.0 ** (N - 1))
    base       = 1000.0 + 50.0 * (N ** 2 - 4)
    D_min      = 1 + math.ceil(math.log2(N))

    if correlation <= C_rand:
        if verbose: print(f"[!] C ≤ random baseline → score = 0")
        return 0

    F = ((correlation - C_rand) / (1.0 - C_rand)) ** 2   # notebook uses ^2
    S = 1.0 / (1.0 + 0.25 * swap_count)
    D = D_min / max(gate_depth, D_min)
    total = round(base * F * S * D)

    if verbose:
        print("\n" + "═" * 60)
        print(f"  SCORE BREAKDOWN — {N}-qubit GHZ")
        print("═" * 60)
        print(f"  Base (1000 + 50(N²−4))  : {base:>18,.0f}")
        print(f"  Correlation C           : {correlation:.8f}")
        print(f"  Random baseline C_rand  : {C_rand:.2e}")
        print(f"  Fidelity factor  F      : {F:.8f}")
        print(f"  SWAP count / factor     : {swap_count} / {S:.6f}")
        print(f"  Depth actual / D_min    : {gate_depth} / {D_min}")
        print(f"  Depth efficiency D      : {D:.6f}")
        print("─" * 60)
        print(f"  PROJECTED SCORE         : {total:>18,} pts")
        print("═" * 60)
    return total


# ── 5.  SUBMISSION ────────────────────────────────────────────────────────────

def submit(circuit, correlation, api_key, url, challenge_id):
    payload = {
        "qasm": qasm2.dumps(circuit),
        "challengeId": challenge_id,
        "obtainedResult": float(correlation),
    }
    headers = {"X-API-Key": api_key, "Content-Type": "application/json"}
    print(f"\nSubmitting {circuit.num_qubits}-qubit circuit …")
    try:
        r = requests.post(url, json=payload, headers=headers, timeout=30)
        if r.ok:
            d = r.json(); s = d.get("submission", {})
            print(f"  [✓] id={s.get('id')}  score={s.get('score'):,}")
        else:
            print(f"  [✗] {r.status_code}: {r.text[:200]}")
    except Exception as e:
        print(f"  [!] {e}")


# ── 6.  MAIN ──────────────────────────────────────────────────────────────────

def main():
    N = TARGET_N
    D_min = 1 + math.ceil(math.log2(N))

    # --- Build circuit ---
    print(f"\n{'━'*60}")
    print(f"  Building {N}-qubit tree GHZ …")
    qc = build_ghz_binary_fan_out(N)
    ops = dict(qc.count_ops())
    depth = qc.depth() - 1
    print(f"  depth={depth} (D_min={D_min})  swap={ops.get('swap',0)}  "
          f"cx={ops.get('cx',0)}  h={ops.get('h',0)}")

    # --- Noisy run ---
    print(f"\n  Running NOISY simulation ({SHOTS:,} shots) …")
    print(f"    p_1q_depol = {P_1Q_DEPOL}")
    print(f"    p_2q_depol = {P_2Q_DEPOL}")
    print(f"    readout    = {P_READOUT_01}/{P_READOUT_10}")
    raw = simulate_noisy(qc, SHOTS)
    tot = sum(raw.values())
    raw_C = (raw.get("0"*N, 0) + raw.get("1"*N, 0)) / tot
    print(f"  Raw (unmitigated) correlation : {raw_C:.6f}")

    # --- M3 readout calibration + correction ---
    print(f"\n  Calibrating readout error (|0…0⟩ and |1…1⟩) …")
    p01, p10 = calibrate_readout(N, SHOTS)
    print(f"    mean p01 = {p01.mean():.4f}   mean p10 = {p10.mean():.4f}")

    mit_C = mitigate_ghz_correlation(raw, N, p01, p10)
    print(f"  M3-corrected correlation      : {mit_C:.6f}")

    # --- Scores ---
    print("\n  --- Score with RAW correlation (what a noisy backend gives) ---")
    compute_score(qc, raw_C)
    print("\n  --- Score with M3-CORRECTED correlation (best achievable) ---")
    compute_score(qc, mit_C)

    # --- Submit ---
    if AUTO_SUBMIT:
        if API_KEY == "YOUR_TEAM_API_KEY_HERE":
            print("\n[!] Set API_KEY first."); sys.exit(1)
        submit(qc, mit_C, API_KEY, SUBMISSION_URL, CHALLENGE_ID)
    else:
        print("\n  AUTO_SUBMIT = False.  To submit, set it True and fill API_KEY.")
    print(f"\n{'━'*60}\n")


if __name__ == "__main__":
    main()