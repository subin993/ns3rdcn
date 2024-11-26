import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the datasets
datamining_df = pd.read_csv('/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/datamining.csv')
hadoop_df = pd.read_csv('/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/FB_Hadoop_Inter_Rack_FlowCDF.csv')
websearch_df = pd.read_csv('/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/websearch.csv')

# Plot the CDFs for the datasets
plt.figure(figsize=(16, 9))

# Data Mining Dataset
plt.plot(datamining_df['Flow size'], datamining_df['CDF'], label='Data Mining', marker='o', markersize=8, linewidth=3.5)

# Hadoop Dataset
plt.plot(hadoop_df['Flow size'], hadoop_df['CDF'], label='Hadoop', marker='s', markersize=8, linewidth=3.5)

# Web Search Dataset
plt.plot(websearch_df['Flow size'], websearch_df['CDF'], label='Web Search', marker='^', markersize=8, linewidth=3.5)

# Set x-axis to log scale and define ticks
plt.xscale('log')
plt.xticks([10**2, 10**3, 10**4, 10**5, 10**6, 10**7, 10**8, 10**9], 
           [r'$10^2$', r'$10^3$', r'$10^4$', r'$10^5$', r'$10^6$', r'$10^7$', r'$10^8$', r'$10^9$'], fontsize=27)
plt.yticks(fontsize=28)
# Adding titles and labels
# plt.title('CDF of Flow Sizes')
plt.xlabel('Flow size (bytes)', fontsize=30)
plt.ylabel('CDF', fontsize=30)
plt.legend(fontsize=28)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)



# Show plot
plt.show()
plt.savefig('flow_cdf_3datasets_wide_new.png', dpi=300, bbox_inches='tight')
