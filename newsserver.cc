/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "messagehandler.h"
#include "protocol.h"
//#include "VÅRA DATABASER"
#include "InMemDataBase.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

using std::cerr;
using std::cout;
using std::endl;
using std::endl;

typedef InMemDatabase Database;  //osäker på om vi ska göra såhär

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
    //CREATE DATABASE HERE
    Database db;

    while(true){
        auto conn = server.waitForActivity();
        if(conn != nullptr){

            MessageHandler mh(conn);
            try{
                auto received = mh.rec_cmd();
                switch(received){
                    case Protocol::COM_LIST_NG : listNG(db, mh);
                    case Protocol::COM_CREATE_NG : createNG(db, mh);
                    case Protocol::COM_DELETE_NG : listNG(db, mh);
                    case Protocol::COM_LIST_ART : listNG(db, mh);
                    case Protocol::COM_CREATE_ART : listNG(db, mh);
                    case Protocol::COM_DELETE_ART : listNG(db, mh);
                    case Protocol::COM_GET_ART : listNG(db, mh);
                }
                mh.send_anscode(Protocol::ANS_END);
                if(mh.rec_cmd() != Protocol::COM_END){
                    cout << "No end of command received, terminating connection" << endl;
                    throw ConnectionClosedException();
                }

            } catch(ConnectionClosedException&) {
                server.deregisterConnection(conn);
                cout << "Closed connection exception" << endl;    
            }
        } else {
            conn = std::make_shared<Connection>();
            server.registerConnection(conn);
            cout << "New client connected" << endl;
        }
    }


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
    auto ngList = db.listNewsgroups();
    mh.send_int(ngList.size());
    for(auto& ng : ngList){
        mh.send_int_parameter(ng.newsGroup_ID);
        mh.send_string_parameter(ng.name);
    }
}

void createNG(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_CREATE_NG);
    if(db.createNewsgroup(mh.rec_string_parameter())){
        mh.send_anscode(Protocol::ANS_ACK);
    } else {
        mh.send_anscode(Protocol::ANS_NAK);
        mh.send_anscode(Protocol::ERR_NG_ALREADY_EXISTS);
    }
}

void deleteNG(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_DELETE_NG);
    if(db.deleteNewsgroup(mh.rec_int_parameter())){
        mh.send_anscode(Protocol::ANS_ACK);
    } else {
        mh.send_anscode(Protocol::ANS_NAK);
        mh.send_anscode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }
}

void listArt(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_LIST_ART);
    
    auto ngID = mh.rec_int_parameter();  //sent in newsgroup ID
    if(ANVÄND db.findByID() HÄR){  //check if the newsgroup exists
        auto articles = db.listArticles(ngID);  //get vector of articles corresponding to the newsgroup with ID ngID
        mh.send_anscode(Protocol::ANS_ACK);
        mh.send_int_parameter(articles.size());
        for(auto& a : articles){
            mh.send_int_parameter(a.article_ID);
            mh.send_string_parameter(a.title);
        }
    } else {
        mh.send_anscode(Protocol::ANS_NAK);
        mh.send_anscode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }

}

void createArt(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_CREATE_ART);
    auto grID = mh.rec_int_parameter();
    auto title = mh.rec_string_parameter();
    auto author = mh.rec_string_parameter();
    auto text = mh.rec_string_parameter();

    if(db.createArticle(grID, title, author, text)){  //if newsgroup exists
        mh.send_anscode(Protocol::ANS_ACK);
    } else {
        mh.send_anscode(Protocol::ANS_NAK);
        mh.send_anscode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }
}

void deleteArt(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_DELETE_ART);
    auto grID = mh.rec_int_parameter();
    auto artID = mh.rec_int_parameter();

    if(db.deleteArticle(grID, artID)){
        mh.send_anscode(Protocol::ANS_ACK);
    } else {
        if(ANVÄND db.findByID() HÄR){  //om gruppen inte finns
            mh.send_anscode(Protocol::ANS_NAK);
            mh.send_anscode(Protocol::ERR_NG_DOES_NOT_EXIST);
        } else {  //om nyheten inte finns
            mh.send_anscode(Protocol::ANS_NAK);
            mh.send_anscode(Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    }
}

void getArt(Database& db, MessageHandler& mh){
    mh.send_anscode(Protocol::ANS_GET_ART);
    auto grID = mh.rec_int_parameter();
    auto artID = mh.rec_int_parameter();

    if(ANVÄND db.findByID() HÄR){  
        if(ANVÄND db.artExists(artID) HÄR){
            mh.send_anscode(Protocol::ANS_ACK);
            auto article = db.getArticle(grID, artID);
            mh.send_string_parameter(article.title);
            mh.send_string_parameter(article.author);
            mh.send_string_parameter(article.text);

        } else {
            mh.send_anscode(Protocol::ANS_NAK);
            mh.send_anscode(Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    } else {
        mh.send_anscode(Protocol::ANS_NAK);
        mh.send_anscode(Protocol::ERR_NG_DOES_NOT_EXIST);
    }
}