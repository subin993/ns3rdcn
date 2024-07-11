import matplotlib.pyplot as plt



# New data for ports
ports_data = {
    16: {
        'GT': 2461205,
        'P4-DADS': 2461205,
        'Solstice': 2461205,
        'RotorNet': 2461205,
        'kEC': 2461205
    },
    32: {
        'GT': 19538740,
        'P4-DADS': 19538740,
        'Solstice': 17184927,
        'RotorNet': 19538740,
        'kEC': 19353489
    },
    64: {
        'GT': 57162188,
        'P4-DADS': 57162188,
        'Solstice': 52488321,
        'RotorNet': 56585813,
        'kEC': 57010817
    },
    128: {
        'GT': 169131328,
        'P4-DADS': 163202195,
        'Solstice': 115048598,
        'RotorNet': 96381634,
        'kEC': 148235792
    }
}

# Graph settings
techniques = ['Solstice', 'RotorNet', 'kEC', 'P4-DADS', 'GT']
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#8c564b', '#d62728']  # Updated color scheme
hatches = ['', '+', '/', '\\', '|']  # Updated hatch patterns
port_labels = list(ports_data.keys())

# Calculate relative throughput
relative_data = {port: {} for port in ports_data}
for port in ports_data:
    for tech in techniques:
        base_value = ports_data[port][tech]
        relative_data[port][tech] = base_value / max(ports_data[port].values())

# Create individual plots for each port
for port in port_labels:
    fig, ax = plt.subplots(figsize=(6, 7.7))  # Adjust size if needed
    bar_width = 0.2  # Bar width
    positions = [0]  # Single position for the single port

    for i, tech in enumerate(techniques):
        bars = ax.bar([p + i * bar_width for p in positions], 
                      [relative_data[port][tech]], 
                      bar_width, 
                      color=colors[i], 
                      hatch=hatches[i], 
                      label=tech, 
                      alpha=0.7 if tech != 'GT' else 1.0)  # Emphasize GT

        # Add text labels on the bars
        for bar in bars:
            yval = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2, yval + 0.02, f'{yval:.2f}', ha='center', va='bottom', fontsize=17)

    # Axis settings
    ax.set_xticks([])  # Remove x-ticks
    ax.tick_params(axis='y', labelsize=17)
    ax.set_ylim(0, 1.1)  # Set y-axis range
    ax.set_ylabel('Normalized aggregate throughput', fontsize=20)
    # ax.set_xlabel(f'Number of ports: {port}', fontsize=20)
    ax.legend(loc='lower right', fontsize=17)

    # Save each graph
    plt.tight_layout()
    plt.savefig(f'throughput_comparison_ports_{port}.png', dpi=300, bbox_inches='tight')
    plt.close(fig)
