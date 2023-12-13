#include "common.h"
#include "socket.h"
using namespace ns3;


NS_LOG_COMPONENT_DEFINE("WifiSimpleAdhoc");


// // 接收方
// void
// RecverSendPacket(Ptr<Socket> socket)
// {
//     Ptr<Packet> packet;
//     Address from;
//     while ((packet = socket->RecvFrom(from)))
//     {
//         uint8_t buffer[packet->GetSize()];
//         packet->CopyData(buffer, packet->GetSize());
//
//         std::string packetData(reinterpret_cast<char*>(buffer), packet->GetSize());
//         NS_LOG_UNCOND("< C Received packet: " << packetData);
//
//         if(packetData.find("J12.0/NPG-9") != std::string::npos) {
//             std::string reply = "J12.6/NPG-9";
//             Ptr<Packet> replyPacket = Create<Packet>((const uint8_t*)reply.c_str(), reply.size());
//             socket->Send(replyPacket);
//             NS_LOG_UNCOND("> C Send packet: " << reply);
//         }
//         else {
//             NS_LOG_UNCOND("- C Ignore");
//         }
//     }
// }
//
// // 中间方
// void
// MidSendPacket(Ptr<Socket> socket)
// {
//     Ptr<Packet> packet;
//     Address from;
//     while ((packet = socket->RecvFrom(from)))
//     {
//         uint8_t buffer[packet->GetSize()];
//         packet->CopyData(buffer, packet->GetSize());
//
//         std::string packetData(reinterpret_cast<char*>(buffer), packet->GetSize());
//         NS_LOG_UNCOND("< B Received packet: " << packetData);
//
//
//         if(packetData.find("J9.0/NPG-8") != std::string::npos) {
//             std::string reply = "J12.0/NPG-9";
//             Ptr<Packet> replyPacket = Create<Packet>((const uint8_t*)reply.c_str(), reply.size());
//             socket->Send(replyPacket);
//             NS_LOG_UNCOND("> B Send packet: " << reply);
//         }
//         else {
//             NS_LOG_UNCOND("- B Ignore");
//         }
//     }
// }
//
// // 发送方
// void
// SenderSendPacket(Ptr<Socket> socket)
// {
//     Ptr<Packet> packet;
//     Address from;
//     while ((packet = socket->RecvFrom(from)))
//     {
//         uint8_t buffer[packet->GetSize()];
//         packet->CopyData(buffer, packet->GetSize());
//
//         std::string packetData(reinterpret_cast<char*>(buffer), packet->GetSize());
//         NS_LOG_UNCOND("< A Received packet: " << packetData);
//
//
//         if(packetData.find("J12.6/NPG-9") != std::string::npos) {
//             std::string reply = "J10.2/NPG-8";
//             Ptr<Packet> replyPacket = Create<Packet>((const uint8_t*)reply.c_str(), reply.size());
//             socket->Send(replyPacket);
//             NS_LOG_UNCOND("> A Send packet: " << reply);
//         }
//         else if(packetData.find("J13.2/NPG-6") != std::string::npos) {
//         NS_LOG_UNCOND("- A Finish");
//         }
//         else {
//             NS_LOG_UNCOND("- A Ignore");
//         }
//     }
// }
//
// static void
// GenerateTraffic(Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
// {
//     std::string customData = "J9.0/NPG-8";
//     Ptr<Packet> packet = Create<Packet>((uint8_t*)customData.c_str(), customData.size());
//     if (pktCount > 0) {
//         // socket->Send(Create<Packet>(pktSize));
//         socket->Send(packet);
//         NS_LOG_UNCOND("> A Send packet: " << customData);
//         Simulator::Schedule(pktInterval,
//                             &GenerateTraffic,
//                             socket,
//                             pktSize,
//                             pktCount - 1,
//                             pktInterval);
//     }
//     else
//     {
//         socket->Close();
//     }
// }

int main(int argc, char* argv[]) {
    // 指定无线物理层的模式
    std::string phyMode("DsssRate1Mbps");
    double rss = -80;           // -dBm
    uint32_t packetSize = 1000; // bytes
    uint32_t numPackets = 1;
    double interval = 1.0;      // 发送数据包的间隔时间
    bool verbose = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("phyMode", "Wifi Phy mode", phyMode);
    cmd.AddValue("rss", "received signal strength", rss);
    cmd.AddValue("packetSize", "size of application packet sent", packetSize);
    cmd.AddValue("numPackets", "number of packets generated", numPackets);
    cmd.AddValue("interval", "interval (seconds) between packets", interval);
    cmd.AddValue("verbose", "turn on all WifiNetDevice log components", verbose);
    cmd.Parse(argc, argv);

    Time interPacketInterval = Seconds(interval);

    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

    NodeContainer c;
    c.Create(3);

    // The below set of helpers will help us to put together the wifi NICs we want
    WifiHelper wifi;
    if (verbose)
    {
        WifiHelper::EnableLogComponents(); // Turn on all Wifi logging
    }

    // 802.11协议位于物理层与链路层
    wifi.SetStandard(WIFI_STANDARD_80211b);

    // 配置无线物理层的参数
    YansWifiPhyHelper wifiPhy;
    {
        // 接受过程不添加额外的增益
        wifiPhy.Set("RxGain", DoubleValue(0));
        // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
        wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);

        YansWifiChannelHelper wifiChannel;
        // 传播延迟模型为常速传播延迟模型
        wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
        // 这个模型会使得接收信号强度（rss）固定，不受两个站点之间的距离和发射功率的影响。
        wifiChannel.AddPropagationLoss("ns3::FixedRssLossModel", "Rss", DoubleValue(rss));

        // 将无线物理层与无线信道关联
        wifiPhy.SetChannel(wifiChannel.Create());
    }

    // 配置无线数据链路层的参数
    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    {
        // 设置远程站点控制器参数
        // 通过设置一致的数值禁用速率控制
        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     StringValue(phyMode),
                                     "ControlMode",
                                     StringValue(phyMode));

        // Set it to adhoc mode(自组织网络模式)
        wifiMac.SetType("ns3::AdhocWifiMac");
    }

    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, c);

    // 设置节点的移动性和位置
    {
        // Note that with FixedRssLossModel, the positions below are not
        // used for received signal strength.
        MobilityHelper mobility;
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        positionAlloc->Add(Vector(0.0, 0.0, 0.0));
        positionAlloc->Add(Vector(5.0, 0.0, 0.0));
        positionAlloc->Add(Vector(10.0, 0.0, 0.0));
        // 设置位置分配器
        mobility.SetPositionAllocator(positionAlloc);
        // 设置节点的移动模型为固定不变
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        // 将移动性配置应用到节点上
        mobility.Install(c);
    }

    // 实现 IP/TCP/UDP协议栈
    InternetStackHelper internet;
    internet.Install(c);

    {
        Ipv4AddressHelper ipv4;
        NS_LOG_INFO("Assign IP Addresses.");
        ipv4.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer i = ipv4.Assign(devices);
    }

    // 实现应用层应用

    // // 为c[0]创建一个socket对象
    // Ptr<Socket> recvSink = Socket::CreateSocket(c.Get(0), tid);
    // // 表示可以接受任何IP的80口的数据包
    // recvSink->SetAllowBroadcast(true);
    // recvSink->Bind(InetSocketAddress(Ipv4Address::GetAny(), 80));
    // recvSink->SetRecvCallback(MakeCallback(&RecverSendPacket));
    // recvSink->Connect(remote);
    //
    // Ptr<Socket> midSink = Socket::CreateSocket(c.Get(1), tid);
    // midSink->SetAllowBroadcast(true);
    // midSink->Bind(InetSocketAddress(Ipv4Address::GetAny(), 80));
    // midSink->SetRecvCallback(MakeCallback(&MidSendPacket));
    // midSink->Connect(remote);
    //
    // Ptr<Socket> source = Socket::CreateSocket(c.Get(2), tid);
    // source->SetAllowBroadcast(true);
    // source->Bind(InetSocketAddress(Ipv4Address::GetAny(), 80));
    // source->Connect(remote);
    
    LinkSocket link_socket[3];
    link_socket[0] = LinkSocket(c.Get(0));
    link_socket[1] = LinkSocket(c.Get(1));
    link_socket[2] = LinkSocket(c.Get(2));


    // Tracing
    // wifiPhy.EnablePcap("wifi-simple-adhoc", devices);

    // Output what we are doing
    // NS_LOG_UNCOND("Testing " << numPackets << " packets sent with receiver rss " << rss);

    // Simulator::ScheduleWithContext(source->GetNode()->GetId(),
    //                                Seconds(1.0),
    //                                &GenerateTraffic,
    //                                source,
    //                                packetSize,
    //                                numPackets,
    //                                interPacketInterval);

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

