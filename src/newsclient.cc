/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "MessageHandler.h"
#include "Protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <exception>
#include <vector>
#include <stdio.h>
#include <sstream>

using std::string;
using std::cout;
using std::endl;

string validCommands[] = {"read" , "list" , "delete", "create", "exit" , "help", "clear"};


Connection* init(int argc, char *argv[]){

  if(argc != 3){
    cout << "Usage: ./newsclient server port " << std::endl;
    exit(1);
  }
  // Is the servername on the correct form? Check?

  int port = -1;
  try{
    port = std::stoi(argv[2]);
  }catch(std::exception &e){
    cout << "Invalid port number format:\t" << e.what() << std::endl;
    exit(2);
  }
  if(0 > port || 65535 < port) {
    cout << "Invalid port number. Valid values: 0 < port < 65535" << std::endl;
    exit(3);
  }
  Connection* c = new Connection(argv[1], port);

  if(!c->isConnected()){
    cout << "Unable to make connection..." << std::endl;
    exit(5);
  }

  return c;

}
void clearScreen(){for(unsigned i=0; i != 50; i++)cout << "\n";}

void printWelcomeMessage(){
  clearScreen();
  cout << "Welcome to NewsClient!\n" << "All is set and you are connected"
            << " to the newsserver." << "\nType \"help\" to list available commands. "
            << std::endl;
}

// Only checks if first word is in the validCommands list
bool validCommand(string command){
  for(auto& s : validCommands){
    if(s == command)
      return true;
  }
  return false;
}
// From main -> list -> listArticles
void listArticles(MessageHandler& mh, int ID){
  std::vector<std::pair<int,string>> result;
  mh.send_anscode(Protocol::COM_LIST_ART);
  mh.send_int_parameter(ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // SHould be ANS_LIST_ART check?
  Protocol r = mh.rec_cmd();
  if(r == Protocol::ANS_NAK){
    mh.rec_cmd(); // Should be ERR_NG_DOES_NOT_EXIST check?
    cout << "No newsgroup with ID:\t" << ID << std::endl;
  }else{
    int arts = mh.rec_int_parameter();
    int tempInt =0;
    string tempStr;
    for(int i = 0; i != arts; i++){
      tempInt = mh.rec_int_parameter();
      tempStr = mh.rec_string_parameter();
      result.push_back(std::make_pair(tempInt,tempStr));
    }
    cout << "Articles: ";
    if(arts == 0){
      cout <<"\tnone" << std::endl;
    }else{
      cout << "\n";
      for(auto& p: result)
        cout << "\t" << p.first << ". " << p.second << "\n";
    }
  }
  mh.rec_cmd(); // should be ANS_END

}

// from main -> list -> listNewsgroups
void listNewsgroups(MessageHandler& mh){
  std::vector<std::pair<int,string>> result;
  mh.send_anscode(Protocol::COM_LIST_NG);
  mh.send_anscode(Protocol::COM_END);
  mh.rec_cmd(); // should be ANS_LIST check?
  int ngs = mh.rec_int_parameter();
  string tempStr;
  int tempInt = 0;
  for(int i=0; i != ngs; i++){
    tempInt = mh.rec_int_parameter();
    tempStr = mh.rec_string_parameter();
    result.push_back(std::make_pair(tempInt,tempStr));
  }
  cout << "Available newsgroups:";
  if(ngs == 0)
    cout << "\tnone." <<std::endl;
  else
    cout << "\n";
  for(auto& p: result){
    cout << "\t" << p.first << ". " << p.second << "\n";
  }
  mh.rec_cmd();

}
// get the index:th int from the string p,
int getIntParam(string p, int index){
  while(p[0] == ' ')
    p = p.substr(1,string::npos);
  std::stringstream ss(p);
  ss.seekg(0);
  string str;
  int t = 0;
  while(index != 0){
    if(ss.eof())
      throw std::invalid_argument("Index larger than arguments"); // Correct int not encountered
    ss>>str;
    t = std::stoi(str);
    index--;
  }
  return t;
}
// displays an article in the database
// from main -> read
void read(MessageHandler& mh, string parameters){
  int ng_ID = 0, art_ID = 0;
  try{
    ng_ID = getIntParam(parameters,1);
    art_ID = getIntParam(parameters,2);
  }catch(std::exception& e){
    cout <<
      "read NG_ID ART_ID\t\t\t- to read article ART_ID in newsgroup NG_ID." <<std::endl;
    return;
  }
  mh.send_anscode(Protocol::COM_GET_ART);
  mh.send_int_parameter(ng_ID);
  mh.send_int_parameter(art_ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd();
  Protocol reply = mh.rec_cmd();
  if(reply == Protocol::ANS_NAK){
    reply = mh.rec_cmd();
    if(reply == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "No newsgroup with ID:\t" << ng_ID <<std::endl;

    }else{ // Article does not exist
      cout << "No article with ID:\t"  << ng_ID << ". In newsgroup:\t "<< ng_ID<< std::endl;

    }
    mh.rec_cmd(); // ANS_END
    return;
  }
  // Found article
  string title = mh.rec_string_parameter();
  string author = mh.rec_string_parameter();
  string text = mh.rec_string_parameter();
  mh.rec_cmd();// ANS_END
  cout << "\t" << title << "\t|\tBy: " << author <<"\n" <<std::endl;
  cout << text <<std::endl;
}

// With no parametrs this functions lists newsgroups
// if provided with an index it will list articles in the newsgroup with that index
// from main -> list
void list(MessageHandler &mh, string parameters){
  while(parameters[0] == ' ')
    parameters.substr(1,string::npos);
  if(parameters.size() == 0){
    listNewsgroups(mh);
    return;
  }
  try{
    int ng_ID = std::stoi(parameters);
    listArticles(mh, ng_ID);
    return;
  }catch(std::exception& e){}

  cout <<
    "list\t\t\t\t\t- to display newsgroups\nlist ng_ID\t\t\t\t- to list articles in a specific newsgroup."
    << std::endl;
  return;
}
void printHelpMessage(){
  cout << "Available commands: \n"
            << "list\t\t\t\t\t- list newsgroups.\n"
            << "list ng_ID\t\t\t\t- list articles in a specific newsgroup.\n"
            << "read ng_ID art_ID\t\t\t- read an article.\n"
            << "create \"title\"\t\t\t\t- create a newsgroup.\n"
            << "create ng_ID\t\t\t\t- create an article in the newsgroup ng_ID.\n"
            //<< "create ng_ID \"title\" \"author\" \"text\"\t- create an article.\n"
            << "delete ng_ID\t\t\t\t- delete a newsgroup.\n"
            << "delete ng_ID art_ID\t\t\t- delete an article.\n"
            << "clear\t\t\t\t\t- clear screen.\n"
            << "exit\t\t\t\t\t- exit NewsClient."
            << std::endl;
}
// from main -> create -> createNg
void createNg(MessageHandler& mh, string parameters){
  unsigned q = 0;
  for(char& c: parameters)
    if(c == '"')
      q++;
  if(q != 2){
    cout << "create \"title\"\t\t\t\t- create Newsgroup." << std::endl;
    return;
  }
  size_t p1,p2;
  p1 = parameters.find('"',0);
  p2 = parameters.find('"',p1+1);
  string title = parameters.substr(p1+1,p2-p1 - 1);

  mh.send_anscode(Protocol::COM_CREATE_NG);
  mh.send_string_parameter(title);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // ANS_CREATE_NG
  Protocol r = mh.rec_cmd();
  if(r == Protocol::ANS_NAK){
    mh.rec_cmd(); // ERR_NG_ALREADY_EXISTS
    cout << "A newsgroup named " << title << " already exists." <<std::endl;
  }else{ // ANS_ACK
    cout << "Newsgroup created." << std::endl;
  }
  mh.rec_cmd(); // ANS_END
}

void createArt(MessageHandler &mh, int ng_ID){
  string title;
  string author;
  string text;
  string temp_line;

  mh.send_anscode(Protocol::COM_CREATE_ART);
  mh.send_int_parameter(ng_ID);
  cout << "You are now creating an article, please provide the following:" << endl;

  cout << "Title: ";
  std::getline(std::cin, title);
  mh.send_string_parameter(title);

  cout << "Author: ";
  std::getline(std::cin, author);
  mh.send_string_parameter(author);

  cout << "Article Text (finish with '~'): ";
  while(std::getline(std::cin, temp_line)){
    auto end = temp_line.back();
    if(end == '~'){
      temp_line = temp_line.substr(0, temp_line.size()-1);
      text.append(temp_line);
      break;
    } else {
      text.append(temp_line + '\n');
    }
  }

  mh.send_string_parameter(text);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // ANS_CREATE_ART
  Protocol p = mh.rec_cmd();

  if(p == Protocol::ANS_NAK){
    mh.rec_cmd(); // ERR_NG_DOES_NOT_EXIST
    cout << "No newsgroup with ID:\t" << ng_ID << "."<< std::endl;
  }else{
    cout << "Article created." <<std::endl;
  }

  mh.rec_cmd(); // ANS_END


}

void create(MessageHandler &mh, string parameters){
  while(parameters[0] == ' ')
    parameters.substr(1,string::npos);
  try{
    int ng_ID = stoi(parameters); // if able to find a number, create art otherwise create ng
    parameters = parameters.substr(parameters.find_first_of(" ")+1,string::npos);
    createArt(mh, ng_ID);
  }catch(std::invalid_argument& e){
    createNg(mh,parameters);
  }
}
// from main -> deleteC -> deleteArt
void deleteArt(MessageHandler& mh, int ng_ID, int art_ID){
  mh.send_anscode(Protocol::COM_DELETE_ART);
  mh.send_int_parameter(ng_ID);
  mh.send_int_parameter(art_ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // ANS_DELETE_ART
  Protocol r = mh.rec_cmd();
  if(r == Protocol::ANS_NAK){
    Protocol r = mh.rec_cmd();
    if(r == Protocol::ERR_NG_DOES_NOT_EXIST){
      cout << "Newsgroup does not exist." <<std::endl;
    }else{ // ERR_ART_DOES_NOT_EXIST
      cout << "Article does not exist." <<std::endl;
    }
  }else{ // ANS_ACK
    cout << "Article deleted." << std::endl;
  }

  mh.rec_cmd(); // ANS_END
}
// from main ->deleteC -> deleteNG
void deleteNG(MessageHandler& mh, int ng_ID){
  mh.send_anscode(Protocol::COM_DELETE_NG);
  mh.send_int_parameter(ng_ID);
  mh.send_anscode(Protocol::COM_END);

  mh.rec_cmd(); // ANS_DELETE_NG

  Protocol r = mh.rec_cmd();
  if(r == Protocol::ANS_NAK){
    mh.rec_cmd(); // ERR_NG_DOES_NOT_EXIST
    cout << "Newsgroup does not exist." <<std::endl;
  }else{ // ANS_ACK
    cout << "Newsgroup deleted." << std::endl;
  }
  mh.rec_cmd(); // ANS_END
}

void deleteC(MessageHandler &mh, string parameters){
  while(parameters[0] == ' ')
    parameters.substr(1,string::npos);
  int ng_ID = -1, art_ID = -1;
  try{
    ng_ID = getIntParam(parameters,1); // if unable to find int parameter
    art_ID = getIntParam(parameters,2); // getIntParam throws invalid_argument
    deleteArt(mh, ng_ID, art_ID);
  }catch(std::invalid_argument& e){
    if(ng_ID == -1){
      cout << "delete ng_ID\t\t\t\t- delete newsgroup." << std::endl
                << "delete ng_ID art_ID\t\t\t- delete article in newsgroup." <<std::endl;
      return;
    }
    deleteNG(mh,ng_ID);
  }

}


int main(int argc, char *argv[]){

  Connection* conn = init(argc, argv);

  MessageHandler mh(*conn);

  bool running = true;
  string input;
  string command;
  string parameters;
  printWelcomeMessage();
  while(running){
    cout << ">>>"; // prompt

    std::getline(std::cin , input);

    if(std::cin.fail()){
      cout << "Input failed!" << std::endl;
      std::cin.clear();
      // std::cin.ignore(100,'\n');
      continue;
    }


    // The first word is considered the command.
    command = input.substr(0, input.find_first_of(' ',0));
    // Only send parameters of the command to respective function
    parameters = input.substr(input.find_first_of(' ',0) + 1,string::npos);
    if(parameters == command) // If only one word -> no parameters
      parameters = "";
    if(!validCommand(command)){
      cout << "Invalid command, type \"help\" for available instructions. " <<std::endl;
      continue;
    }
    try{
      if(command == "read"){
        read(mh,parameters);
      }
      if(command == "list"){
        list(mh, parameters);
      }
      if(command == "delete"){
        deleteC(mh,parameters);
      }
      if(command == "create"){
        create(mh,parameters);
      }
      if(command == "help"){
        printHelpMessage();
      }
      if(command == "clear")
        clearScreen();
      if(command == "exit"){
        running = false;
      }
  }catch(ConnectionClosedException& e){
    cout << "Unexpected server reply. Exiting..." << std::endl;
    running = false;
  }

  }
  cout << "Exiting newsclient.." << std::endl;

  return 0;
}
