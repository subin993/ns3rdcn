import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import numpy as np

def calculate_rx_bytes(file_path):
    # Load and parse the XML file
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Initialize the sum for this file
    total_rx_bytes = 0

    # Loop through each 'Flow' element and sum the 'rxBytes'
    for flow in root.findall('.//Flow'):
        rx_bytes = flow.get('rxBytes')
        if rx_bytes:
            total_rx_bytes += int(rx_bytes)

    return total_rx_bytes

def main():
    xml_files = [
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_16ports_1G_mix_FB_x5.xml", "GT(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_x0.5.xml", "P4ToR(Hadoop_x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_x0.5.xml", "Solstice(Hadoop_x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB_x0.5.xml", "RotorNet(Hadoop_x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB_x0.5.xml", "kEC(Hadoop_x0.5)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_16ports_1G_mix_FB_x5.xml", "GT(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_p4_DBT_32ports_1G_mix.xml", "P4ToR(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_dynamic_32ports_1G_mix.xml", "Solstice(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_fixed_32ports_1G_mix.xml", "RotorNet(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_kec_32ports_1G_mix.xml", "kEC(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_GT_32ports_1G_mix.xml", "GT(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_16ports_1G_mix_FB.xml", "P4ToR(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_16ports_1G_mix_FB.xml", "Solstice(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_16ports_1G_mix_FB.xml", "RotorNet(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_16ports_1G_mix_FB.xml", "kEC(Hadoop_x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_32ports_1G_mix.xml", "GT(Hadoop_x1)"),

        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_16ports_1G_mix_FB.xml", "GT(16ports)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_16ports_1G_mix_FB_x5.xml", "GT(16ports)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_p4_128ports_1G_mix_FB_x1.5.xml", "P4ToR(Hadoop_x1.5)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_dynamic_128ports_1G_mix_FB_x1.5.xml", "Solstice(Hadoop_x1.5)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_fixed_128ports_1G_mix_FB_x1.5.xml", "RotorNet(Hadoop_x1.5)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_kec_128ports_1G_mix_FB_x1.5.xml", "kEC(Hadoop_x1.5)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_16ports_1G_mix_FB_x5.xml", "GT(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_p4_128ports_1G_mix_FB_x2.xml", "P4ToR(Hadoop_x2)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_dynamic_128ports_1G_mix_FB_x2.xml", "Solstice(Hadoop_x2)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_fixed_128ports_1G_mix_FB_x2.xml", "RotorNet(Hadoop_x2)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP11/FlowMonitorData_kec_128ports_1G_mix_FB_x2.xml", "kEC(Hadoop_x2)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_16ports_1G_mix_FB_x5.xml", "GT(16ports)"),
        # # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_32ports_1G_mix_FB_x5.xml", "GT(32ports)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_DBT_128ports_1G_mix.xml", "P4ToR(DBT)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_SB50_ST30_128ports_1G_mix.xml", "P4ToR(SB50_ST30)"),
        # # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_DBT_128ports_1G_mix_FB_x5.xml", "P4ToR(DBT)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_dynamic_128ports_1G_mix.xml", "Solstice"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_fixed_128ports_1G_mix.xml", "RotorNet"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_kec_128ports_1G_mix.xml", "kEC"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_GT_128ports_1G_mix.xml", "GT"),
        # # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP10/FlowMonitorData_p4_128ports_1G_mix_datamining_x100.xml", "P4ToR(datamining)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP10/FlowMonitorData_dynamic_128ports_1G_mix_datamining_x100.xml", "Solstice(datamining)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP10/FlowMonitorData_fixed_128ports_1G_mix_datamining_x100.xml", "RotorNet(datamining)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP10/FlowMonitorData_kec_128ports_1G_mix_datamining_x100.xml", "kEC(datamining)"),
        # # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_32ports_1G_mix_FB_x5.xml", "GT(32ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_16ports_1G_mix_FB_INF.xml", "GT(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_16ports_1G_mix_FB.xml", "P4ToR(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_16ports_1G_mix_FB.xml", "Solstice(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_16ports_1G_mix_FB.xml", "RotorNet(16ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_16ports_1G_mix_FB.xml", "kEC(16ports)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_64ports_1G_mix_FB_x5.xml", "GT(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_32ports_1G_mix_FB_INF.xml", "GT(32ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_32ports_1G_mix_FB.xml", "P4ToR(32ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_32ports_1G_mix_FB.xml", "Solstice(32ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_32ports_1G_mix_FB.xml", "RotorNet(32ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_32ports_1G_mix_FB.xml", "kEC(32ports)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_64ports_1G_mix_FB_x5.xml", "GT(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_64ports_1G_mix_FB_INF.xml", "GT(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_64ports_1G_mix_FB.xml", "P4ToR(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_64ports_1G_mix_FB.xml", "Solstice(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_64ports_1G_mix_FB.xml", "RotorNet(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_64ports_1G_mix_FB.xml", "kEC(64ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_128ports_1G_mix_FB_x5.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_INF.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SBT.xml", "P4ToR(SBT)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_x0.5.xml", "P4ToR(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_x0.5.xml", "Solstice(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB_x0.5.xml", "RotorNet(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB_x0.5.xml", "kEC(128ports)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_128ports_1G_mix_FB_x5.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4ToR(WS)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB.xml", "Solstice(WS)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB.xml", "RotorNet(WS)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB.xml", "kEC(WS)"),
        # # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_128ports_1G_mix_FB_x5.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_x1.5.xml", "P4ToR(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_x1.5.xml", "Solstice(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB_x1.5.xml", "RotorNet(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB_x1.5.xml", "kEC(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_128ports_1G_mix_FB_x5.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_x2.xml", "P4ToR(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_x2.xml", "Solstice(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB_x2.xml", "RotorNet(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB_x2.xml", "kEC(DM)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_INF.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB30_ST30.xml", "P4ToR(SB30_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST30.xml", "P4ToR(SB50_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB70_ST30.xml", "P4ToR(SB70_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB100_ST30.xml", "P4ToR(SB100_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST10.xml", "P4ToR(SB50_ST10)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST30.xml", "P4ToR(SB50_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST50.xml", "P4ToR(SB50_ST50)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST70.xml", "P4ToR(SB50_ST70)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_SB50_ST90.xml", "P4ToR(SB50_ST90)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_GT_128ports_1G_mix_FB_x5.xml", "GT(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_p4_128ports_1G_mix_FB_x1.5.xml", "P4ToR(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB_x1.5.xml", "P4ToR(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB_x1.5.xml", "P4ToR(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB_x1.5.xml", "P4ToR(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_dynamic_128ports_1G_mix_FB.xml", "Solstice(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_fixed_128ports_1G_mix_FB.xml", "RotorNet(128ports)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/FlowMonitorData_kec_128ports_1G_mix_FB.xml", "kEC(128ports)")
    ]

    # Iterate over each file and calculate rxBytes
    for file_path, description in xml_files:
        total_rx_bytes = calculate_rx_bytes(file_path)
        print(f"Total rxBytes for {description}: {total_rx_bytes}")

if __name__ == "__main__":
    main()