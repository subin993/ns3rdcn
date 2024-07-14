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
            avg_delay_time = (delay_sum / rx_bytes) / 1000  # Convert ns to μs
            avg_delay_times.append(avg_delay_time)
    return avg_delay_times

# Main function
def main():
    port_groups = {
        "16ports": [
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_solstice_16ports_1G_mix_FB_INF.xml", "Solstice"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_rotornet_16ports_1G_mix_FB_INF.xml", "RotorNet"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_kec_16ports_1G_mix_FB_INF.xml", "kEC"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_p4_16ports_1G_mix_FB.xml", "P4-DADS")
        ],
        "32ports": [
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_solstice_32ports_1G_mix_FB_INF.xml", "Solstice"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_rotornet_32ports_1G_mix_FB_INF.xml", "RotorNet"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_kec_32ports_1G_mix_FB_INF.xml", "kEC"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_p4_32ports_1G_mix_FB.xml", "P4-DADS")
        ],
        "64ports": [
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_solstice_64ports_1G_mix_FB_INF.xml", "Solstice"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_rotornet_64ports_1G_mix_FB_INF.xml", "RotorNet"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_kec_64ports_1G_mix_FB_INF.xml", "kEC"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_p4_64ports_1G_mix_FB.xml", "P4-DADS")
        ],
        "128ports": [
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_solstice_128ports_1G_mix_FB_INF.xml", "Solstice"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_rotornet_128ports_1G_mix_FB_INF.xml", "RotorNet"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_kec_128ports_1G_mix_FB_INF.xml", "kEC"),
            ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/fct/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS")
        ],
    }

    for port_config, file_info in port_groups.items():
        all_avg_delay_times = []
        labels = []
        
        for file_path, label in file_info:
            if os.path.exists(file_path):
                avg_delay_times = extract_avg_delay_time(file_path)
                
                all_avg_delay_times.append(avg_delay_times)
                labels.append(label)
                
            else:
                print(f"File {file_path} does not exist.")
        
        plt.figure(figsize=(6, 7.7))  # Adjust the figure size to match the given code

        # Box Plot for each port configuration
        boxplot_data = plt.boxplot(all_avg_delay_times, labels=labels, vert=True, showfliers=False)  # Changed vert to True and hide fliers
        plt.ylabel('Flow completion time (μs)', fontsize=20)  # Changed xlabel to ylabel and adjusted unit
        plt.yticks(fontsize=17)  # Set y-axis ticks and fontsize
        plt.xticks(fontsize=17)  # Set x-axis ticks fontsize
        plt.grid(True)
        
        # Annotate median and max whisker values
        for i, avg_delay_times in enumerate(all_avg_delay_times, start=1):
            median = np.median(avg_delay_times)
            max_whisker_value = boxplot_data['whiskers'][2*i-1].get_ydata()[1]
            
            plt.text(i, median, f'{median:.2f}', horizontalalignment='center', fontsize=12, color='red', weight='bold')
            plt.text(i, max_whisker_value, f'{max_whisker_value:.2f}', horizontalalignment='center', fontsize=12, color='blue', weight='bold')

        plt.tight_layout()
        plt.savefig(f'avg_fct_analysis_{port_config}_1G.png', dpi=300, bbox_inches='tight')
        plt.close()

if __name__ == "__main__":
    main()
