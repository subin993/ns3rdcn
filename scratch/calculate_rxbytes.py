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
        # EXP0
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB_ST10_128ports_1G_mix.xml", "P4ToR(SB_ST10)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB_ST30_128ports_1G_mix.xml", "P4ToR(SB_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB_ST50_128ports_1G_mix.xml", "P4ToR(SB_ST50)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB_ST70_128ports_1G_mix.xml", "P4ToR(SB_ST70)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB_ST90_128ports_1G_mix.xml", "P4ToR(SB_ST90)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB30_ST30_128ports_1G_mix.xml", "P4ToR(SB30_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB50_ST30_128ports_1G_mix.xml", "P4ToR(SB50_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB70_ST30_128ports_1G_mix.xml", "P4ToR(SB70_ST30)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP0_motivating/FlowMonitorData_p4_SB100_ST30_128ports_1G_mix.xml", "P4ToR(SB100_ST30)"),
        # # EXP1
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_GT_128ports_1G_mix.xml", "GT"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_128ports_1G_mix_FB_ABC.xml", "P4-DADS(ABC)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP1/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS(Static)"),
        # # EXP2(Througput)
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_GT_16ports_1G_mix.xml", "GT(16)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_GT_32ports_1G_mix.xml", "GT(32)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_GT_64ports_1G_mix.xml", "GT(64)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_GT_128ports_1G_mix.xml", "GT(128)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_GT_256ports_1G_mix.xml", "GT(256)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_solstice_16ports_1G_mix_FB.xml", "Solstice(16)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_solstice_32ports_1G_mix_FB.xml", "Solstice(32)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_solstice_64ports_1G_mix_FB.xml", "Solstice(64)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_solstice_128ports_1G_mix_FB.xml", "Solstice(128)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_solstice_256ports_1G_mix_FB.xml", "Solstice(256)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_rotornet_16ports_1G_mix_FB.xml", "Rotornet(16)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_rotornet_32ports_1G_mix_FB.xml", "Rotornet(32)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_rotornet_64ports_1G_mix_FB.xml", "Rotornet(64)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_rotornet_128ports_1G_mix_FB.xml", "Rotornet(128)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_rotornet_256ports_1G_mix_FB.xml", "Rotornet(256)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_kec_16ports_1G_mix_FB.xml", "kEC(16)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_kec_32ports_1G_mix_FB.xml", "kEC(32)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_kec_64ports_1G_mix_FB.xml", "kEC(64)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_kec_128ports_1G_mix_FB.xml", "kEC(128)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_kec_256ports_1G_mix_FB.xml", "kEC(256)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_p4_16ports_1G_mix_FB.xml", "P4-DADS(16)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_p4_32ports_1G_mix_FB.xml", "P4-DADS(32)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_p4_64ports_1G_mix_FB.xml", "P4-DADS(64)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS(128)"),
        ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP2/throughput/FlowMonitorData_p4_256ports_1G_mix_FB.xml", "P4-DADS(256)"),
        # # EXP3
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_solstice_128ports_1G_mix_Datamining.xml", "Solstice(Datamining)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_solstice_128ports_1G_mix_FB.xml", "Solstice(FB_Hadoop)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_solstice_128ports_1G_mix_Websearch.xml", "Solstice(Websearch)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_rotornet_128ports_1G_mix_Datamining.xml", "Rotornet(Datamining)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_rotornet_128ports_1G_mix_FB.xml", "Rotornet(FB_Hadoop)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_rotornet_128ports_1G_mix_Websearch.xml", "Rotornet(Websearch)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_kec_128ports_1G_mix_Datamining.xml", "kEC(Datamining)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_kec_128ports_1G_mix_FB.xml", "kEC(FB_Hadoop)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_kec_128ports_1G_mix_Websearch.xml", "kEC(Websearch)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_p4_128ports_1G_mix_Datamining.xml", "P4-DADS(Datamining)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS(FB_Hadoop)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP3/FlowMonitorData_p4_128ports_1G_mix_Websearch.xml", "P4-DADS(Websearch)"),
        # # EXP4
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_solstice_128ports_1G_mix_FB_x0.5.xml", "Solstice(x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_solstice_128ports_1G_mix_FB.xml", "Solstice(x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_solstice_128ports_1G_mix_FB_x1.5.xml", "Solstice(x1.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_rotornet_128ports_1G_mix_FB_x0.5.xml", "Rotornet(x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_rotornet_128ports_1G_mix_FB.xml", "Rotornet(x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_rotornet_128ports_1G_mix_FB_x1.5.xml", "Rotornet(x1.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_kec_128ports_1G_mix_FB_x0.5.xml", "kEC(x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_kec_128ports_1G_mix_FB.xml", "kEC(x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_kec_128ports_1G_mix_FB_x1.5.xml", "kEC(x1.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_p4_128ports_1G_mix_FB_x0.5.xml", "P4-DADS(x0.5)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_p4_128ports_1G_mix_FB.xml", "P4-DADS(x1)"),
        # ("/home/rdcn/ns-3.35-workshop/ns-3.35/EXP4/FlowMonitorData_p4_128ports_1G_mix_FB_x1.5.xml", "P4-DADS(x1.5)"),
    ]

    # Iterate over each file and calculate rxBytes
    for file_path, description in xml_files:
        total_rx_bytes = calculate_rx_bytes(file_path)
        print(f"Total rxBytes for {description}: {total_rx_bytes}")

if __name__ == "__main__":
    main()