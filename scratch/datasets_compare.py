import matplotlib.pyplot as plt
import numpy as np

# Data for total rxBytes for each technique and dataset

throughput_data = {
    'Websearch': {
        'P4-DADS': 211746871,
        'Solstice': 129667221,
        'RotorNet': 109495436,
        'kEC': 158010151
    },
    'Datamining': {
        'P4-DADS': 150715554,
        'Solstice': 150715554,
        'RotorNet': 150715554,
        'kEC': 150715554
    },
    'FB_Hadoop': {
        'P4-DADS': 161456033,
        'Solstice': 115048598,
        'RotorNet': 93207231,
        'kEC': 148235792
    }
}

# Normalize the throughput data
normalized_throughput = {}
for dataset, techniques in throughput_data.items():
    max_rx_bytes = max(techniques.values())
    normalized_throughput[dataset] = {tech: rx_bytes / max_rx_bytes for tech, rx_bytes in techniques.items()}

# Graph settings
datasets = list(normalized_throughput.keys())
techniques = ['Solstice', 'RotorNet', 'kEC', 'P4-DADS']
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#8c564b']  # Updated color scheme
hatches = ['', '+', '/', '\\']  # Updated hatch patterns

# Create bar chart
fig, ax = plt.subplots(figsize=(16, 9))  # Adjust size if needed
bar_width = 0.2  # Bar width
positions = np.arange(len(datasets))

# Plotting
for i, tech in enumerate(techniques):
    values = [normalized_throughput[dataset][tech] for dataset in datasets]
    bar_positions = [p + i * bar_width for p in positions]
    bars = ax.bar(bar_positions, values, bar_width, color=colors[i], hatch=hatches[i], label=tech, alpha=0.7)

    # Add text labels on the bars
    for bar in bars:
        yval = bar.get_height()
        ax.text(bar.get_x() + bar.get_width() / 2, yval + 0.02, f'{yval:.2f}', ha='center', va='bottom', fontsize=27)

# Axis settings
ax.set_xticks(positions + bar_width * 1.5)  # Center the ticks between the groups
ax.set_xticklabels(datasets, fontsize=28, rotation=0, ha='center')
ax.tick_params(axis='y', labelsize=28)
ax.set_ylim(0, 1.1)  # Set y-axis range
ax.set_ylabel('Normalized aggregate throughput', fontsize=30)
ax.set_xlabel('Traffic data', fontsize=30)
ax.legend(loc='lower left', fontsize=28)
ax.grid(True, axis='y', linestyle='--', alpha=0.7)

# Save the graph
plt.tight_layout()
plt.savefig('dataset_compare_new.png', dpi=300, bbox_inches='tight')
plt.show()
