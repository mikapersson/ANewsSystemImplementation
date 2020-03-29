#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <string>
#include <vector>
#include <map>

using std::string;

struct Article {
  string title;
  string author;
  unsigned ID;
  string text;
};

struct Newsgroup {
  string Name;
  unsigned NewsGroup_ID;
  unsigned Article_IDs;
  std::vector<Article> articles;
};

class dbInterface{  //Interface för databaserna
        //virtuella funktioner..


};

#endif