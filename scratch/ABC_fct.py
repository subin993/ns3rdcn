import os
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt

# Function to extract Flow Completion Time from XML file
def extract_avg_delay_time(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()
    avg_delay_times = []
    
    for flow in root.findall('.//FlowStats/Flow'):
        delay_sum = float(flow.get('delaySum').replace('+', '').replace('ns', ''))
        rx_bytes = float(flow.get('rxBytes'))
        if rx_bytes > 0:  # Check to avoid division by zero
            avg_delay_time = delay_sum / rx_bytes / 1000  # Convert ns to μs
            avg_delay_times.append(avg_delay_time)
    return avg_delay_times

# Main function
def main():
    file_info = [
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS\n(Static)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_128ports_1G_mix_FB_ABC.xml", "P4-DADS\n(ABC)")
    ]

    all_avg_delay_times = []
    labels = []

    for file_path, label in file_info:
        if os.path.exists(file_path):
            avg_delay_times = extract_avg_delay_time(file_path)
            all_avg_delay_times.append(avg_delay_times)
            labels.append(label)
        else:
            print(f"File {file_path} does not exist.")

    plt.figure(figsize=(5, 6))  # Adjust the figure size to match the given code

    # Box Plot for each port configuration
    boxplot_data = plt.boxplot(all_avg_delay_times, labels=labels, vert=True, showfliers=False)  # Changed vert to True and hide fliers
    plt.yscale('log')  # Set y-axis to log scale
    plt.ylabel('Flow completion time (μs)', fontsize=20)  # Changed xlabel to ylabel and adjusted unit
    plt.xticks(fontsize=17)  # Set x-axis ticks and fontsize
    plt.yticks(fontsize=17)  # Set y-axis ticks fontsize
    plt.grid(True, which="both", ls="--")

    # Annotate median and max whisker values
    for i, avg_delay_times in enumerate(all_avg_delay_times, start=1):
        median = np.median(avg_delay_times)
        max_whisker_value = boxplot_data['whiskers'][2*i-1].get_ydata()[1]
        
        plt.text(i, median, f'{median:.2f}', horizontalalignment='center', fontsize=15, color='red', weight='bold')
        plt.text(i, max_whisker_value, f'{max_whisker_value:.2f}', horizontalalignment='center', fontsize=15, color='blue', weight='bold')

    plt.tight_layout()
    plt.savefig('avg_delay_analysis_ABC_new.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    main()
