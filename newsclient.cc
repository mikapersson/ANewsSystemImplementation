/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "MessageHandler.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <exception>


std::string validCommands[] = {"read" , "list" , "delete", "create", "exit" , "help"};


Connection init(int argc, char *argv[]){

  if(argc != 3){
    std::cout << "Usage: ./newsclient server port " << std::endl;
    exit(1);
  }

  // Check if server is in correct format ?? ipadress or "localhost"

  int port = -1;
  try{
    port = std::stoi(argv[2]);
  }catch(std::exception &e){
    std::cout << "Invalid port number format: " << e.what() << std::endl;
    exit(2);
  }
  if(0 > port || 65535 < port) {
    std::cout << "Invalid port number. Valid values: 0 < port < 65535" << std::endl;
    exit(3);
  }
  Connection c = Connection(argv[1], port);

  if(!c.isConnected()){
    std::cout << "Unable to make connection..." << std::endl;
    exit(5);
  }

  return c;

}

// Only checks if first word is in the validCommands list;
bool validCommand(std::string input){
  std::string str = input.substr(0, input.find_first_of(' ',0));
  for(auto& s : validCommands){
    if(s == str)
      return true;
  }
  return false;
}

std::string cts(char array[]){return std::string(array);}

int main(int argc, char *argv[]){

  Connection conn = init(argc, argv);
  std::shared_ptr<Connection> c(&conn);
  char cinput[256];
  MessageHandler mh(c);
  bool running = true;
  std::string input;

  while(running){
    std::cout << ">>>"; // prompt
    std::cin.getline(cinput,256);
    input = cts(cinput);// var det den här eller den som tar en stream man ska använda??
    if(!validCommand(input)){
      std::cout << "Invalid command, type \"help\" for info. " <<std::endl;
      continue;
    }
    // Kolla vilket command
    // sen en switch eller flera ifsatser somgör olika beroende på command
    // command == exit ska sätta running till false
    // create ska leda till två undergrupper, ena create Article och andra create newsGroup
    // samma sak med list, bara list ger alla newsgroups men list 1 ger artiklar i newsgroup 1
    // och såklart samma med delete
    // skriver man bara typ create ska det dyka upp en hjälptext hur man använder create
    // och help ska förklara hur man använder programmet och de olika kommandona
    // kanske en ny klass som hanterar de olika kommandon eller så blir de många funktioner
    // i den här filen. Aja det får vi hantera imorgon.
  }

  return 0;
}
