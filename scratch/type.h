/// type.h 用于定义一些 Link-16 专有数据结构
#ifndef LINK_TYPE_H
#define LINK_TYPE_H

#include <bits/stdc++.h>
#include <cstring>

using std::string;

typedef signed char i8;
typedef unsigned char u8;

const int MAX_MESSAGE_LENGTH = 100;
const int J_MSG_LENGTH = 75;

class J_Msg {
    char        _source[MAX_MESSAGE_LENGTH];    // 报文发送方
    u8          _data[J_MSG_LENGTH];            // 报文内容

public:
    J_Msg();
    J_Msg(const string &source, const string &data);

    void set_data(string data);
    void set_source(string source);

    string data();
    string source();
};

/// 起始字(Initial)结构
class J_Init: public J_Msg {
    // 利用 Union 解包 Msg 的 data 字段数据
    typedef union {
        struct{
            u8 word_format[2];
            u8 label[5];
            u8 sub_label[3];
            u8 message_length[3];
            u8 message[57];
            u8 crc[5];
        };
        u8 data[75];
    } J_Init_Data;

public:
    J_Init(char *source, int label, int sub_label, int message_length);
};


/// 连续字(Continuous)结构
class J_Con: public J_Msg {

};

/// 拓展字(Extend)结构
class J_Ext: public J_Msg {

};

#endif //LINK_TYPE_H
