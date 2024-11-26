import matplotlib.pyplot as plt
import numpy as np

# Data setup
ratios = ['10:1 (10G:1G)', '5:1 (10G:2G)', '2:1 (10G:5G)']
p4_dads_throughput = np.array([279279838, 330713542, 362835814])
kec_throughput = np.array([226202015, 262151535, 284194874])

# Normalizing throughput values to the highest throughput in the dataset for comparison
highest_throughput = max(p4_dads_throughput.max(), kec_throughput.max())
p4_dads_normalized = p4_dads_throughput / highest_throughput
kec_normalized = kec_throughput / highest_throughput

# Throughput improvement calculation
improvement = (p4_dads_throughput - kec_throughput) / kec_throughput * 100

# Plotting
fig, ax = plt.subplots()

# Customizing bar colors and width
bar_width = 0.35
# Darker and more aesthetic colors
custom_colors = ['#2874A6', '#CB4335']  # Navy blue for P4-DADS, dark red for kEC
index = np.arange(len(ratios))

# Plotting bars
bar1 = ax.bar(index, p4_dads_normalized, bar_width, color=custom_colors[0], label='P4-DADS')
bar2 = ax.bar(index + bar_width, kec_normalized, bar_width, color=custom_colors[1], label='kEC')

# Adding grid lines for better readability
ax.set_axisbelow(True)
ax.yaxis.grid(color='gray', linestyle='dashed')

# Add text for labels, title and axes ticks
ax.set_xlabel('OCS:EPS bandwidth ratio')
ax.set_ylabel('Normalized aggregate throughput')
ax.set_title('Throughput comparison at different bandwidth ratios')
ax.set_xticks(index + bar_width / 2)
ax.set_xticklabels(ratios)
ax.legend()

# Adjusting text for improvement percentages above the bars for better visibility
offsets = [0.01, 0.01, 0.01]  # Adjusting the offset for each bar based on appearance
for idx in range(len(ratios)):
    ax.text(idx, p4_dads_normalized[idx] + offsets[idx], f'{improvement[idx]:.2f}%', ha='center', color='black')

plt.show()
plt.savefig('throughput_comparison_ports_extra.png', dpi=300, bbox_inches='tight')
