#include "socket.h"

/// 辅助函数, 将节点的 socket 设置为广播模式
Ptr<Socket>  make_broad_cast_socket(Ptr<Node> node){
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    InetSocketAddress remote = InetSocketAddress(Ipv4Address("255.255.255.255"), 80);

    Ptr<Socket> socket = Socket::CreateSocket(node, tid);
    socket->SetAllowBroadcast(true);
    socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), 80));
    socket->Connect(remote);
    return socket;
}


/// 构造函数
LinkSocket::LinkSocket() { }

LinkSocket::LinkSocket(Ptr<Node> node){
    this->node = node;
    this->socket = make_broad_cast_socket(node);
}

/// 绑定FSM
void LinkSocket::bind_fsm(FSM *fsm) {
    // 绑定回调函数, 当接收到数据包时, 调用FSM的状态转移函数
    this->fsm = fsm;
    // socket->SetRecvCallback(MakeCallback(&this->foo));
}

/// 发包函数
void LinkSocket::sendPacket(J_Msg *msg) {
    // 发送数据包
    Ptr<Packet> replyPacket = Create<Packet>((const uint8_t*)msg->data().c_str(), 75);
    socket->Send(replyPacket);

    // 回显日志
    std::string log_msg = std::string("> ") + msg->source() + " send packet: " + msg->data();
    NS_LOG_UNCOND(log_msg);
}


// J_Msg* recvPacket(Ptr<Socket> socket){
//     Ptr<Packet> packet;
//     Address from;
//
//     // while 还是 if? 是否在图上会有问题
//     // while ((packet = socket->RecvFrom(from)))
//     // {
//     //     uint8_t buffer[packet->GetSize()];
//     //     packet->CopyData(buffer, packet->GetSize());
//     //
//     //     std::string packetData(reinterpret_cast<char*>(buffer), packet->GetSize());
//     //     NS_LOG_UNCOND("< C Received packet: " << packetData);
//
// }
