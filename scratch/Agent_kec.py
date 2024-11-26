import argparse
from ns3gym import ns3env
import numpy as np

# Simulation parameters
startSim = False
iterationNum = 30
port = 5558
simTime = 60  # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime, "--stepTime": stepTime, "--testArg": 123}
debug = False

# Define the number of entities dynamically
num_entities = 128  # Adjust this to 8, 16, 32, or 64 depending on the test configuration

# Map for base cycles and active periods based on entity counts
cycle_period_map = {
    8: (57, 7),
    16: (58, 8),
    32: (61, 11),
    64: (67, 17),
    128: (79, 29)
}

# Get the cycle and period from the map based on the number of entities
base_cycle, active_period = cycle_period_map.get(num_entities, (67, 37))  # Default to largest if not found

# Create the environment
env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim,
                    simSeed=seed, simArgs=simArgs, debug=debug)
env.reset()


# Initialize simulation variables
currIt = 0
# bs = [5000] * num_entities  # All buffer sizes initialized to 5000
bs = [500000] * num_entities  # All buffer sizes initialized to 5000

for i in range(iterationNum):
    print("Start iteration: ", currIt)
    obs = env.reset()
    stepIdx = 0
    queue_length_sums = []
    prev_max_idx = None  # Track previous max index

    with open('queue_lengths_kec_mix.txt', 'w') as file:
        pass  # Open file in write mode to clear contents

    while True:
        stepIdx += 1
        queuelength = obs['QueueLength']
        total_queue_received = sum(obs['QueueReceived'])
        total_queue_length = sum(queuelength)
        queue_length_sums.append(total_queue_length)

        # Create data rate list initialized to minimum rate
        ds = [1] * num_entities
        
        # Dynamic scheduling logic based on the step index
        if stepIdx % base_cycle < active_period:
            ds = [1] * num_entities
        else:
            if prev_max_idx is not None:
                ds[prev_max_idx] = 100000  # Apply high data rate to previous max index

            max_idx = np.argmax(queuelength)  # Find new max index
            ds[max_idx] = 100000  # Apply high data rate to new max index
            prev_max_idx = max_idx  # Update previous max index

        # Create and send action to the environment
        action = {'DataRate': ds, 'BM': bs}
        obs, reward, done, info = env.step(action)

        

        if stepIdx % 10 == 0:  # Every 10 steps, write average queue length to file
            with open('queue_lengths_kec_mix.txt', 'a') as file:
                file.write(f"{sum(queue_length_sums) / 10}\n")
            queue_length_sums = []

        if done or stepIdx >= 200000:
            print("Iteration done")
            break

    currIt += 1
    if currIt == iterationNum:
        env.close()
        print("Simulation completed")
