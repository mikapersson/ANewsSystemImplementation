#include "MessageHandler.h"
#include "connection.h"
#include "connectionclosedexception.h"

#include "Protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <exception>
#include <vector>
#include<stdlib.h>
#include <sstream>
using std::cout;

Connection init(int argc, char *argv[]){
  if(argc != 3){
    cout << "Usage: ./newsclient server port " << std::endl;
    exit(1);
  }
  int port = -1;
  try{
    port = std::stoi(argv[2]);
  }catch(std::exception &e){
    cout << "Invalid port number format: " << e.what() << std::endl;
    exit(2);
  }
  if(0 > port || 65535 < port) {
    cout << "Invalid port number. Valid values: 0 < port < 65535" << std::endl;
    exit(3);
  }
  Connection c = Connection(argv[1], port);

  if(!c.isConnected()){
    cout << "Unable to make connection..." << std::endl;
    exit(5);
  }

  return c;

}



int main(int argc, char* argv[]){


  Connection conn = init(argc,argv);
  MessageHandler mh(conn);
  int n =0;
  std::string input;
  system("sleep 3");
  //while(std::cin >> input){


    mh.send_anscode(Protocol::COM_DELETE_ART);
    system("sleep 3");
    mh.send_string_parameter("hello");
    mh.send_anscode(Protocol::COM_END);
    /*
    if(!conn.isConnected())
      break;*/
  //}

}
