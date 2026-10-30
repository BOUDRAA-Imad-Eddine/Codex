from IPython.core.display_functions import display
from qiskit.quantum_info import Statevector, Operator
from numpy import sqrt

w = Statevector([0, 1, 1, 0, 1, 0, 0, 0] / sqrt(3))
display(w.draw("text"))

result, state = w.measure([0])
print(f"Measured: {result}\nState after measurement:")
display(state.draw("text"))

result, state = w.measure([0, 1])
print(f"Measured: {result}\nState after measurement:")
display(state.draw("text"))