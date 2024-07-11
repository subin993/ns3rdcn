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
    // 패킷이 어떤 주소에서 왔는지 확인합니다.
    Ipv4Address srcAddr = InetSocketAddress::ConvertFrom(from).GetIpv4();
    
    // ostringstream 객체를 사용하여 Ipv4Address 객체를 문자열로 변환합니다.
    std::ostringstream oss;
    oss << srcAddr;
    std::string addrStr = oss.str();
    
    // 파일 이름을 생성합니다. 주소를 파일 이름의 일부로 사용합니다.
    std::string fileName = "packet_log_dynamic_" + addrStr + ".txt";
    
    // 파일을 append 모드로 엽니다.
    std::ofstream logFile(fileName.c_str(), std::ios_base::app);
    
    // 파일이 제대로 열렸는지 확인합니다.
    if (!logFile.is_open())
    {
        NS_LOG_ERROR("Error opening log file!");
        return;
    }
    
    // 패킷 정보를 파일에 작성합니다.
    logFile << "Packet received at " << Simulator::Now().GetSeconds()
            << " seconds from " << srcAddr
            << " packet size: " << packet->GetSize() << std::endl;
    
    // 파일을 닫습니다.
    logFile.close();
}

void PacketsInQueueCallback(uint32_t oldValue, uint32_t newValue) {
    NS_LOG_UNCOND("Queue check at " << Simulator::Now ().GetSeconds ()
            << " seconds" << " and Queue size changed: " << oldValue << " -> " << newValue);
}

static std::vector<uint32_t> rxPkts;

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
  uint32_t openGymPort = 5556;
//   double steptime = 0.001;
  double steptime = 0.0001; // Dynamic-scheduling
//   double steptime = 0.01; // Fixed-scheduling


  // 로그 파일 초기화
  std::vector<std::string> ipAddresses = {
    "10.0.1.1", "10.0.3.1", "10.0.4.1", "10.0.5.1", "10.0.6.1", "10.0.7.1",
    "10.1.2.1", "10.1.4.1", "10.1.5.1", "10.1.7.1",
    "10.2.3.1", "10.2.5.1", "10.2.6.1",
    "10.3.6.1", "10.3.7.1",
    "10.6.7.1"
};

//   for (const auto& ip : ipAddresses) {
//       std::string filename = "packet_log_dynamic_32ports" + ip + ".txt";
//       std::ofstream logFile(filename, std::ios_base::trunc);
//       logFile.close();
//   }

  NodeContainer c;

  // Crate Node
  c.Create (8);

  // Gym 
  Ptr<MyGymEnv> Server = CreateObject<MyGymEnv> (steptime);
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
  Server->SetOpenGymInterface(openGymInterface);

  // Circuit switch path
  std::vector<std::pair<uint32_t, uint32_t>> nodePairs = {
    {0, 1}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7},
    {1, 2}, {1, 4}, {1, 5}, {1, 7},
    {2, 3}, {2, 5}, {2, 6},
    {3, 6}, {3, 7},
    {6, 7}
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
  for (int i = 0; i < 16; ++i) {
      devices.push_back(p2p.Install(nodeContainers[i]));
  }

  std::vector<Ptr<NetDeviceContainer>> devicePointers;
  for (auto& device : devices) {
      devicePointers.push_back(&device);
  }

  for (auto& pointer : devicePointers) {
      Server->AddNewNetDevice(pointer);
  }

  NS_LOG_INFO ("Assign IP Addresses.");

  // Create a vector to hold the IP base addresses from the given ipAddresses
  std::vector<std::string> ipBases;
  for (const auto &addr : ipAddresses) {
      std::string base = addr.substr(0, addr.find_last_of('.') + 1) + "0";
      ipBases.push_back(base);
  }

  // 주어진 ipAddresses는 미리 정의되어 있다고 가정합니다.
  std::vector<Ipv4InterfaceContainer> interfaces(ipAddresses.size());

  // 각 IP 주소에 대한 인터페이스 생성
  Ipv4AddressHelper ipv4;
  for (std::size_t i = 0; i < ipAddresses.size(); ++i) {
      std::string base = ipAddresses[i].substr(0, ipAddresses[i].find_last_of('.') + 1) + "0";
      ipv4.SetBase(base.c_str(), "255.255.255.0");
      interfaces[i] = ipv4.Assign(devices[i]);
  }

  // OnOffHelper를 이용한 애플리케이션 생성
  std::vector<ApplicationContainer> appsVector;
  uint16_t port = 9;
  // double beta = 1;
  // double alpha = 30 * beta;
  // double beta = 1;
  // double alpha = 5 * beta;
  double beta = 2;
  double alpha = 60 * beta;
NS_LOG_INFO("Assign IP Addresses3.");

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

  // Weighted random sampling approach (alternative: kernel density estimation)
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
  for (uint32_t i = 0; i <= 6; i++) {
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

  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dynamic-global-routing.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (3), routingStream);

  Simulator::Stop(Seconds(5));  // where x is the total duration



  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();

  // Serialize Flow Monitor data to xml
  flowMonitor->SerializeToXmlFile ("FlowMonitorData_dynamic_32ports_1G_mix_FB_INF.xml", true, true);
  
//   Simulator::Destroy ();
//   NS_LOG_INFO ("Done.");
  NS_LOG_INFO ("Before Destroying.");
  Simulator::Destroy ();
  NS_LOG_INFO ("After Destroying.");

}
