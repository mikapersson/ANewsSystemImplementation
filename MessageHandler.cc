#include "messagehandler.h"

/*Create MessageHandler*/
MessageHandler::MessageHandler(std::shared_ptr<Connection> &c) : conn(c){}

/*send byte from server to client*/
void MessageHandler::send_byte(const char code){  
    conn->write(code);
}

/*send answer code from server to client*/
void MessageHandler::send_anscode(Protocol code){  
    send_byte(static_cast<unsigned char>(code));
}

/*send int (four bytes) from server to client*/
void MessageHandler::send_int(int val){  
    send_byte((val >> 24) & 0xFF);
    send_byte((val >> 16) & 0xFF);
    send_byte((val >> 8) & 0xFF);
    send_byte(val & 0xFF);
}

/*send int parameter from server to client*/
void MessageHandler::send_int_parameter(int i_par){  
    send_anscode(Protocol::PAR_NUM);
    send_int(i_par);
}

/*send string parameter from server to client*/
void MessageHandler::send_string_parameter(std::string s_par){  
    send_anscode(Protocol::PAR_STRING);
    send_int(s_par.size());
    for(auto c : s_par){
        send_byte(c);
    }
}

/*read byte from client to server*/
unsigned char MessageHandler::rec_byte(){  
    return conn->read();
}

/*read command from client to server*/
Protocol MessageHandler::rec_cmd(){  
    unsigned char c = rec_byte();
    return static_cast<Protocol>(c);
}

/*read int from client to server*/
int MessageHandler::rec_int(){  
    unsigned char b1 = conn->read();
    unsigned char b2 = conn->read();
    unsigned char b3 = conn->read();
    unsigned char b4 = conn->read();
    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

int MessageHandler::rec_int_parameter(){
    if(rec_cmd() != Protocol::PAR_NUM){
        throw ConnectionClosedException();
    }
    return rec_int();
}

std::string MessageHandler::rec_string_parameter(){
    if(rec_cmd() != Protocol::PAR_STRING){
        throw ConnectionClosedException();
    }
    int chars = rec_int();
    std::string res;
    for(int i = 0; i != chars; ++i){
        res.push_back(conn->read());
    }
    return res;
}
