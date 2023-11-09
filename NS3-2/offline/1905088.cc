#include "tutorial-app.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-highspeed.h"
#include "ns3/tcp-westwood-plus.h"
#include<math.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TaskA");

AsciiTraceHelper asciiTraceHelper;
Ptr<OutputStreamWrapper> stream1 =
    asciiTraceHelper.CreateFileStream("scratch/taskA/1.dat", std::ios::app);
Ptr<OutputStreamWrapper> stream2 =
    asciiTraceHelper.CreateFileStream("scratch/taskA/2.dat", std::ios::app);
Ptr<OutputStreamWrapper> stream3a =
    asciiTraceHelper.CreateFileStream("scratch/taskA/3a.dat");
Ptr<OutputStreamWrapper> stream3b =
    asciiTraceHelper.CreateFileStream("scratch/taskA/3b.dat");
Ptr<OutputStreamWrapper> stream4 =
    asciiTraceHelper.CreateFileStream("scratch/taskA/4.dat", std::ios::app);
Ptr<OutputStreamWrapper> stream5 =
    asciiTraceHelper.CreateFileStream("scratch/taskA/5.dat", std::ios::app);

static void
CwndChange(uint32_t oldCwnd, uint32_t newCwnd)
{
    //NS_LOG_UNCOND(newCwnd << "\t" << Simulator::Now().GetSeconds());
    *stream3a->GetStream() << (newCwnd / 1e6) << "\t" << Simulator::Now().GetSeconds() << std::endl;
}

static void
CwndChange2(uint32_t oldCwnd, uint32_t newCwnd)
{
   // NS_LOG_UNCOND(newCwnd << "\t" << Simulator::Now().GetSeconds());
    *stream3b->GetStream() << (newCwnd / 1e6) << "\t" << Simulator::Now().GetSeconds() << std::endl;
}

int
main(int argc, char* argv[])
{
    std::string tcpVariant2 = "ns3::TcpHighSpeed"; // "ns3::TcpWestwoodPlus"; // "ns3::TcpAdaptiveReno"
    double bottleneckDataRate = 50;
    double errorValue = -6;
    uint32_t packetSize = 1024;

    CommandLine cmd(__FILE__);
    cmd.AddValue("tcp2", "Name of TCP variant 2", tcpVariant2);
    cmd.AddValue("bottleneckDataRate", "Bottleneck bandwidth in bps", bottleneckDataRate);
    cmd.AddValue("errorValue", "Error rate", errorValue);
    cmd.Parse(argc, argv);

    double bandwidthDelayProduct = bottleneckDataRate * 0.1 / packetSize;
    errorValue = pow(10, errorValue);
    bottleneckDataRate = bottleneckDataRate * 1e6;

    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1472));

    PointToPointHelper pointToPointLeaves;
    pointToPointLeaves.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    pointToPointLeaves.SetChannelAttribute("Delay", StringValue("1ms"));
    pointToPointLeaves.SetQueue("ns3::DropTailQueue",
                                "MaxSize",
                                StringValue(std::to_string((int)bandwidthDelayProduct) + "p"));

    PointToPointHelper pointToPointBottleNeck;
    pointToPointBottleNeck.SetDeviceAttribute("DataRate",
                                              DataRateValue(DataRate(bottleneckDataRate)));
    pointToPointBottleNeck.SetChannelAttribute("Delay", StringValue("100ms"));

    PointToPointDumbbellHelper dumbbell(2,
                                        pointToPointLeaves,
                                        2,
                                        pointToPointLeaves,
                                        pointToPointBottleNeck);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(errorValue));
    dumbbell.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    InternetStackHelper stack1;
    stack1.Install(dumbbell.GetLeft(0));
    stack1.Install(dumbbell.GetRight(0));
    stack1.Install(dumbbell.GetLeft());
    stack1.Install(dumbbell.GetRight());

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue(tcpVariant2));
    InternetStackHelper stack2;
    stack2.Install(dumbbell.GetLeft(1));
    stack2.Install(dumbbell.GetRight(1));

    dumbbell.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"),
                                 Ipv4AddressHelper("10.3.1.0", "255.255.255.0"),
                                 Ipv4AddressHelper("10.2.1.0", "255.255.255.0"));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    FlowMonitorHelper flowHelper;
    flowHelper.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(1.0)));
    Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();

    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
                                InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer Flow1receiver = sinkHelper.Install(dumbbell.GetRight(0));
    ApplicationContainer Flow2receiver = sinkHelper.Install(dumbbell.GetRight(1));

    Flow1receiver.Start(Seconds(0.0));
    Flow2receiver.Start(Seconds(0.0));

    Flow1receiver.Stop(Seconds(10.0));
    Flow2receiver.Stop(Seconds(10.0));

    Ptr<Socket> ns3TcpSocket1 =
        Socket::CreateSocket(dumbbell.GetLeft(0), TcpSocketFactory::GetTypeId());
    ns3TcpSocket1->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChange));

    Ptr<TutorialApp> app1 = CreateObject<TutorialApp>();
    Address sinkAddress1(InetSocketAddress(dumbbell.GetRightIpv4Address(0), 9));
    app1->Setup(ns3TcpSocket1, sinkAddress1, packetSize, 1e9, DataRate("1Gbps"));
    dumbbell.GetLeft(0)->AddApplication(app1);
    app1->SetStartTime(Seconds(1.0));
    app1->SetStopTime(Seconds(10.0));

    Ptr<Socket> ns3TcpSocket2 =
        Socket::CreateSocket(dumbbell.GetLeft(1), TcpSocketFactory::GetTypeId());
    ns3TcpSocket2->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChange2));

    Ptr<TutorialApp> app2 = CreateObject<TutorialApp>();
    Address sinkAddress2(InetSocketAddress(dumbbell.GetRightIpv4Address(1), 9));
    app2->Setup(ns3TcpSocket2, sinkAddress2, packetSize, 1e9, DataRate("1Gbps"));
    dumbbell.GetLeft(1)->AddApplication(app2);
    app2->SetStartTime(Seconds(1.0));
    app2->SetStopTime(Seconds(10.0));

    Simulator::Stop(Seconds(11.0));
    Simulator::Run();

    double flow1receivedPackets = 0;
    double flow2receivedPackets = 0;

    flowMonitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats();

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin();
         iter != stats.end();
         iter++)
    {
        //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);

        // std::cout << "Flow " << iter->first << " (" << t.sourceAddress << " -> "<<
        // t.destinationAddress << ")\n";
        if (iter->first % 2 != 0)
        {
            flow1receivedPackets += iter->second.rxPackets;
        }
        else
        {
            flow2receivedPackets += iter->second.rxPackets;
        }
    }

    Time now = Simulator::Now();

    double flow1Thoroughput = flow1receivedPackets * packetSize * 8 / (1e6 * now.GetSeconds());
    //NS_LOG_UNCOND(flow1Thoroughput << "\t" << bottleneckDataRate);
    *stream1->GetStream() << flow1Thoroughput << "\t" << bottleneckDataRate << "\t";

    double flow2Thoroughput = flow2receivedPackets * packetSize * 8 / (1e6 * now.GetSeconds());
    //NS_LOG_UNCOND(flow2Thoroughput << "\t" << bottleneckDataRate);
    *stream1->GetStream() << flow2Thoroughput << "\t" << bottleneckDataRate << std::endl;

    *stream2->GetStream() << flow1Thoroughput << "\t" << errorValue << "\t";

    *stream2->GetStream() << flow2Thoroughput << "\t" << errorValue << std::endl;

    double jainIndex =
        (flow1Thoroughput + flow2Thoroughput) * (flow1Thoroughput + flow2Thoroughput) /
        (2 * (flow1Thoroughput * flow1Thoroughput + flow2Thoroughput * flow2Thoroughput));
    //NS_LOG_UNCOND(jainIndex << "\t" << bottleneckDataRate);
    *stream4->GetStream() << jainIndex << "\t" << bottleneckDataRate << std::endl;

    *stream5->GetStream() << jainIndex << "\t" << errorValue << std::endl;

    Simulator::Destroy();

    return 0;
}
