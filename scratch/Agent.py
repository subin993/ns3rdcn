import argparse
from ns3gym import ns3env
# from action_func import*
from collections import OrderedDict
import numpy as np

startSim = False
iterationNum = 1
port = 5566
simTime = 60 # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = False

# Define the number of entities dynamically
num_entities = 64  # Can be set to 64, 32, 16, or 8
env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim,
                    simSeed=seed, simArgs=simArgs, debug=debug)
env.reset()

print("Observation space: ", env.observation_space, env.observation_space.dtype)
print("Action space: ", env.action_space, env.action_space.dtype)

currIt = 0
# Main simulation loop
bs = [5000] * num_entities  # Initial buffer sizes

for i in range(1):
    print("Start iteration: ", currIt)
    obs = env.reset()
    stepIdx = 0
    queue_length_sums = []
    current_threshold = 90
    
    with open('queue_lengths_p4_mix.txt', 'w') as file:
        pass

    # while True:
    
    for j in range(200000):
            
        stepIdx += 1

        queuelength = obs['QueueLength']
        total_queue_received = sum(obs['QueueReceived'])
        total_queue_length = sum(queuelength)
        queue_length_sums.append(total_queue_length)
        
        qs = [queuelength[i] for i in range(num_entities)]
        
        ds = [1] * num_entities  # This creates a list of 32 elements, all set to 1000

        
        for idx, q_size in enumerate(qs):
            # Calculate queue size as a percentage of buffer size
            queue_percentage = (q_size / bs[idx]) * 100 if bs[idx] > 0 else 0
            # Current policy threshold percentage

            if queue_percentage >= current_threshold:
                ds[idx] = 100000
                # token_possession[idx] = 1
                break  # Adjust and stop at the first queue meeting the threshold

        # for idx, value in enumerate(qs):
        #     if value > 100:
        #         ds[idx] = 1000000000
        #         break


        action = {'DataRate': ds, 'BM': bs}
        obs, reward, done, info = env.step(action)
        # print("obs : ",obs)
        # print("done : ",done)
        print("---obs, reward, done, info: ", obs, reward, done, info)
        print ("************  ", total_queue_received)
        
        if stepIdx % 10 == 0: # Every 100 steps
            with open('queue_lengths_p4_mix.txt', 'a') as file: # Use 'a' (append mode) to add data to the file without overwriting
                file.write(f"{sum(queue_length_sums) / 10}\n")  # Save the sum of the last 100 values
            queue_length_sums = [] # Reset the list

        if done or stepIdx >= 200000:
            print("Iteration done")
            break
        
        

    currIt += 1
    if currIt == iterationNum:
        env.close()
        print("Simulation completed")
        