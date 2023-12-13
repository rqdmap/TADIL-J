/// Link-16 报文交互的有限状态机(finite-state machine, FSM)
#ifndef LINK_FSM_H
#define LINK_FSM_H

#include <vector>
#include "common.h"
#include "type.h"
using namespace ns3;
using std::vector;

const int MAX_LABEL_LENGTH = 100;

class State;
class Transition;
class FSM;

/// 状态转移条件
class Condition {

public:
    Condition();
    ~Condition();
};

/// 状态转移
class Transition {
    char                label[MAX_LABEL_LENGTH];
    Condition           condition;
    State               &to_state;

public:
    Transition();
    ~Transition();
};

/// 状态节点
class State {
    char                label[MAX_LABEL_LENGTH];    // 状态标签
    vector<Transition>  trans;                      // 状态转移
    void                (*on_enter)();              // 进入状态时的回调函数

public:
    State();
    ~State();
};

/// 有限状态机
class FSM {
    State               &current_state;
    vector<State>       states;

public:
    FSM();
    ~FSM();

    void init_fsm();
    void on_recv_packet(Ptr<Socket> socket);
};

#endif //LINK_FSM_H
