import argparse
from ns3gym import ns3env
import numpy as np

# Parameters
startSim = False
iterationNum = 30
port = 5557
simTime = 60  # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime, "--stepTime": stepTime, "--testArg": 123}
debug = False

# Define the number of ports dynamically
num_entities = 128  # Can be 8, 16, 32, etc.

# Create the environment
env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim,
                    simSeed=seed, simArgs=simArgs, debug=debug)
env.reset()


currIt = 0
for i in range(iterationNum):
    print("Start iteration: ", currIt)
    obs = env.reset()
    stepIdx = 0
    queue_length_sums = []

    with open('queue_lengths_rotornet_mix.txt', 'w') as file:
        pass

    while True:
        queuelength = obs['QueueLength']
        QueueReceived = obs['QueueReceived']
        total_queue_length = sum(obs['QueueLength'])
        total_queue_received = sum(obs['QueueReceived'])
        queue_length_sums.append(total_queue_length)

        qs = [queuelength[i] for i in range(num_entities)]
        ds = [10] * num_entities  # Base data rate
        bs = [5000000] * num_entities  # All buffer sizes initialized to 5000

        current_idx = stepIdx % num_entities
        ds[current_idx] = 1000000  # Dynamically change data rate of the current port

        action = {
            'DataRate': ds,
            'BM': bs
        }
        obs, reward, done, info = env.step(action)
        
        if stepIdx % 20 == 0:
            with open('queue_lengths_rotornet_mix.txt', 'a') as file:
                file.write(f"{sum(queue_length_sums) / 20}\n")
            queue_length_sums = []

        if done:
            if currIt + 1 < iterationNum:
                obs = env.reset()
            else:
                break
            stepIdx = 0

        stepIdx += 1
        
    currIt += 1
    if currIt == iterationNum:
        env.close()
        print("Simulation Complete")

