import matplotlib.pyplot as plt
import numpy as np

# Network load data
load_data = {
    0.5: {
        'P4-DADS': 77053139,
        'Solstice': 62434846,
        'RotorNet': 54602146,
        'kEC': 73600475
    },
    1: {
        'P4-DADS': 161456033,
        'Solstice': 115048598,
        'RotorNet': 93207231,
        'kEC': 148235792
    },
    1.5: {
        'P4-DADS': 233962661,
        'Solstice': 141971768,
        'RotorNet': 105828355,
        'kEC': 172144187
    },
    # 2: {
    #     'P4-DADS': 260018266,
    #     'Solstice': 152958150,
    #     'RotorNet': 127711205,
    #     'kEC': 186146963
    # }
}

# Graph settings
techniques = ['Solstice', 'RotorNet', 'kEC', 'P4-DADS']
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#8c564b']  # Updated color scheme
hatches = ['', '+', '/', '\\']  # Updated hatch patterns
load_labels = list(load_data.keys())

# Calculate relative throughput
relative_data = {tech: [] for tech in techniques}
for load in load_data:
    max_throughput = max(load_data[load].values())
    for tech in techniques:
        relative_data[tech].append(load_data[load][tech] / max_throughput)

# Create plot for each technique over different loads
fig, ax = plt.subplots(figsize=(16, 9))  # Adjust size if needed

bar_width = 0.2  # Bar width
index = np.arange(len(load_labels))  # Load positions

for i, tech in enumerate(techniques):
    bars = ax.bar(index + i * bar_width, 
                  relative_data[tech], 
                  bar_width, 
                  color=colors[i], 
                  hatch=hatches[i], 
                  label=tech, 
                  alpha=0.7)  # Bar transparency

    # Add text labels on the bars
    for j, bar in enumerate(bars):
        yval = bar.get_height()
        ax.text(bar.get_x() + bar.get_width() / 2, yval + 0.02, f'{yval:.2f}', ha='center', va='bottom', fontsize=22)

# Axis settings
ax.set_xticks(index + bar_width * (len(techniques) - 1) / 2)
ax.set_xticklabels([f'x{load}' for load in load_labels])
ax.tick_params(axis='both', labelsize=22)
ax.set_ylim(0, 1.1)  # Set y-axis range
ax.set_ylabel('Normalized aggregate throughput', fontsize=24)
ax.set_xlabel('Network load (FB_Hadoop)', fontsize=24)
ax.legend(loc='lower left', fontsize=22)

# Save the graph
plt.tight_layout()
plt.savefig('throughput_comparison_load.png', dpi=300, bbox_inches='tight')
plt.show()
plt.close(fig)
