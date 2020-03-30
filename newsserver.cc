/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "messagehandler.h"
#include "protocol.h"
//#include "VÅRA DATABASER"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using std::cerr;
using std::endl;

Server init(int argc, char* argv[]);
void listNG(Database& db, MessageHandler& mh);
void createNG(Database& db, MessageHandler& mh);
void deleteNG(Database& db, MessageHandler& mh);
void listArt(Database& db, MessageHandler& mh);
void createArt(Database& db, MessageHandler& mh);
void deleteArt(Database& db, MessageHandler& mh);
void getArt(Database& db, MessageHandler& mh);


int main(int argc, char* argv[]){
    Server server = init(argc, argv);



    return 0;
}



Server init(int argc, char* argv[]){
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]);
        } catch (std::exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

void listNG(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_LIST_NG);
}

void createNG(Database& db, MessageHandler& mh){

}

void deleteNG(Database& db, MessageHandler& mh){

}

void listArt(Database& db, MessageHandler& mh){

}

void createArt(Database& db, MessageHandler& mh){

}

void deleteArt(Database& db, MessageHandler& mh){

}

void getArt(Database& db, MessageHandler& mh){

}