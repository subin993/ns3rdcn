import numpy as np
from scipy.stats import poisson
import csv

def save_policy_to_csv(policy, filename='itr_policy_2.csv'):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Queue Size', 'Token Possession', 'Buffer Size', 'Threshold', 'Delta Buffer', 'Delta Threshold'])
        for state, action in policy.items():
            writer.writerow([*state, *action])

# Constants
B_max = 12000
lambda_on = 3000
alpha = 0.01
beta = 0.6
gamma = 0.95
mu_OCS = 3000
mu_EPS = 10

# State and action spaces updated for wider range
states = [(q, t, b, p) for b in range(0, B_max+1, 300) for t in [0, 1] for q in range(0, b+1, 100) for p in range(10, 61, 10)]
actions = [(db, dp) for db in range(-B_max, B_max+1, 300) for dp in range(-10, 11, 10)]  # Broader changes considered

# Effective arrival rate recalculated with new values
lambda_eff = beta * lambda_on / (alpha + beta)

# Effective arrival rate
lambda_eff = beta * lambda_on / (alpha + beta)

def transition_probability(current_state, action, next_state, dt=1):
    Q_i, T_i, B_i, P_i = current_state
    Delta_B_i, Delta_P_i = action
    Q_next, T_next, B_next, P_next = next_state

    if B_next != B_i + Delta_B_i or P_next != P_i + Delta_P_i:
        return 0
    
    new_arrivals = np.random.poisson(lambda_eff * dt)
    service_rate = mu_OCS if T_i == 1 else mu_EPS
    Q_potential = max(0, Q_i - service_rate * dt) + new_arrivals
    Q_next_expected = min(Q_potential, B_next)

    # Handling arrivals when the queue is full
    if Q_potential > B_next:
        return 0 if Q_next != B_next else 1
    else:
        return 1 if Q_next == Q_next_expected else 0

def reward_function(current_state, action):
    Q_i, T_i, B_i, P_i = current_state
    Delta_B_i, Delta_P_i = action
    B_next = B_i + Delta_B_i
    P_next = P_i + Delta_P_i

    if not (0 <= B_next <= B_max) or not (0 <= P_next <= 100):
        return float('-inf')  # Penalize invalid actions

    # Calculate packet drops if any
    expected_arrivals = np.random.poisson(lambda_eff)
    packet_drops = max(0, Q_i + expected_arrivals - B_next)
    drop_penalty = 1000 * packet_drops  # Increase drop penalty

    # Throughput reward: Assuming a higher reward if the buffer size allows processing more packets
    throughput_reward = min(Q_i + expected_arrivals, mu_OCS if T_i == 1 else mu_EPS)
    
    # Check if B_next is zero to avoid division by zero
    buffer_utilization_reward = -1000 if B_next == 0 else -abs((B_next - min(Q_i + expected_arrivals, B_next))) * 1000

    # # Modified costs for buffer and threshold adjustments
    # buffer_cost = 0.5 * (B_next - B_i) ** 2  # Reduced cost for increasing buffer size
    # threshold_cost = 0.1 * abs(P_next - 50)  # Reduced threshold adjustment cost

    return throughput_reward - drop_penalty + buffer_utilization_reward

def itr_heuristic(states, actions):
    policy = {}
    for state in states:
        best_action = None
        max_reward = float('-inf')  # Start with the lowest possible reward

        # Evaluate each action based on immediate reward
        for action in actions:
            reward = reward_function(state, action)
            if reward > max_reward:
                max_reward = reward
                best_action = action
        
        policy[state] = best_action
    return policy

def save_policy_to_csv(policy, filename='itr_policy_2.csv'):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Queue Size', 'Token Possession', 'Buffer Size', 'Threshold', 'Delta Buffer', 'Delta Threshold'])
        for state, action in policy.items():
            writer.writerow([*state, *action])

# Run the greedy heuristic
itr_policy = itr_heuristic(states, actions)
save_policy_to_csv(itr_policy)

# # Run value iteration
# optimal_policy = value_iteration(states, actions)
