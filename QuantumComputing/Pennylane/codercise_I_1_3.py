import numpy as np
def measure_state(state, num_meas):
    """Simulate a quantum measurement process.

    Args:
        state (np.array[complex]): A normalized qubit state vector.
        num_meas (int): The number of measurements to take

    Returns:
        np.array[int]: A set of num_meas samples, 0 or 1, chosen according to the probability
        distribution defined by the input state.
    """

    ##################
    # YOUR CODE HERE #
    ##################

    # COMPUTE THE MEASUREMENT OUTCOME PROBABILITIES

    # RETURN A LIST OF SAMPLE MEASUREMENT OUTCOMES

    # COMPUTE THE MEASUREMENT OUTCOME PROBABILITIES
    # Probabilities are the squared magnitudes of the amplitudes
    probs = np.abs(state) ** 2  # [ |alpha|^2, |beta|^2 ]

    # RETURN A LIST OF SAMPLE MEASUREMENT OUTCOMES
    # Draw `num_meas` samples from [0, 1] using the calculated probabilities
    measurements = np.random.choice([0, 1], size=num_meas, p=probs)
    
    return measurements