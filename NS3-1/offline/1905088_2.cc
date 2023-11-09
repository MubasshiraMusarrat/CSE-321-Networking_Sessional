#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/stats-module.h"
#include "ns3/yans-wifi-helper.h"


// Default Network Topology
// mobile                static         mobile
//   Wifi 10.1.1.0                           Wifi 10.1.3.0
//                    AP                AP
//  *        *   *    *                 *    *    *         *
//  |  ....  |   |    |    10.1.2.0     |    |    |   ....  |
// n5       n6   n7   n0 -------------- n1   n2   n3        n4
//                       point-to-point

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiMobileExample");

uint32_t nNodes = 20;
uint32_t nFlows = 10;
uint32_t nPacketsPerSec = 100;
uint32_t nodeSpeed = 5;

double dataRate = 0;

uint16_t PortSend = 9;
uint16_t PortReceive = 9;

uint64_t TotalRx = 0;
uint32_t noDeliveredPackets = 0;
uint32_t noReceivedPackets = 0;

NodeContainer senders;
NodeContainer receivers;

Ipv4InterfaceContainer receiver_interfaces;

ApplicationContainer senderApps;
ApplicationContainer sinkApps;

void TotalRxCallBack(Ptr<const Packet> packet, const Address &from){
    TotalRx += packet->GetSize() * 8;
}

void DeliveryCallBack(Ptr<const Packet> packet){
    noDeliveredPackets++;
}

void ReceivedCallBack(Ptr<const Packet> packet, const Address &from){
    noReceivedPackets++;
}

void AssignAppsToSender(int newFlow){
    uint32_t turn;

    if(newFlow<static_cast<int>(nNodes/2)){
        turn = newFlow;
    }
    else{
        turn = nNodes/2;
    }
    for(uint32_t i=0; i<(turn-1);i++){
        OnOffHelper sender_helper("ns3::TcpSocketFactory", (InetSocketAddress(receiver_interfaces.GetAddress(i),PortSend)));
        sender_helper.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
        sender_helper.SetAttribute("PacketSize", UintegerValue(1024));
        sender_helper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        sender_helper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        senderApps.Add(sender_helper.Install(senders.Get(i)));
    }
    newFlow -= (nNodes/2);
    if(newFlow <= 0){
        return;
    }
    else{
        PortSend++;
        AssignAppsToSender(newFlow);
    }  
}

void AssignAppsToSink(int newFlow){
    uint32_t turn;

    if(newFlow<static_cast<int>(nNodes/2)){
        turn = newFlow;
    }
    else{
        turn = nNodes/2;
    }

    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(), PortReceive));
    for(uint32_t i=0; i<(turn-1);i++){
        sinkApps.Add(sinkHelper.Install(receivers.Get(i)));
    }
    newFlow -= (nNodes/2);
    if(newFlow <= 0){
        return;
    }
    else{
        PortReceive++;
        AssignAppsToSink(newFlow);
    }
}

int main(int argc, char* argv[]){
    uint32_t payloadSize = 1100; // packet size = 1024 bytes, extra bytes for the potentianl
                                 // overhead


    CommandLine cmd(__FILE__);
    cmd.AddValue("nNodes", "Number of Nodes", nNodes);
    cmd.AddValue("nFlows", "Number of Flows", nFlows);
    cmd.AddValue("nPacketsPerSec", "Number of Packets per Second", nPacketsPerSec);
    cmd.AddValue("Speed", "Speed of Nodes", nodeSpeed);

    cmd.Parse(argc, argv);

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper>stream1  = asciiTraceHelper.CreateFileStream("scratch/mobile1.dat", std::ios::app);
    Ptr<OutputStreamWrapper>stream2  = asciiTraceHelper.CreateFileStream("scratch/mobile2.dat", std::ios::app);

    dataRate = 5* 1024 * 8;
    double p2pDataRate = dataRate*0.8;

    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    senders.Create(nNodes / 2 - 1);
    receivers.Create(nNodes / 2 - 1);

    NodeContainer gateway;
    gateway.Create(2);

    //Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    //em->SetAttribute("ErrorRate", DoubleValue(0.00001));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", DataRateValue(DataRate(p2pDataRate)));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));
    //pointToPoint.SetDeviceAttribute ("ReceiveErrorModel", PointerValue(em));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(gateway);

    NodeContainer wifiApNodeSender = gateway.Get(0);
    NodeContainer wifiApNodeReceiver = gateway.Get(1);

    YansWifiChannelHelper channelSender = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phySender;
    phySender.SetChannel(channelSender.Create());

    YansWifiChannelHelper channelReceiver = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phyReceiver;
    phyReceiver.SetChannel(channelReceiver.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::IdealWifiManager");

    WifiMacHelper macSender;
    Ssid ssidSender = Ssid("ns-3-ssid-Sender");

    WifiMacHelper macReceiver;
    Ssid ssidReceiver = Ssid("ns-3-ssid-Receiver");

    NetDeviceContainer staDevicesSenders;
    macSender.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssidSender),"ActiveProbing",BooleanValue(false));
    for(uint32_t i=0; i<(nNodes/2-1); i++){
        staDevicesSenders.Add(wifi.Install(phySender, macSender, senders.Get(i)));
    }

    NetDeviceContainer staDevicesReceivers;
    macReceiver.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssidReceiver),"ActiveProbing",BooleanValue(false));
    for(uint32_t i=0; i<(nNodes/2-1); i++){
    staDevicesReceivers.Add(wifi.Install(phyReceiver, macReceiver, receivers.Get(i)));
    }

    NetDeviceContainer apDevicesSender;
    macSender.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidSender));
    apDevicesSender = wifi.Install(phySender, macSender, wifiApNodeSender);

    NetDeviceContainer apDevicesReceiver;
    macReceiver.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidReceiver));
    apDevicesReceiver = wifi.Install(phyReceiver, macReceiver, wifiApNodeReceiver);

    MobilityHelper mobility;
    Ptr<RandomRectanglePositionAllocator> PositionAllocator = CreateObject<RandomRectanglePositionAllocator>();
    Ptr<UniformRandomVariable> Xstream = CreateObject<UniformRandomVariable>();
    Xstream->SetAttribute("Min",DoubleValue(0.0));
    Xstream->SetAttribute("Max",DoubleValue(nNodes/2));
    PositionAllocator->SetX(Xstream);

    Ptr<UniformRandomVariable> Ystream = CreateObject<UniformRandomVariable>();
    Ystream->SetAttribute("Min",DoubleValue(0.0));
    Ystream->SetAttribute("Max",DoubleValue(nNodes/2));
    PositionAllocator->SetY(Ystream);

    mobility.SetPositionAllocator(PositionAllocator);
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Mode", StringValue("Time"),
                                "Time", TimeValue(Seconds(1.0)),
                                "Speed", StringValue("ns3::ConstantRandomVariable[Constant="+std::to_string(nodeSpeed)+"]"),
                                "Bounds", RectangleValue(Rectangle(0,nNodes,0,nNodes)));
    mobility.Install(senders);
    mobility.Install(receivers);
    
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(gateway);

    InternetStackHelper stack;
    stack.Install(senders);
    stack.Install(receivers);
    stack.Install(gateway);

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer sender_interfaces;
    address.SetBase("10.1.1.0", "255.255.255.0");
    sender_interfaces.Add(address.Assign(staDevicesSenders));
    sender_interfaces.Add(address.Assign(apDevicesSender));
    
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
    
    address.SetBase("10.1.3.0", "255.255.255.0");
    receiver_interfaces.Add(address.Assign(staDevicesReceivers));
    receiver_interfaces.Add(address.Assign(apDevicesReceiver));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    AssignAppsToSender(static_cast<int>(nFlows));
    AssignAppsToSink(static_cast<int>(nFlows));

    for(uint32_t i=0; i<(nNodes/2-1); i++){ //Trace delivered packets
        uint32_t nApp = senders.Get(i)->GetNApplications();
        for(uint32_t j=0; j<nApp; j++){
            //std::cout<<j<<std::endl;
            Ptr<OnOffApplication> OnOffApp = StaticCast<OnOffApplication>(senders.Get(i)->GetApplication(j));
            OnOffApp->TraceConnectWithoutContext("Tx",MakeCallback(&DeliveryCallBack));
        }
    }

    for(uint32_t i=0; i<(nNodes/2-1); i++){ //Trace received packets
        uint32_t nApp = receivers.Get(i)->GetNApplications();
        for(uint32_t j=0; j<nApp; j++){
            Ptr<PacketSink> sink = StaticCast<PacketSink>(receivers.Get(i)->GetApplication(j));
            sink->TraceConnectWithoutContext("Rx", MakeCallback(&TotalRxCallBack));
            sink->TraceConnectWithoutContext("Rx", MakeCallback(&ReceivedCallBack));
        }
    }
    
    sinkApps.Start(Seconds(0.0));
    senderApps.Start(Seconds(1.0));

    senderApps.Stop(Seconds(10.0));
    sinkApps.Stop(Seconds(10.0));
    
    Simulator::Stop(Seconds(11.0));
    Simulator::Run();

    Time now = Simulator::Now();
    double cur = TotalRx/(1e6 * now.GetSeconds());
    *stream1->GetStream() << cur << std::endl;
    //std::cout<< "Throughput:" << cur << std::endl;

    double pktRatio = noReceivedPackets/(noDeliveredPackets * now.GetSeconds());
    *stream2->GetStream() << pktRatio << std::endl;
    //std::cout<< "Delivery ratio:" << pktRatio << std::endl;

    Simulator::Destroy();

    return 0;
}
