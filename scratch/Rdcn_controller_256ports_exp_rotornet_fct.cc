#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/random-variable-stream.h"

#include "ns3/mygym.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RDCN");

#include <sstream> // 이 헤더를 포함해야 합니다.

void LoadTrafficModel(std::vector<uint32_t>& flowSizes, std::vector<double>& flowCDF) {
    // std::ifstream file("/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/datamining.csv");
    // std::ifstream file("/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/websearch.csv");
    std::ifstream file("/home/rdcn/ns-3.35-workshop/ns-3.35/scratch/FB_Hadoop_Inter_Rack_FlowCDF.csv");
    std::string line;
    if (!file.is_open()) {
        NS_LOG_UNCOND("Error opening file");
        return;
    }

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        uint32_t size;
        double cdf;
        char comma;

        if (!(ss >> size >> comma >> cdf)) {
            NS_LOG_UNCOND("Failed to parse line: " << line);
            continue;
        }

        if (comma != ',') {
            NS_LOG_UNCOND("Parsing error, comma not found where expected");
            continue;
        }

        flowSizes.push_back(size);
        flowCDF.push_back(cdf);

        NS_LOG_UNCOND("Size: " << size << ", CDF: " << cdf);
    }
}


void ReceivedPacketCallback (Ptr<const Packet> packet, const Address &from)
{
    
    Ipv4Address srcAddr = InetSocketAddress::ConvertFrom(from).GetIpv4();
    
    
    std::ostringstream oss;
    oss << srcAddr;
    std::string addrStr = oss.str();
    std::string fileName = "packet_log_rotornet_" + addrStr + ".txt";
    std::ofstream logFile(fileName.c_str(), std::ios_base::app);
    if (!logFile.is_open())
    {
        NS_LOG_ERROR("Error opening log file!");
        return;
    }
    logFile << "Packet received at " << Simulator::Now().GetSeconds()
            << " seconds from " << srcAddr
            << " packet size: " << packet->GetSize() << std::endl;
    logFile.close();
}



void PacketsInQueueCallback(uint32_t oldValue, uint32_t newValue) {
    NS_LOG_UNCOND("Queue check at " << Simulator::Now ().GetSeconds ()
            << " seconds" << " and Queue size changed: " << oldValue << " -> " << newValue);
}

static std::vector<uint32_t> rxPkts;

// static void
// CountRxPkts(uint32_t sinkId, Ptr<const Packet> packet, const Address & srcAddr)
// {
//   rxPkts[sinkId]++;
// }

// static void
// PrintRxCount()
// {
//   uint32_t size = rxPkts.size();
//   NS_LOG_UNCOND("RxPkts:");
//   for (uint32_t i=0; i<size; i++){
//     NS_LOG_UNCOND("---SinkId: "<< i << " RxPkts: " << rxPkts.at(i));
//   }
// }

int 
main (int argc, char *argv[])
{
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

  std::string prefix_file_name = "TcpVariantsComparison";
  std::vector<uint32_t> flowSizes;
  std::vector<double> flowCDF;
  LoadTrafficModel(flowSizes, flowCDF);
//   bool flow_monitor = true;

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  LogComponentEnable ("RDCN", LOG_LEVEL_INFO);

  //opengym environment
  uint32_t openGymPort = 5557;
//   double steptime = 0.001;
  double steptime = 0.00005; 
  // double steptime = 0.0002; // 


  // 로그 파일 초기화
  std::vector<std::string> ipAddresses = {
  "10.0.1.1", "10.0.3.1", "10.0.4.1", "10.0.5.1", "10.0.6.1", "10.0.7.1", "10.0.8.1", "10.0.9.1", "10.0.10.1", "10.0.11.1", "10.0.12.1", "10.0.13.1", "10.0.14.1", "10.0.15.1", "10.0.16.1",
  "10.1.2.1", "10.1.4.1", "10.1.5.1", "10.1.6.1", "10.1.7.1", "10.1.8.1", "10.1.9.1", "10.1.10.1", "10.1.11.1", "10.1.12.1", "10.1.13.1", "10.1.14.1", "10.1.15.1", "10.1.16.1",
  "10.2.3.1", "10.2.5.1", "10.2.6.1", "10.2.7.1", "10.2.8.1", "10.2.9.1", "10.2.10.1", "10.2.11.1", "10.2.12.1", "10.2.13.1", "10.2.14.1", "10.2.15.1", "10.2.16.1",
  "10.3.4.1", "10.3.6.1", "10.3.7.1", "10.3.8.1", "10.3.9.1", "10.3.10.1", "10.3.11.1", "10.3.12.1", "10.3.13.1", "10.3.14.1", "10.3.15.1", "10.3.16.1",
  "10.4.5.1", "10.4.7.1", "10.4.8.1", "10.4.9.1", "10.4.10.1", "10.4.11.1", "10.4.12.1", "10.4.13.1", "10.4.14.1", "10.4.15.1", "10.4.16.1",
  "10.5.6.1", "10.5.8.1", "10.5.9.1", "10.5.10.1", "10.5.11.1", "10.5.12.1", "10.5.13.1", "10.5.14.1", "10.5.15.1", "10.5.16.1",
  "10.6.7.1", "10.6.9.1", "10.6.10.1", "10.6.11.1", "10.6.12.1", "10.6.13.1", "10.6.14.1", "10.6.15.1", "10.6.16.1",
  "10.7.8.1", "10.7.10.1", "10.7.11.1", "10.7.12.1", "10.7.13.1", "10.7.14.1", "10.7.15.1", "10.7.16.1",
  "10.8.9.1", "10.8.10.1", "10.8.11.1", "10.8.12.1", "10.8.13.1", "10.8.14.1", "10.8.15.1", "10.8.16.1",
  "10.9.10.1", "10.9.11.1", "10.9.12.1", "10.9.13.1", "10.9.14.1", "10.9.15.1", "10.9.16.1",
  "10.10.11.1", "10.10.12.1", "10.10.13.1", "10.10.14.1", "10.10.15.1", "10.10.16.1",
  "10.11.12.1", "10.11.13.1", "10.11.14.1", "10.11.15.1", "10.11.16.1",
  "10.12.13.1", "10.12.14.1", "10.12.15.1", "10.12.16.1",
  "10.13.14.1", "10.13.15.1", "10.13.16.1",
  "10.14.15.1", "10.14.16.1",
  "10.15.16.1"
  };



//   for (const auto& ip : ipAddresses) {
//       std::string filename = "packet_log_solstice_" + ip + ".txt";
//       std::ofstream logFile(filename, std::ios_base::trunc);
//       logFile.close();
//   }

  NodeContainer c;

  // Crate Node
  c.Create (18);

  // Gym 
  Ptr<MyGymEnv> Server = CreateObject<MyGymEnv> (steptime);
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
  Server->SetOpenGymInterface(openGymInterface);

  // Circuit switch path
    std::vector<std::pair<uint32_t, uint32_t>> nodePairs = {
    {0, 1}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}, {0, 16},
    {1, 2}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15}, {1, 16},
    {2, 3}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {2, 14}, {2, 15}, {2, 16},
    {3, 4}, {3, 6}, {3, 7}, {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15}, {3, 16},
    {4, 5}, {4, 7}, {4, 8}, {4, 9}, {4, 10}, {4, 11}, {4, 12}, {4, 13}, {4, 14}, {4, 15}, {4, 16},
    {5, 6}, {5, 8}, {5, 9}, {5, 10}, {5, 11}, {5, 12}, {5, 13}, {5, 14}, {5, 15}, {5, 16},
    {6, 7}, {6, 9}, {6, 10}, {6, 11}, {6, 12}, {6, 13}, {6, 14}, {6, 15}, {6, 16},
    {7, 8}, {7, 10}, {7, 11}, {7, 12}, {7, 13}, {7, 14}, {7, 15}, {7, 16},
    {8, 9}, {8, 10}, {8, 11}, {8, 12}, {8, 13}, {8, 14}, {8, 15}, {8, 16},
    {9, 10}, {9, 11}, {9, 12}, {9, 13}, {9, 14}, {9, 15}, {9, 16},
    {10, 11}, {10, 12}, {10, 13}, {10, 14}, {10, 15}, {10, 16},
    {11, 12}, {11, 13}, {11, 14}, {11, 15}, {11, 16},
    {12, 13}, {12, 14}, {12, 15}, {12, 16},
    {13, 14}, {13, 15}, {13, 16},
    {14, 15}, {14, 16},
    {15, 16}
};

  std::vector<NodeContainer> nodeContainers;
  for (const auto& pair : nodePairs) {
      nodeContainers.push_back(NodeContainer(c.Get(pair.first), c.Get(pair.second)));
  }

  InternetStackHelper internet;
  internet.Install(c);

  NS_LOG_INFO("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("1kbps"));
  p2p.SetChannelAttribute("Delay", StringValue("1us"));

  std::vector<NetDeviceContainer> devices;
  for (int i = 0; i < 128; ++i) {
      devices.push_back(p2p.Install(nodeContainers[i]));
  }

  std::vector<Ptr<NetDeviceContainer>> devicePointers;
  for (auto& device : devices) {
      devicePointers.push_back(&device);
  }

  for (auto& pointer : devicePointers) {
      Server->AddNewNetDevice(pointer);
  }


  // Server->AddNewChannel(Cpointer2);

  NS_LOG_INFO ("Assign IP Addresses.");

  // Create a vector to hold the IP base addresses from the given ipAddresses
  std::vector<std::string> ipBases;
  for (const auto &addr : ipAddresses) {
      std::string base = addr.substr(0, addr.find_last_of('.') + 1) + "0";
      ipBases.push_back(base);
  }

  
  std::vector<Ipv4InterfaceContainer> interfaces(ipAddresses.size());
  Ipv4AddressHelper ipv4;
  for (std::size_t i = 0; i < ipAddresses.size(); ++i) {
      std::string base = ipAddresses[i].substr(0, ipAddresses[i].find_last_of('.') + 1) + "0";
      ipv4.SetBase(base.c_str(), "255.255.255.0");
      interfaces[i] = ipv4.Assign(devices[i]);
  }
  NS_LOG_INFO ("Assign IP Addresses2.");


  std::vector<ApplicationContainer> appsVector;
  uint16_t port = 9;
  // double beta = 1;
  // double alpha = 30 * beta;
  // double beta = 1;
  // double alpha = 5 * beta;
  double beta = 1.55;
  double alpha = 60 * beta;
// Assuming pre-calculated flowSizes and flowCDF (from previous analysis)
std::vector<double> flowPDF(flowCDF.size()); // Initialize PDF

// Calculate flowPDF using numerical differentiation (refer to previous explanation)
for (unsigned int i = 1; i < flowCDF.size(); ++i) {
  flowPDF[i] = (flowCDF[i] - flowCDF[i-1]) / (flowSizes[i] - flowSizes[i-1]);
}

Ptr<UniformRandomVariable> randomVariable = CreateObject<UniformRandomVariable>();
randomVariable->SetAttribute("Min", DoubleValue(0.0));
randomVariable->SetAttribute("Max", DoubleValue(1.0));

for (std::size_t i = 0; i < nodePairs.size(); ++i) {
  double randVal = randomVariable->GetValue();

  // Weighted random sampling approach 
  double totalArea = 0.0;
  for (double pdfValue : flowPDF) {
    totalArea += pdfValue;
  }

  double chosenWeight = 0.0;
  uint32_t chosenIndex = 0;
  while (chosenWeight < randVal) {
    chosenWeight += flowPDF[chosenIndex] / totalArea;
    chosenIndex++;
  }

  uint32_t flowSize = flowSizes[chosenIndex - 1]; // Adjust for indexing

  // Generate inter-arrival time independent of CDF (e.g., exponential)
  Ptr<ExponentialRandomVariable> interArrivalVar = CreateObject<ExponentialRandomVariable>();
  interArrivalVar->SetAttribute("Mean", DoubleValue(1/alpha)); // Adjust mean as needed
  double interArrivalTime = interArrivalVar->GetValue();

  uint32_t nodeIndex = nodePairs[i].first;
  OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(interfaces[i].GetAddress(1), port));
  onoff.SetAttribute("OnTime", StringValue("ns3::ExponentialRandomVariable[Mean=" + std::to_string(1/alpha) + "]"));
  onoff.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=" + std::to_string(1/beta) + "]"));
  // onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
  // onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));

    std::string rate;
    if (flowSize <= 10000) {
        rate = std::to_string(flowSize * 8 / interArrivalTime) + "bps";  // Rate in kbps
        flowSize = flowSize;
        NS_LOG_UNCOND(rate);
    } else if (flowSize > 10000 && flowSize < 10000000) {
        rate = std::to_string((flowSize * 8 / interArrivalTime) / 1000) + "kbps";  // Rate in Mbps
        flowSize = flowSize/1000;
        NS_LOG_UNCOND(rate);
    } else if (flowSize >= 10000000 && flowSize < 10000000000) {
        rate = std::to_string((flowSize * 8 / interArrivalTime) / 1000000) + "Mbps";  // Rate in Gbps
        flowSize = flowSize/1000000;
        NS_LOG_UNCOND(rate);
    }
    onoff.SetAttribute("DataRate", StringValue(rate));
    onoff.SetAttribute("PacketSize", UintegerValue(flowSize));

    ApplicationContainer currentApp = onoff.Install(c.Get(nodeIndex));
    currentApp.Start(Seconds(0.1));
    // currentApp.Stop(Seconds(18.0));
    currentApp.Stop(Seconds(3.0));

    appsVector.push_back(currentApp);
    }
  NS_LOG_INFO ("Assign IP Addresses4.");


  // Receive Application
  for (uint32_t i = 0; i <=17; i++) {
      PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
      ApplicationContainer sinkApps = sink.Install(c.Get(i));
      sinkApps.Start (Seconds (0.1));
      sinkApps.Stop (Seconds (6.0));
      // Ptr<PacketSink> sinkInstance = DynamicCast<PacketSink> (sinkApps.Get (0));
      //sinkInstance->TraceConnectWithoutContext ("Rx", MakeCallback (&ReceivedPacketCallback));
  }

  // Create Flow Monitor
  FlowMonitorHelper flowMonitorHelper;
  Ptr<FlowMonitor> flowMonitor = flowMonitorHelper.InstallAll();
  flowMonitor->Start (Seconds (0.1));
  flowMonitor->Stop (Seconds (5.0));

 
 // N3으로 향하는 트래픽의 Setdown 효과가 없는것으로 확인.
  // Ptr<Node> n0 = c.Get (0);
  // Ptr<Ipv4> ipv41 = n0->GetObject<Ipv4> ();
  // The first ifIndex is 0 for loopback, then the first p2p is numbered 1,
  // then the next p2p is numbered 2
  // uint32_t ipv4ifIndex1 = 2;
  // Simulator::Schedule (Seconds (5),&Ipv4::SetDown,ipv41, ipv4ifIndex1);
  // Simulator::Schedule (Seconds (7),&Ipv4::SetUp,ipv41, ipv4ifIndex1);

  //  Ptr<Node> n5 = c.Get (5);
  // Ptr<Ipv4> ipv45 = n5->GetObject<Ipv4> ();
  // // The first ifIndex is 0 for loopback, then the first p2p is numbered 1,
  // // then the next p2p is numbered 2
  // uint32_t ipv4ifIndex5 = 2;

  // Simulator::Schedule (Seconds (2.01),&Ipv4::SetDown,ipv45, ipv4ifIndex5);

  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dynamic-global-routing.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (3), routingStream);

  Simulator::Stop(Seconds(5));  // where x is the total duration

  // Trace sources 설정 (예: d0d4를 위한)
  // d0d3.Get (0)->GetObject<PointToPointNetDevice>()->GetQueue()->TraceConnectWithoutContext("PacketsInQueue", MakeCallback(&PacketsInQueueCallback));
  // d0d1.Get (0)->GetObject<PointToPointNetDevice>()->GetQueue()->TraceConnectWithoutContext("PacketsInQueue", MakeCallback(&PacketsInQueueCallback));


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();

  // Serialize Flow Monitor data to xml
  flowMonitor->SerializeToXmlFile ("FlowMonitorData_GT_256ports_1G_mix_FB_230.xml", true, true);
  
//   Simulator::Destroy ();
//   NS_LOG_INFO ("Done.");
  NS_LOG_INFO ("Before Destroying.");
  Simulator::Destroy ();
  NS_LOG_INFO ("After Destroying.");

}
