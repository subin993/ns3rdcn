import numpy as np
from scipy.stats import poisson
import csv

def save_policy_to_csv(policy, filename='heuristic_policy.csv'):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Queue Size', 'Token Possession', 'Buffer Size', 'Threshold', 'Delta Buffer', 'Delta Threshold'])
        for state, action in policy.items():
            writer.writerow([*state, *action])

# Constants
B_max = 6000
lambda_on = 1000
alpha = 0.01
beta = 0.6
gamma = 0.95
mu_OCS = 1000
mu_EPS = 10

# State and action spaces
states = [(q, t, b, p) for b in range(0, B_max+1, 100) for t in [0, 1] for q in range(0, b+1, 10) for p in range(10, 101, 10)]
lambda_eff = beta * lambda_on / (alpha + beta)

def heuristic_policy(state):
    Q_i, T_i, B_i, P_i = state
    best_action = None
    best_reward = float('-inf')
    candidate_actions = [
    (0, 0),    # No change
    (100, 0),   # Increase buffer slightly
    (200, 0),   # Increase buffer more
    (-100, 0),  # Decrease buffer slightly
    (-200, 0),  # Decrease buffer more
    (0, 10),   # Increase threshold slightly
    (0, 20),   # Increase threshold more
    (0, -10),  # Decrease threshold slightly
    (0, -20),  # Decrease threshold more
    (100, 10),  # Increase both buffer and threshold slightly
    (100, -10), # Increase buffer, decrease threshold
    (-100, 10), # Decrease buffer, increase threshold
    (-100, -10),# Decrease both buffer and threshold
    (200, 10),  # Increase both buffer and threshold more
    (200, -10), # Increase buffer more, decrease threshold
    (-200, 10), # Decrease buffer more, increase threshold
    (-200, -10) # Decrease both buffer and threshold more
]

    for action in candidate_actions:
        Delta_B_i, Delta_P_i = action
        B_next = B_i + Delta_B_i
        P_next = P_i + Delta_P_i

        if not (0 <= B_next <= B_max) or not (10 <= P_next <= 100):
            continue

        expected_arrivals = np.random.poisson(lambda_eff)
        # print(expected_arrivals)
        packet_drops = max(0, Q_i + expected_arrivals - B_next)
        drop_penalty = 100 * packet_drops
        throughput_reward = min(Q_i + expected_arrivals, mu_OCS if T_i == 1 else mu_EPS)
        buffer_utilization_reward = -100 if B_next == 0 else -abs((B_next - min(Q_i + expected_arrivals, B_next))) * 100
        
        total_reward = throughput_reward - drop_penalty + buffer_utilization_reward

        if total_reward > best_reward:
            best_reward = total_reward
            best_action = action

    return best_action

# Apply heuristic to all states and save the policy
heuristic_policy_result = {s: heuristic_policy(s) for s in states}
save_policy_to_csv(heuristic_policy_result)
