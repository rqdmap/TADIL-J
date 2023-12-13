/// 封装节点的 socket 操作
#ifndef LINK_SOCKET_H
#define LINK_SOCKET_H

#include "common.h"
#include "type.h"
#include "fsm.h"
using namespace ns3;


class LinkSocket {
    Ptr<Node>           node;
    Ptr<Socket>         socket;
    FSM                 *fsm;

public:
    LinkSocket();
    LinkSocket(Ptr<Node>);

    // 基于FSM的状态绑定回调函数
    void bind_fsm(FSM *fsm);
    void foo(Ptr<Socket> socket);

    // 发送报文
    void sendPacket(J_Msg *msg);
};


J_Msg* recvPacket(Ptr<Socket> socket);

#endif //LINK_SOCKET_H
