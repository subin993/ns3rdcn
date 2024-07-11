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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-static-routing-helper.h"

#include "ns3/log.h"
// #include "ns3/trace-helper.h"

// Default Network Topology
//
//       10.1.1.0
// n0              n2
//  |       X       |
// n1              n3
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RDCN");

void ReceivedPacketCallback (Ptr<const Packet> packet, const Address &from)
{
  std::cout << "Packet received at " << Simulator::Now ().GetSeconds () 
            << " seconds from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ()
            << " packet size: " << packet->GetSize () << std::endl;
}

void MacRxCallback (std::string context, Ptr<const Packet> packet)
{
  std::cout << "MacRx at " << Simulator::Now ().As (Time::S)
            << ", Packet received: " << packet->ToString () << std::endl;
}

void PrintRoutingTable (Ptr<Node> node)
{
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  Ptr<Ipv4RoutingProtocol> routingProtocol = ipv4->GetRoutingProtocol ();
  Ptr<Ipv4ListRouting> listRouting = DynamicCast<Ipv4ListRouting> (routingProtocol);
  Ptr<Ipv4GlobalRouting> globalRouting = DynamicCast<Ipv4GlobalRouting> (routingProtocol);

  if (globalRouting)
  {
    std::cout << "Routing table for node: " << node->GetId () << std::endl;
    globalRouting->PrintRoutingTable (Create<OutputStreamWrapper> (&std::cout));
  }

  if (listRouting)
  {
    std::cout << "Number of routing protocols: " << listRouting->GetNRoutingProtocols () << std::endl;
    listRouting->PrintRoutingTable (Create<OutputStreamWrapper> (&std::cout));

    for (uint32_t i = 0; i < listRouting->GetNRoutingProtocols (); i++)
    {
      int16_t priority;
      Ptr<Ipv4RoutingProtocol> subProtocol = listRouting->GetRoutingProtocol (i, priority);
      std::cout << "Routing protocol #" << i << " is " << subProtocol->GetInstanceTypeId ().GetName ()
                << " with priority " << priority << std::endl;
    }
  }
  else
  {
    std::cout << "No list routing protocol found on this node." << std::endl;
  }
}

void ChangeRoute (Ptr<Node> node, Ipv4Address dest, Ipv4Address nextHop)
{
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  NS_ASSERT (ipv4);
  Ptr<Ipv4RoutingProtocol> routingProtocol = ipv4->GetRoutingProtocol ();
  NS_ASSERT (routingProtocol);

  Ptr<Ipv4ListRouting> listRouting = DynamicCast<Ipv4ListRouting> (routingProtocol);

  if (listRouting)
  {
    for (uint32_t i = 0; i < listRouting->GetNRoutingProtocols ();  i++)
    {
      int16_t priority;
      Ptr<Ipv4RoutingProtocol> subProtocol = listRouting->GetRoutingProtocol (i, priority);
      Ptr<Ipv4StaticRouting> staticRouting = DynamicCast<Ipv4StaticRouting> (subProtocol);

      if (staticRouting)
      {

        // 라우팅 테이블에서 라우트의 개수를 얻습니다.
        uint32_t numRoutes = staticRouting->GetNRoutes ();

        // 목적지 주소가 10.1.2.2인 라우트를 찾아 삭제합니다.
        for (uint32_t i = 0; i < numRoutes; i++)
          {
            Ipv4RoutingTableEntry routeEntry = staticRouting->GetRoute (i);
            if (routeEntry.GetDest () == Ipv4Address ("10.1.2.2"))
              {
                // 찾은 라우트를 삭제합니다.
                staticRouting->RemoveRoute (i);
                break;  // 루프를 종료합니다.
              }
            if (routeEntry.GetDest () == Ipv4Address ("10.1.4.2"))
              {
                // 찾은 라우트를 삭제합니다.
                staticRouting->RemoveRoute (i);
                break;  // 루프를 종료합니다.
              }
          }
        // staticRouting->RemoveRoute (1);
        staticRouting->AddHostRouteTo (dest, nextHop, 1);
        std::cout << "Successfully added route to " << dest << " via " << nextHop << std::endl;
        PrintRoutingTable(node);  // Print the updated routing table
        return;
      }
    }
  }

  NS_FATAL_ERROR ("Failed to find a static routing protocol.");
}

// New
void ChangeDestinationAddress (UdpEchoClientHelper echoClient, Ipv4Address newAddress)
{
  echoClient.SetAttribute ("RemoteAddress", AddressValue (InetSocketAddress (newAddress, 9)));
}



int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("RDCN", LOG_LEVEL_INFO);
  // LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  // LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpTraceClient", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // LogComponentEnable ("Ipv4GlobalRouting", LOG_LEVEL_ALL);
  // LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);


  // 노드 생성
  NodeContainer nodes;
  nodes.Create (4);

  // P2P 링크 설정
  PointToPointHelper p2p[4];
  NetDeviceContainer dev[4];
  Ipv4InterfaceContainer iface[4];

  std::string dataRates[4] = {"5kbps", "5kbps", "100kbps", "100kbps"};
  std::string delays[4] = {"2ms", "2ms", "1ms", "1ms"};
  std::string ipBases[4] = {"10.1.1.0", "10.1.2.0", "10.1.3.0", "10.1.4.0"};

  NodeContainer nodePairs[4] = {NodeContainer(nodes.Get(0), nodes.Get(1)),
                                NodeContainer(nodes.Get(0), nodes.Get(3)),
                                NodeContainer(nodes.Get(2), nodes.Get(1)),
                                NodeContainer(nodes.Get(2), nodes.Get(3))};

  for (int i = 0; i < 4; ++i)
  {
    p2p[i].SetDeviceAttribute ("DataRate", StringValue (dataRates[i]));
    p2p[i].SetChannelAttribute ("Delay", StringValue (delays[i]));
    dev[i] = p2p[i].Install (nodePairs[i]);
  }

  // 프로토콜 스택 설치
  InternetStackHelper stack;

  Ipv4GlobalRoutingHelper globalRouting;

  Ipv4ListRoutingHelper listRouting;
  Ipv4StaticRoutingHelper staticRouting;

  listRouting.Add (staticRouting, 0);  // 여기서 Ipv4StaticRouting을 추가합니다.
  // listRouting.Add (globalRouting, -10);
  
  // stack.SetRoutingHelper (globalRouting);
  stack.SetRoutingHelper (listRouting);  // 여기서 Ipv4ListRouting을 사용합니다.

  stack.Install (nodes);

  // IP 주소 설정
  Ipv4AddressHelper address;

  for (int i = 0; i < 4; ++i)
  {
    address.SetBase (ipBases[i].c_str(), "255.255.255.0");
    iface[i] = address.Assign (dev[i]);
  }

  // UDP 애플리케이션 설정
//   uint16_t port = 9;   // Discard port (RFC 863)
//   OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address("10.1.2.2"), port));
//   onoff.SetConstantRate(DataRate("100kbps"));
//   onoff.SetAttribute("PacketSize", UintegerValue(500));

//   ApplicationContainer apps;
//   // for (uint32_t i = 0; i < 10; i++)
//   //   {
//   //     apps.Add(onoff.Install(nodes.Get(1)));
//   //   }
//   apps.Add(onoff.Install(nodes.Get(1)));
  
//   apps.Start(Seconds(0.0));
//   apps.Stop(Seconds(10.2));

//   PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
//   ApplicationContainer sinkApps = sink.Install(nodes.Get(3));
//   sinkApps.Start(Seconds(0.0));
//   sinkApps.Stop(Seconds(9.0));

//   Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
//   sink1->TraceConnectWithoutContext ("Rx", MakeCallback (&ReceivedPacketCallback));


  // New
  //////////////////////////////
  uint16_t port = 9;   // Discard port (RFC 863)

  UdpEchoClientHelper senderApp (InetSocketAddress(Ipv4Address("10.1.2.2"), port));
  senderApp.SetAttribute ("PacketSize", UintegerValue(500));
  senderApp.SetAttribute ("MaxPackets", UintegerValue (1));
  senderApp.SetAttribute ("Interval", TimeValue (Seconds (1.0)));

  ApplicationContainer clientApps = senderApp.Install(nodes.Get(1));
  clientApps.Start(Seconds(0.0));
  clientApps.Stop(Seconds(11.2));

  UdpEchoServerHelper recvApp (port);

  ApplicationContainer serverApps = recvApp.Install (nodes.Get(3));
  serverApps.Start(Seconds(0.0));
  serverApps.Stop(Seconds(10.0));

  //////////////////////////////





  // Simulator::Schedule (Seconds (0.1), &PrintRoutingTable, nodes.Get (1));
  Simulator::Schedule ( Seconds (2), &ChangeDestinationAddress, senderApp, Ipv4Address("10.1.4.2") );
  Simulator::Schedule (Seconds (2), &ChangeRoute, nodes.Get (1), iface[3].GetAddress (1), iface[2].GetAddress (0));
  // Simulator::Schedule (Seconds (0.11), &ChangeRoute, nodes.Get (3), iface[0].GetAddress (1), iface[2].GetAddress (0));
  
  Simulator::Schedule ( Seconds (5), &ChangeDestinationAddress, senderApp, Ipv4Address("10.1.2.2") );
  Simulator::Schedule (Seconds (5), &ChangeRoute, nodes.Get (1), iface[1].GetAddress (1), iface[0].GetAddress (0));
  // Simulator::Schedule (Seconds (1.91), &ChangeRoute, nodes.Get (3), iface[0].GetAddress (1), iface[0].GetAddress (0));

  Simulator::Schedule ( Seconds (8), &ChangeDestinationAddress, senderApp, Ipv4Address("10.1.4.2") );
  Simulator::Schedule (Seconds (8), &ChangeRoute, nodes.Get (1), iface[3].GetAddress (1), iface[2].GetAddress (0));
  // Simulator::Schedule (Seconds (0.11), &ChangeRoute, nodes.Get (3), iface[0].GetAddress (1), iface[2].GetAddress (0));

  // PointToPointNetDevice의 Trace를 활성화합니다.
  for (int i = 0; i < 4; ++i)
  {
    p2p[i].EnablePcapAll ("my_p2p_trace");
  }

  // 시뮬레이션 실행
  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}