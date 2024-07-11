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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RDCN");

void ReceivedPacketCallback (Ptr<const Packet> packet, const Address &from)
{
  std::cout << "Packet received at " << Simulator::Now ().GetSeconds ()
            << " seconds from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ()
            << " packet size: " << packet->GetSize () << std::endl;
}

int 
main (int argc, char *argv[])
{
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  LogComponentEnable ("RDCN", LOG_LEVEL_INFO);

  NodeContainer c;

  // Crate Node
  c.Create (7);

  // Circuit switch path
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n2n6 = NodeContainer (c.Get (2), c.Get (6));
  
  
  // Packet switch path
  NodeContainer n0n4 = NodeContainer (c.Get (0), c.Get (4));
  NodeContainer n4n3 = NodeContainer (c.Get (4), c.Get (3));
  NodeContainer n4n6 = NodeContainer (c.Get (4), c.Get (6));
  
  // Additional
  NodeContainer n5n1 = NodeContainer (c.Get (5), c.Get (1));
  NodeContainer n5n4 = NodeContainer (c.Get (5), c.Get (4));

  InternetStackHelper internet;
  internet.Install (c);

  // 패킷 스위치 BW 설정
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("4kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d4 = p2p.Install (n0n4);
  NetDeviceContainer d4d3 = p2p.Install (n4n3);
  NetDeviceContainer d4d6 = p2p.Install (n4n6);

  NetDeviceContainer d5d4 = p2p.Install (n5n4);

  d0d4.Get(0)->SetAttribute("DataRate", StringValue ("400kbps"));
  d0d4.Get(1)->SetAttribute("DataRate", StringValue ("400kbps"));
  d4d3.Get(0)->SetAttribute("DataRate", StringValue ("400kbps"));
  d4d3.Get(1)->SetAttribute("DataRate", StringValue ("400kbps"));
  d4d6.Get(0)->SetAttribute("DataRate", StringValue ("400kbps"));
  d4d6.Get(1)->SetAttribute("DataRate", StringValue ("400kbps"));
  d5d4.Get(1)->SetAttribute("DataRate", StringValue ("400kbps"));
  d5d4.Get(1)->SetAttribute("DataRate", StringValue ("400kbps"));




  // 서킷 스위치 BW 설정
  PointToPointHelper p2p2;
  p2p2.SetDeviceAttribute ("DataRate", StringValue ("2kbps"));
  p2p2.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer d0d1 = p2p2.Install (n0n1);
  NetDeviceContainer d1d2 = p2p2.Install (n1n2);
  NetDeviceContainer d2d3 = p2p2.Install (n2n3);
  NetDeviceContainer d2d6 = p2p2.Install (n2n6);

  NetDeviceContainer d5d1 = p2p.Install (n5n1);

  // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;

  // Circuit switch Path
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i6 = ipv4.Assign (d2d6);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  ipv4.Assign (d5d1);
  
  // Packet swtich Path
  ipv4.SetBase ("20.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i4 = ipv4.Assign (d0d4);
  ipv4.SetBase ("20.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i3 = ipv4.Assign (d4d3);
  ipv4.SetBase ("20.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i6 = ipv4.Assign (d4d6);

  ipv4.SetBase ("20.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i5i4 = ipv4.Assign (d5d4);

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Create the OnOff application to send UDP datagrams of size
  // 210 bytes at a rate of 448 Kb/s
  // NS_LOG_INFO ("Create Applications.");

  // Source Application
  uint16_t port = 9;   // Discard port (RFC 863)
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
  uint32_t ipv4ifIndex1 = 2;
  Simulator::Schedule (Seconds (2),&Ipv4::SetDown,ipv41, ipv4ifIndex1);

   Ptr<Node> n5 = c.Get (5);
  Ptr<Ipv4> ipv45 = n5->GetObject<Ipv4> ();
  // The first ifIndex is 0 for loopback, then the first p2p is numbered 1,
  // then the next p2p is numbered 2
  uint32_t ipv4ifIndex5 = 2;

  Simulator::Schedule (Seconds (2.01),&Ipv4::SetDown,ipv45, ipv4ifIndex5);

  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dynamic-global-routing.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (3), routingStream);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
