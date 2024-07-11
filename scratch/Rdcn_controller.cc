/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contributed by:  Luis Cortes (cortes@gatech.edu)
 */


// This script exercises global routing code in a mixed point-to-point
// and csma/cd environment.  We bring up and down interfaces and observe
// the effect on global routing.  We explicitly enable the attribute
// to respond to interface events, so that routes are recomputed
// automatically.
//
// Network topology
//
// 
//   
//             (shared csma/cd)
//       n2 -------------------
//      /                     | 
//     / p-p                  n5 ---------- n6
//   n1                             p-p
//   |                                      |
//   ---------------- n7 --------------------
//          p-p                 p-p
//
// - at time 1 CBR/UDP flow from n1 to n6's IP address on the n5/n6 link
// - at time 10, start similar flow from n1 to n6's address on the n1/n6 link
//
//  Order of events
//  At pre-simulation time, configure global routes.  Shortest path from
//  n1 to n6 is via the direct point-to-point link
//  At time 1s, start CBR traffic flow from n1 to n6
//  At time 2s, set the n1 point-to-point interface to down.  Packets
//    will be diverted to the n1-n2-n5-n6 path
//  At time 4s, re-enable the n1/n6 interface to up.  n1-n6 route restored.
//  At time 6s, set the n6-n1 point-to-point Ipv4 interface to down (note, this
//    keeps the point-to-point link "up" from n1's perspective).  Traffic will
//    flow through the path n1-n2-n5-n6
//  At time 8s, bring the interface back up.  Path n1-n6 is restored
//  At time 10s, stop the first flow.
//  At time 11s, start a new flow, but to n6's other IP address (the one
//    on the n1/n6 p2p link)
//  At time 12s, bring the n1 interface down between n1 and n6.  Packets
//    will be diverted to the alternate path
//  At time 14s, re-enable the n1/n6 interface to up.  This will change 
//    routing back to n1-n6 since the interface up notification will cause
//    a new local interface route, at higher priority than global routing
//  At time 16s, stop the second flow.

// - Tracing of queues and packet receptions to file "dynamic-global-routing.tr"

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
#include "ns3/flow-monitor-helper.h"

#include "ns3/mygym.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RDCN");

#include <sstream> // 이 헤더를 포함해야 합니다.

void ReceivedPacketCallback (Ptr<const Packet> packet, const Address &from)
{
    // 패킷이 어떤 주소에서 왔는지 확인합니다.
    Ipv4Address srcAddr = InetSocketAddress::ConvertFrom(from).GetIpv4();
    
    // ostringstream 객체를 사용하여 Ipv4Address 객체를 문자열로 변환합니다.
    std::ostringstream oss;
    oss << srcAddr;
    std::string addrStr = oss.str();
    
    // 파일 이름을 생성합니다. 주소를 파일 이름의 일부로 사용합니다.
    std::string fileName = "packet_log_" + addrStr + ".txt";
    
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

static void
CountRxPkts(uint32_t sinkId, Ptr<const Packet> packet, const Address & srcAddr)
{
  rxPkts[sinkId]++;
}

static void
PrintRxCount()
{
  uint32_t size = rxPkts.size();
  NS_LOG_UNCOND("RxPkts:");
  for (uint32_t i=0; i<size; i++){
    NS_LOG_UNCOND("---SinkId: "<< i << " RxPkts: " << rxPkts.at(i));
  }
}

int 
main (int argc, char *argv[])
{
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

  std::string prefix_file_name = "TcpVariantsComparison";
  bool flow_monitor = false;

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  LogComponentEnable ("RDCN", LOG_LEVEL_INFO);

  //opengym environment
  uint32_t openGymPort = 5555;
  double steptime = 1;


  // 로그 파일 초기화
  std::ofstream logFile1("packet_log_20.1.2.1.txt", std::ios_base::trunc);
  logFile1.close();
  
  std::ofstream logFile2("packet_log_10.1.2.1.txt", std::ios_base::trunc);
  logFile2.close();
  
  NodeContainer c;

  // Crate Node
  c.Create (7);

  // Gym 
  Ptr<MyGymEnv> Server = CreateObject<MyGymEnv> (steptime);
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
  Server->SetOpenGymInterface(openGymInterface);

  // Circuit switch path
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n2n6 = NodeContainer (c.Get (2), c.Get (6));
  NodeContainer n5n1 = NodeContainer (c.Get (5), c.Get (1));
  
  // Packet switch path
  NodeContainer n0n4 = NodeContainer (c.Get (0), c.Get (4));
  NodeContainer n4n3 = NodeContainer (c.Get (4), c.Get (3));
  NodeContainer n4n6 = NodeContainer (c.Get (4), c.Get (6));
  NodeContainer n5n4 = NodeContainer (c.Get (5), c.Get (4));
  
  // Additional
  
  // 여러가지 해본 부분들인데 지금은 필요 없는 부분
  ////////////////////////////////////////////////////////////////////////////////////////
  // Ptr<NodeContainer> Npointer01 = &n0n1;
  // Ptr<NodeContainer> Npointer12 = &n1n2;
  // Ptr<NodeContainer> Npointer23 = &n2n3;
  // Ptr<NodeContainer> Npointer26 = &n2n6;

  // Ptr<NodeContainer> Npointer04 = &n0n4;
  // Ptr<NodeContainer> Npointer43 = &n4n3;
  // Ptr<NodeContainer> Npointer46 = &n4n6;
  // Ptr<NodeContainer> Npointer51 = &n5n1;
  // Ptr<NodeContainer> Npointer54 = &n5n4;

  // Server->AddNewNode(Npointer04);
  // Server->AddNewNode(Npointer43);
  // Server->AddNewNode(Npointer46);
  // Server->AddNewNode(Npointer54);

  // Server->AddNewNode(Npointer01);
  // Server->AddNewNode(Npointer12);
  // Server->AddNewNode(Npointer23);
  // Server->AddNewNode(Npointer26);
  // Server->AddNewNode(Npointer51);
  ////////////////////////////////////////////////////////////////////////////////////////
  

  InternetStackHelper internet;
  internet.Install (c);

  // 패킷 스위치 BW 설정
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d4 = p2p.Install (n0n4);
  NetDeviceContainer d4d3 = p2p.Install (n4n3);
  NetDeviceContainer d4d6 = p2p.Install (n4n6);

  NetDeviceContainer d5d4 = p2p.Install (n5n4);

  Ptr<NetDeviceContainer> Dpointer04 = &(d0d4);
  Ptr<NetDeviceContainer> Dpointer43 = &(d4d3);
  Ptr<NetDeviceContainer> Dpointer46 = &(d4d6);
  Ptr<NetDeviceContainer> Dpointer54 = &(d5d4);

  // Ptr<PointToPointHelper> Cpointer1 = CreateObject<PointToPointHelper>();
  // Cpointer1 = &(p2p);
  // Ptr<PointToPointHelper> Cpointer1 = &(p2p);


  Server->AddNewNetDevice(Dpointer04);
  Server->AddNewNetDevice(Dpointer43);
  Server->AddNewNetDevice(Dpointer46);
  Server->AddNewNetDevice(Dpointer54);

  // Server->AddNewChannel(Cpointer1);

  // 서킷 스위치 BW 설정
  PointToPointHelper p2p2;
  p2p2.SetDeviceAttribute ("DataRate", StringValue ("2kbps"));
  p2p2.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer d0d1 = p2p2.Install (n0n1);
  NetDeviceContainer d1d2 = p2p2.Install (n1n2);
  NetDeviceContainer d2d3 = p2p2.Install (n2n3);
  NetDeviceContainer d2d6 = p2p2.Install (n2n6);

  NetDeviceContainer d5d1 = p2p.Install (n5n1);

  Ptr<NetDeviceContainer> Dpointer01 = &(d0d1);
  Ptr<NetDeviceContainer> Dpointer12 = &(d1d2);
  Ptr<NetDeviceContainer> Dpointer23 = &(d2d3);
  Ptr<NetDeviceContainer> Dpointer26 = &(d2d6);

  Ptr<NetDeviceContainer> Dpointer51 = &(d5d1);

  // Ptr<PointToPointHelper> Cpointer2 = &(p2p2);

  Server->AddNewNetDevice(Dpointer01);
  Server->AddNewNetDevice(Dpointer12);
  Server->AddNewNetDevice(Dpointer23);
  Server->AddNewNetDevice(Dpointer26);

  Server->AddNewNetDevice(Dpointer51);

  // Server->AddNewChannel(Cpointer2);

  // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;

  // Circuit switch Path
  Ipv4AddressHelper ip1;
  ip1.SetBase ("10.1.1.0", "255.255.255.0");
  ip1.Assign (d0d1);
  Ipv4AddressHelper ip2;
  ip2.SetBase ("10.1.2.0", "255.255.255.0");
  ip2.Assign (d1d2);
  Ipv4AddressHelper ip3;
  ip3.SetBase ("10.1.3.0", "255.255.255.0");
  ip3.Assign (d2d3);
  Ipv4AddressHelper ip4;
  ip4.SetBase ("10.1.4.0", "255.255.255.0");
  ip4.Assign (d2d6);
  Ipv4AddressHelper ip5;
  ip5.SetBase ("10.1.5.0", "255.255.255.0");
  ip5.Assign (d5d1);

  // Packet swtich Path
  Ipv4AddressHelper ip6;
  ip6.SetBase ("20.1.6.0", "255.255.255.0");
  ip6.Assign (d0d4);
  Ipv4AddressHelper ip7;
  ip7.SetBase ("20.1.7.0", "255.255.255.0");
  ip7.Assign (d4d3);
  Ipv4AddressHelper ip8;
  ip8.SetBase ("20.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i6 = ipv4.Assign (d4d6);
  // ip8.Assign (d4d6);
  Ipv4AddressHelper ip9;
  ip9.SetBase ("20.1.9.0", "255.255.255.0");
  ip9.Assign (d5d4);

  // 여러가지 해본 부분들인데 지금은 필요 없는 부분
  ////////////////////////////////////////////////////////////////////////////////////////
  // Ptr<Ipv4AddressHelper> Ipointer01 = &ip1;
  // Ptr<Ipv4AddressHelper> Ipointer12 = &ip2;
  // Ptr<Ipv4AddressHelper> Ipointer23 = &ip3;
  // Ptr<Ipv4AddressHelper> Ipointer26 = &ip4;
  // Ptr<Ipv4AddressHelper> Ipointer51 = &ip5;

  // Ptr<Ipv4AddressHelper> Ipointer04 = &ip6;
  // Ptr<Ipv4AddressHelper> Ipointer43 = &ip7;
  // Ptr<Ipv4AddressHelper> Ipointer46 = &ip8;
  // Ptr<Ipv4AddressHelper> Ipointer54 = &ip9;

  // Server->AddNewIp(Ipointer01);
  // Server->AddNewIp(Ipointer12);
  // Server->AddNewIp(Ipointer23);
  // Server->AddNewIp(Ipointer26);
  // Server->AddNewIp(Ipointer51);
  // Server->AddNewIp(Ipointer04);
  // Server->AddNewIp(Ipointer43);
  // Server->AddNewIp(Ipointer46);
  // Server->AddNewIp(Ipointer54);
  ////////////////////////////////////////////////////////////////////////////////////////

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Create the OnOff application to send UDP datagrams of size
  // 210 bytes at a rate of 448 Kb/s
  // NS_LOG_INFO ("Create Applications.");

  // Source Application
  uint16_t port = 9; 
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (i4i6.GetAddress (1), port));
  onoff.SetConstantRate (DataRate ("50Kbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (50));

  ApplicationContainer apps = onoff.Install (c.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  // Receive Application
  // Create an optional packet sink to receive these packets for c.Get(6)
  PacketSinkHelper sink1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer sinkApps1 = sink1.Install(c.Get(6));
  sinkApps1.Start (Seconds (1.0));
  sinkApps1.Stop (Seconds (10.0));
  Ptr<PacketSink> sinkInstance1 = DynamicCast<PacketSink> (sinkApps1.Get (0));
  sinkInstance1->TraceConnectWithoutContext ("Rx", MakeCallback (&ReceivedPacketCallback));

  // Create an optional packet sink to receive these packets for c.Get(3)
  PacketSinkHelper sink2 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer sinkApps2 = sink2.Install(c.Get(3));
  sinkApps2.Start (Seconds (1.0));
  sinkApps2.Stop (Seconds (10.0));
  Ptr<PacketSink> sinkInstance2 = DynamicCast<PacketSink> (sinkApps2.Get (0));
  sinkInstance2->TraceConnectWithoutContext ("Rx", MakeCallback (&ReceivedPacketCallback));

 
 // N3으로 향하는 트래픽의 Setdown 효과가 없는것으로 확인.
  Ptr<Node> n1 = c.Get (0);
  Ptr<Ipv4> ipv41 = n1->GetObject<Ipv4> ();
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


  // Trace sources 설정 (예: d0d4를 위한)
  d0d4.Get (0)->GetObject<PointToPointNetDevice>()->GetQueue()->TraceConnectWithoutContext("PacketsInQueue", MakeCallback(&PacketsInQueueCallback));
  // d0d1.Get (0)->GetObject<PointToPointNetDevice>()->GetQueue()->TraceConnectWithoutContext("PacketsInQueue", MakeCallback(&PacketsInQueueCallback));


  // Flow monitor
  FlowMonitorHelper flowHelper;
  if (flow_monitor)
  {
    flowHelper.InstallAll ();
  }

  // Count RX packets
  rxPkts.push_back(0);
  Ptr<PacketSink> pktSink = DynamicCast<PacketSink>(sinkApps1.Get(0));
  pktSink->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&CountRxPkts, 0));

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();

  
  if (flow_monitor)
    {
      flowHelper.SerializeToXmlFile (prefix_file_name + ".flowmonitor", true, true);
    }

  PrintRxCount();
  
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
