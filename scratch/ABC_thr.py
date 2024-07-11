import matplotlib.pyplot as plt

# Data for total rxBytes for each technique
throughput_data = {
    'P4-DADS\n(ABC)': 163202195,
    'P4-DADS\n(Static)': 158186266,
    'GT': 169131328
}

# Normalize the throughput data
max_rx_bytes = max(throughput_data.values())
normalized_throughput = {tech: rx_bytes / max_rx_bytes for tech, rx_bytes in throughput_data.items()}

# Graph settings
techniques = ['P4-DADS\n(Static)', 'P4-DADS\n(ABC)', 'GT']
normalized_values = [normalized_throughput[tech] for tech in techniques]
colors = ['#8c564b', '#9467bd', '#d62728']  # Updated color scheme
hatches = ['\\', 'x', '|']  # Updated hatch patterns


# Create bar chart
fig, ax = plt.subplots(figsize=(5, 6))  # Adjust size if needed
bar_width = 0.75  # Bar width
positions = range(len(techniques))

bars = ax.bar(positions, 
              normalized_values, 
              bar_width, 
              color=colors, 
              hatch=hatches, 
              alpha=0.7 if techniques != 'GT' else 1.0)  # Bar settings

# Axis settings
ax.set_xticks(positions)
ax.set_xticklabels(techniques, fontsize=16, ha='center')
ax.tick_params(axis='y', labelsize=16)
ax.set_ylim(0, 1.1)  # Set y-axis range
ax.set_ylabel('Normalized aggregate throughput', fontsize=16)
# ax.set_title('Normalized Aggregate Throughput Comparison', fontsize=18)
ax.grid(True, axis='y', linestyle='--', alpha=0.7)

# Adding the text labels on the bars
for bar in bars:
    yval = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2, yval + 0.02, f'{yval:.2f}', ha='center', va='bottom', fontsize=15)

# Save the graph
plt.tight_layout()
plt.savefig('throughput_comparison_ABC.png', dpi=300, bbox_inches='tight')
plt.show()
