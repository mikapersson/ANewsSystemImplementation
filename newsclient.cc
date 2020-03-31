/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "MessageHandler.h"
#include "Protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <exception>
#include <vector>
#include <sstream>

std::string validCommands[] = {"read" , "list" , "delete", "create", "exit" , "help"};
std::string cts(char array[]){return std::string(array);}

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
bool validCommand(std::string command){
  for(auto& s : validCommands){
    if(s == command)
      return true;
  }
  return false;
}
void listArticles(MessageHandler& mh, int ID){
  std::vector<std::pair<int,std::string>> result;
  mh.send_anscode(Protocol::COM_LIST_ART);
  mh.send_int_parameter(ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // SHould be ANS_LIST_ART check?!
  Protocol r = mh.rec_cmd();
  if(r == Protocol::ANS_NAK){
    mh.rec_cmd(); // SHould e ERR_NG_DOES_NOT_EXIST check?!
    std::cout << "No newsgroup with ID:\t" << ID << std::endl;
  }else{
    int arts = mh.rec_int_parameter();
    int tempInt =0;
    std::string tempStr;
    for(int i = 0; i != arts; i++){
      tempInt = mh.rec_int_parameter();
      tempStr = mh.rec_string_parameter();
      result.push_back(std::make_pair(tempInt,tempStr));
    }
    std::cout << "Articles: ";
    if(arts == 0){
      std::cout <<"\tnone" << std::endl;
    }else{
      std::cout << "\n";
      for(auto& p: result)
        std::cout << "\t" << p.first << ". " << p.second << "\n";
    }
  }
  mh.rec_cmd(); // should be ANS_END

}

void listNewsgroups(MessageHandler& mh){
  std::vector<std::pair<int,std::string>> result;
  mh.send_anscode(Protocol::COM_LIST_NG);
  mh.send_anscode(Protocol::COM_END);
  mh.rec_cmd(); // should be ANS_LIST , check!!
  int ngs = mh.rec_int_parameter();
  std::string tempStr;
  int tempInt = 0;
  for(int i=0; i != ngs; i++){
    tempInt = mh.rec_int_parameter();
    tempStr = mh.rec_string_parameter();
    result.push_back(std::make_pair(tempInt,tempStr));
  }
  std::cout << "Available newsgroups:";
  if(ngs == 0)
    std::cout << "\tnone" <<std::endl;
  else
    std::cout << "\n";
  for(auto& p: result){
    std::cout << "\t" << p.first << ". " << p.second << "\n";
  }
  mh.rec_cmd(); // should be end Check?!?!

}
// get int number index in string p
int getIntParam(std::string p, int index){
  while(p[0] == ' ')
    p = p.substr(1,std::string::npos);
  std::stringstream ss(p);
  ss.seekg(0);
  std::string str;
  int t = 0;
  while(index != 0){
    if(ss.eof())
      throw std::invalid_argument("Index larger than arguments");
    ss>>str;
    t = std::stoi(str);
    index--;
  }
  return t;
}
void read(MessageHandler& mh, std::string parameters){
  int ng_ID = 0, art_ID = 0;


  try{
    ng_ID = getIntParam(parameters,1);
    art_ID = getIntParam(parameters,2);
  }catch(std::exception& e){
    std::cout <<
      "read [NG_ID] [ART_ID]\t- to read article ART_ID in newsgroup NG_ID." <<std::endl;
    return;
  }
  mh.send_anscode(Protocol::COM_GET_ART);
  mh.send_int_parameter(ng_ID);
  mh.send_int_parameter(art_ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // SHould be ANS_GET_ART check?
  Protocol reply = mh.rec_cmd();
  if(reply == Protocol::ANS_NAK){
    reply = mh.rec_cmd();
    if(reply == Protocol::ERR_NG_DOES_NOT_EXIST){
      std::cout << "No newsgroup with ID:\t" << ng_ID <<std::endl;

    }else{ // Article does not exist
      std::cout << "No article with ID:\t"  << ng_ID << ". In newsgroup:\t "<< ng_ID<< std::endl;

    }
    mh.rec_cmd(); // should be ANS_END
    return;
  }
  // Found article
  std::string title = mh.rec_string_parameter();
  std::string author = mh.rec_string_parameter();
  std::string text = mh.rec_string_parameter();
  mh.rec_cmd(); // should be ANS_END
  std::cout << "\t" << title << "\t|\tBy: " << author <<"\n" <<std::endl;
  std::cout << text <<std::endl;

}

// With no parametrs this functions lists newsgroups
// if provided with an index it will list articles in the newsgroup with that index
void list(MessageHandler &mh, std::string parameters){
  while(parameters[0] == ' ')
    parameters.substr(1,std::string::npos);
  if(parameters.size() == 0){
    listNewsgroups(mh);
    return;
  }
//  if(!parameters.find(" "))
  {
    try{
      int ng_ID = std::stoi(parameters);

      listArticles(mh, ng_ID);
      return;
    }catch(std::exception& e){


    }
  }
  std::cout <<
    "list\t\t- to display newsgroups\nlist [index]\t- to list articles in a specific newsgroup."
    << std::endl;
  return;
}

int main(int argc, char *argv[]){

  Connection conn = init(argc, argv);
  char cinput[256];
  MessageHandler mh(conn);

  bool running = true;
  std::string input;
  std::string command;
  std::string parameters;
  while(running){
    std::cout << ">>>"; // prompt
    std::cin.getline(cinput,256);
    input = cts(cinput);
    command = input.substr(0, input.find_first_of(' ',0));
    parameters = input.substr(input.find_first_of(' ',0) + 1,std::string::npos );
    if(parameters == command)
      parameters = "";
    if(!validCommand(command)){
      std::cout << "Invalid command, type \"help\" for info. " <<std::endl;
      continue;
    }
    //std::cout << command << ":" << parameters << std::endl; // only while writing program
    if(command == "read"){
      read(mh,parameters);
    }
    if(command == "list"){
      list(mh, parameters);
    }
    if(command == "delete"){

    }
    if(command == "create"){

    }
    if(command == "help"){

    }

    if(command == "exit"){
      running = false;
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
    // borde också omge hela skiten med en catch för socketclosedException...
  }
  std::cout << "Exiting newsclient.." << std::endl;

  exit(0);
}
