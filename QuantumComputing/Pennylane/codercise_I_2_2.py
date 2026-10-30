import pennylane as qp

# This creates a device with three wires on which PennyLane can run computations
dev = qp.device("default.qubit", wires=3)


def my_circuit(theta, phi, omega):

    ##################
    # YOUR CODE HERE #
    ##################

    # IMPLEMENT THE CIRCUIT BY ADDING THE GATES

    # Here are two examples, so you can see the format:
    # qp.CNOT(wires=[0, 1])
    # qp.RX(theta, wires=0)
    qp.RX(theta,wires = 0)
    qp.RY(phi,wires = 1)
    qp.RZ(omega, wires = 2)
    qp.CNOT(wires=[0.1])
    qp.CNOT(wires=[1,2])
    qp.CNOT(wires=[3.1])



    return qp.probs(wires=[0, 1, 2])


# This creates a QNode, binding the function and device
my_qnode = qp.QNode(my_circuit, dev)

# We set up some values for the input parameters
theta, phi, omega = 0.1, 0.2, 0.3

# Now we can execute the QNode by calling it like we would a regular function
my_qnode(theta, phi, omega)