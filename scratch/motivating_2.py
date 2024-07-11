import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# Data
P_values = [10, 30, 50, 70, 90]
throughput_P = [157663611, 158186266, 157984229, 155897961, 151587343]
FCT_avg_P = [10544.09, 46912.41, 83032.17, 129679.94, 144585.93]
FCT_99perc_P = [78686.67, 491720.87, 704875.87, 873917.65, 1087234.83]

# Normalize data
max_throughput_P = max(throughput_P)
max_FCT_P = max(FCT_avg_P + FCT_99perc_P)  # Max value from the combined list of FCT_avg_P and FCT_99perc_P

throughput_P_norm = [x / max_throughput_P for x in throughput_P]
FCT_avg_P_norm = [x / max_FCT_P for x in FCT_avg_P]
FCT_99perc_P_norm = [x / max_FCT_P for x in FCT_99perc_P]

# Plot
fig, ax1 = plt.subplots(figsize=(8, 9))

color = 'tab:blue'
ax1.set_xlabel('Threshold (%)', fontsize=24)
ax1.set_ylabel('Normalized aggregate throughput', color=color, fontsize=24)
ax1.plot(P_values, throughput_P_norm, color=color, marker='o', markersize=12, linewidth=3.5, label='Throughput')
ax1.tick_params(axis='both', labelsize=22)

ax1.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
ax1.yaxis.get_offset_text().set_size(22)  # Set scientific notation size

ax2 = ax1.twinx()
color = 'tab:red'
ax2.set_ylabel('Normalized FCT', color=color, fontsize=24)
ax2.plot(P_values, FCT_avg_P_norm, color='tab:orange', marker='o', markersize=12, linewidth=3.5, label='FCT Avg.')
ax2.plot(P_values, FCT_99perc_P_norm, color=color, marker='o', markersize=12, linewidth=3.5, label='FCT 99perc.')
ax2.tick_params(axis='both', labelsize=22)

ax2.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
ax2.yaxis.get_offset_text().set_size(22)  # Set scientific notation size

fig.tight_layout()
# plt.title('Normalized Throughput and FCT by Threshold (%)', fontsize=24)
ax1.legend(loc='lower right', fontsize=22)
ax2.legend(loc='center right', fontsize=22)

plt.savefig('thr_impact.png', dpi=300, bbox_inches='tight')
# plt.show()
