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

#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <sstream>
#include <iostream>

#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <stdlib.h>
#include <string>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MyGymEnv");

NS_OBJECT_ENSURE_REGISTERED (MyGymEnv);

MyGymEnv::MyGymEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = 0.1;

  Simulator::Schedule (Seconds(0.0), &MyGymEnv::ScheduleNextStateRead, this);
}

MyGymEnv::MyGymEnv (double stepTime)
{
  NS_LOG_FUNCTION (this);
  m_interval = stepTime;

  Simulator::Schedule (Seconds(0.0), &MyGymEnv::ScheduleNextStateRead, this);
}

void
MyGymEnv::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (Seconds(m_interval), &MyGymEnv::ScheduleNextStateRead, this);
  Notify();
}

MyGymEnv::~MyGymEnv ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
MyGymEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyGymEnv")
    .SetParent<OpenGymEnv> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<MyGymEnv> ()
  ;
  return tid;
}

void
MyGymEnv::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

void MyGymEnv::AddNewNetDevice(Ptr<NetDeviceContainer> dev)
{
    m_links.push_back(dev);
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
MyGymEnv::GetObservationSpace()
{
  uint32_t channelNum = 2;
  float low = 0.0;
  float high = 100.0;
  std::vector<uint32_t> shape = {channelNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("QueueLength", box);
  space->Add("QueueReceived", box);

  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
MyGymEnv::GetActionSpace()
{
  // uint32_t flowNum = 64;
  uint32_t flowNum = 64;
  float low = 0.0;
  float high = 100.0;
  std::vector<uint32_t> shape = {flowNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("DataRate", box);
  
  high = 10000.0;

  Ptr<OpenGymBoxSpace> box2 = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  space->Add("BM", box2);

  NS_LOG_UNCOND ("MyGetActionSpace: " << space);
  return space;
}

/*
Define game over condition
*/
bool
MyGymEnv::GetGameOver()
{
  bool isGameOver = false;
  bool test = false;
  static float stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      isGameOver = true;
  }
  // NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer>
MyGymEnv::GetObservation()
{
    // uint32_t flowNum = 64;
    uint32_t flowNum = 64;
    std::vector<Ptr<NetDeviceContainer>> devices(flowNum);
    std::vector<Ptr<Queue<Packet>>> queues(flowNum);
    std::vector<QueueSize> qSizes(flowNum);
    std::vector<uint32_t> qReceive(flowNum);
    std::vector<uint32_t> os(flowNum);
    std::vector<uint32_t> os2(flowNum);
    // std::vector<uint32_t> ct(flowNum);

    for (uint32_t i = 0; i < flowNum; i++) {
        devices[i] = m_links[i];
        queues[i] = devices[i]->Get(0)->GetObject<PointToPointNetDevice>()->GetQueue();
        qSizes[i] = queues[i]->GetCurrentSize();
        qReceive[i] = queues[i]->GetTotalReceivedPackets();
        // qReceive[i] = queues[i]->SetMaxSize();
        os[i] = qSizes[i].GetValue();

        // Throughput 측정에 활용 가능
        os2[i] = qReceive[i];
    }

    std::vector<uint32_t> shape = {flowNum,};
    Ptr<OpenGymBoxContainer<uint32_t>> box = CreateObject<OpenGymBoxContainer<uint32_t>>(shape);

    for (uint32_t i = 0; i < flowNum; i++) {
        box->AddValue(os[i]);
    }
    
    Ptr<OpenGymBoxContainer<uint32_t>> box_2 = CreateObject<OpenGymBoxContainer<uint32_t>>(shape);

    for (uint32_t i = 0; i < flowNum; i++) {
        box_2->AddValue(os2[i]);
    }

    Ptr<OpenGymDictContainer> data = CreateObject<OpenGymDictContainer>();
    data->Add("QueueLength", box);
    data->Add("QueueReceived", box_2);
    
    // for (uint32_t i = 0; i < flowNum; i++) {
    //     box->AddValue(os2[i]);
    // }
    
    // data->Add("QueueReceived", box);
    
    // NS_LOG_UNCOND ("MyGetObservation: " << data);

    return data;
}


/*
Define reward function
*/
float
MyGymEnv::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Define extra info. Optional
*/
std::string
MyGymEnv::GetExtraInfo()
{
  std::string myInfo = "testInfo";
  myInfo += "|123";
  // NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
  return myInfo;
}

/*
Execute received actions
*/
bool
MyGymEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    Ptr<OpenGymDictContainer> dict = DynamicCast<OpenGymDictContainer>(action);
    Ptr<OpenGymBoxContainer<uint32_t> > box = DynamicCast<OpenGymBoxContainer<uint32_t> >(dict->Get("DataRate"));
    Ptr<OpenGymBoxContainer<uint32_t> > box2 = DynamicCast<OpenGymBoxContainer<uint32_t> >(dict->Get("BM"));

    uint32_t flowNum = 64;
    std::vector<Ptr<NetDeviceContainer>> devices(flowNum);
    std::vector<uint32_t> dataRates(flowNum);
    std::vector<uint32_t> BM(flowNum);
    std::vector<std::string> drStrings(flowNum);

    uint32_t scaleFactor = 1; // Scale factor to convert Mbps to bps

    for (uint32_t i = 0; i < flowNum; i++) {
        devices[i] = m_links[i];
        // Perform the multiplication in a wider integer type to prevent overflow
        uint64_t scaledDataRate = static_cast<uint64_t>(box->GetValue(i)) * scaleFactor;
        // if (scaledDataRate > std::numeric_limits<uint32_t>::max()) {
        //     NS_LOG_ERROR("Data rate overflow for port " << i);
        //     return false; // Or handle the overflow appropriately
        // }
        dataRates[i] = static_cast<uint32_t>(scaledDataRate);
        drStrings[i] = std::to_string(dataRates[i]) + "Mbps"; // Append 'bps' to indicate bits per second
        uint32_t maxsize = box2->GetValue(i);
        BM[i] = maxsize;
    }

    std::vector<Ptr<Queue<Packet>>> queues(flowNum);
    std::vector<QueueSize> qSizes(flowNum);
    for (uint32_t i = 0; i < flowNum; i++) {
        // Set the DataRate attribute with the string including the 'bps' unit
        devices[i]->Get(0)->SetAttribute("DataRate", StringValue(drStrings[i]));
        devices[i]->Get(1)->SetAttribute("DataRate", StringValue(drStrings[i]));
        queues[i] = devices[i]->Get(0)->GetObject<PointToPointNetDevice>()->GetQueue();
        queues[i]->SetMaxSize2(BM[i]);
    }

    return true;
}




} // ns3 namespace