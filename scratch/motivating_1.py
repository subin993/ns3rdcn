import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# Data
B_Size = [3000, 5000, 7000, 10000]
throughput = [148014090, 158186266, 166788624, 168349414]
FCT_avg = [20960.87, 46912.41, 75255.71, 103480.83]
FCT_99perc = [175427.59, 491720.87, 707505.93, 829713.72]

# Normalize data
max_throughput = max(throughput)
max_FCT = max(FCT_avg + FCT_99perc)  # Max value from the combined list of FCT_avg and FCT_99perc

throughput_norm = [x / max_throughput for x in throughput]
FCT_avg_norm = [x / max_FCT for x in FCT_avg]
FCT_99perc_norm = [x / max_FCT for x in FCT_99perc]

# Plot
fig, ax1 = plt.subplots(figsize=(8, 9))

color = 'tab:blue'
ax1.set_xlabel('Buffer size (Packets)', fontsize=24)  # Increase label font size
ax1.set_ylabel('Normalized aggregate throughput', color=color, fontsize=24)
ax1.plot(B_Size, throughput_norm, color=color, marker='o', markersize=12, linewidth=3.5, label='Throughput')  # Larger markers and line width
ax1.tick_params(axis='both', labelsize=22)  # Increase tick label size for both axes

# Use ScalarFormatter and set scientific notation and tick label size for ax1
ax1.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
ax1.yaxis.get_offset_text().set_size(22)  # Set scientific notation size

ax2 = ax1.twinx()
color = 'tab:red'
ax2.set_ylabel('Normalized FCT', color=color, fontsize=24)
ax2.plot(B_Size, FCT_avg_norm, color='tab:orange', marker='o', markersize=12, linewidth=3.5, label='FCT Avg.')
ax2.plot(B_Size, FCT_99perc_norm, color=color, marker='o', markersize=12, linewidth=3.5, label='FCT 99perc.')
ax2.tick_params(axis='both', labelsize=22)

# Use ScalarFormatter and set scientific notation and tick label size for ax2
ax2.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
ax2.yaxis.get_offset_text().set_size(22)  # Set scientific notation size

fig.tight_layout()
# plt.title('Normalized Throughput and FCT by Buffer Size', fontsize=24)
ax1.legend(loc='lower right', fontsize=22)
ax2.legend(loc='center right', fontsize=22)

plt.savefig('bs_impact.png', dpi=300, bbox_inches='tight')
plt.show()
