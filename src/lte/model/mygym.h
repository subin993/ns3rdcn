/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Technische Universität Berlin
 *
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
 * Author: Piotr Gawlowicz <gawlowicz@tkn.tu-berlin.de>
 */


#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/nstime.h"

#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <stdlib.h>
#include <string>

namespace ns3 {

class MyGymEnv : public OpenGymEnv
{
public:
  MyGymEnv ();
  MyGymEnv (double stepTime);
  virtual ~MyGymEnv ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  bool GetGameOver();
  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);

  
  void AddNewNetDevice(Ptr<NetDeviceContainer> dev);
  std::vector<Ptr<NetDeviceContainer>> m_links;

  // 여러가지 해본 부분들인데 지금은 필요 없는 부분
  ////////////////////////////////////////////////////////////////////////////////////////
  // void AddNewNode(Ptr<NodeContainer> node);
  // void AddNewChannel(Ptr<PointToPointHelper> cha);
  // void AddNewIp(Ptr<Ipv4AddressHelper> ip);
  // std::vector<Ptr<NodeContainer>> m_node;
  // std::vector<Ptr<PointToPointHelper>> m_channels;
  // std::vector<Ptr<Ipv4AddressHelper>> m_ip;
  // void ChangeChannelAttribute (Ptr<PointToPointHelper> cha ,std::string str);
  // void InstallDevice (Ptr<PointToPointHelper> cha, Ptr<NetDeviceContainer> dev, Ptr<NodeContainer> node);
  // void AssignIp (Ptr<NetDeviceContainer> dev, Ptr<Ipv4AddressHelper> ip);
  ////////////////////////////////////////////////////////////////////////////////////////

private:
  void ScheduleNextStateRead();

  double m_interval;
};

}


#endif // MY_GYM_ENTITY_H
