from IPython.core.display_functions import display
from qiskit.quantum_info import Statevector, Operator
from numpy import sqrt

zero = Statevector.from_label("0")
one = Statevector.from_label("1")
psi = zero.tensor(one)
#display(psi.draw("text"))


plus = Statevector.from_label("+")
minus_i = Statevector.from_label("l")
phi = plus.tensor(minus_i)
#display(phi.draw("text"))

H = Operator.from_label("H")
Id = Operator.from_label("I")
X = Operator.from_label("X")
display(H.tensor(Id).draw("text"))
print("\n")
#display(H.tensor(Id).tensor(X).draw("text"))
display((H ^ Id).draw("text"))