#include "type.h" 

J_Msg::J_Msg() {
    memset(_source, 0, sizeof(_source));
    memset(_data, 0, sizeof(_data));
}

J_Msg::J_Msg(const string &source, const string &data) {
    if (source.length() > MAX_MESSAGE_LENGTH) {
        printf("source length is too long %ld\n", source.length());
        J_Msg();
    }
    else if (data.length() != J_MSG_LENGTH) {
        printf("data length is not %d\n", J_MSG_LENGTH);
        J_Msg();
    }
    else {
        strcpy(_source, source.c_str());
        memcpy(_data, data.c_str(), data.length());
    }
}

string J_Msg::data() {
    return string((char*)_data, J_MSG_LENGTH);
}

string J_Msg::source() {
    return string(_source);
}

// J_Init::J_Init(char *source, int label, int sub_label, int message_length) {
//     assert(label >= 0 && label < 32);
//     assert(sub_label >= 0 && sub_label < 8);
//     assert(message_length >= 0 && message_length < 8);
//
//     J_Init_Data init_data;
//     memcpy(init_data.word_format, "00", 2);
//     memcpy(init_data.label, (u8*)label, 5);
//     memcpy(init_data.sub_label, (u8*)sub_label, 3);
//     memcpy(init_data.message_length, (u8*)message_length, 3);
//
//     J_Msg(source, init_data.data);
// }
//
//
