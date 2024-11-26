import argparse
import pandas as pd
import numpy as np
from ns3gym import ns3env

# Load policy from CSV
policy_df = pd.read_csv('itr_policy_3.csv')

def get_policy(queue_size, token_possession, buffer_size, current_threshold):
    # Find the matching policy
    closest_policy = policy_df.iloc[((policy_df['Queue Size'] - queue_size).abs() + 
                                     (policy_df['Token Possession'] - token_possession).abs() +
                                     (policy_df['Buffer Size'] - buffer_size).abs() + 
                                     (policy_df['Threshold'] - current_threshold).abs()).argmin()]
    return closest_policy

# Simulation parameters
startSim = False
iterationNum = 1
port = 3566
# port = 6566
simTime = 60  # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime, "--stepTime": stepTime, "--testArg": 123}
debug = False

# Define the number of entities dynamically
num_entities = 128  # Can be set to 64, 32, 16, or 8

# Create the environment
env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim,
                    simSeed=seed, simArgs=simArgs, debug=debug)
env.reset()


# Main simulation loop
bs = [5000] * num_entities
for i in range(iterationNum):
    print("Start iteration: ", i)
    obs = env.reset()
    stepIdx = 0
    queue_length_sums = []
    current_threshold = 10
    prev_max_idx = None

    with open('queue_lengths_p4_mix.txt', 'w') as file:
        pass

    while True:
        stepIdx += 1
        queuelength = obs['QueueLength']
        total_queue_length = sum(queuelength)
        queue_length_sums.append(total_queue_length)
        ds = [1] * num_entities
        token_possession = [0] * num_entities
        threshold_met = False

        for idx, q_size in enumerate(queuelength):
            queue_percentage = (q_size / bs[idx]) * 100 if bs[idx] > 0 else 0

            if queue_percentage >= current_threshold:
                ds[idx] = 100000
                token_possession[idx] = 1
                threshold_met = True
            else:
                if prev_max_idx is not None:
                    ds[prev_max_idx] = 100000
                    token_possession[prev_max_idx] = 1
                max_idx = np.argmax(queuelength)
                prev_max_idx = max_idx

        # Policy application and action preparation
        for idx, q_size in enumerate(queuelength):
            buffer_size = bs[idx]
            token = token_possession[idx]
            policy = get_policy(q_size, token, buffer_size, current_threshold)
            potential_new_buffer_size = buffer_size + policy['Delta Buffer']
            current_threshold = max(10, current_threshold + policy['Delta Threshold'])
            bs[idx] = potential_new_buffer_size

        action = {'DataRate': ds, 'BM': bs}
        obs, reward, done, info = env.step(action)
        if stepIdx % 10 == 0:
            average_queue_length = sum(queue_length_sums) / len(queue_length_sums)
            with open('queue_lengths_p4_mix.txt', 'a') as file:
                file.write(f"{average_queue_length}\n")
            queue_length_sums = []

        stepIdx += 1
        if done:
            print("End of simulation iteration.")
            break

    if i + 1 == iterationNum:
        env.close()
        print("Simulation Done")
