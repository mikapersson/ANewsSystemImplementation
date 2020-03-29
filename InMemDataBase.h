#ifndef INMEMDATABASE_H
#define INMEMDATABASE_H

#include "dbInterface.h"
#include <map>

class InMemDatabase : public dbInterface{  //ska vi ärva såhär?
public:

  InMemDatabase();
  // en enda lång string, messagehandler måste omvandla den till bytes som går att skicka
  string listNewsGroups();

  string listArticles(string newsgroup);
  string listArticles(unsigned newsgroup);

  bool createNewsGroup(string name);

  bool deleteNewsGroup(string newsgroup);
  bool deleteNewsGroup(unsigned newsgroup);

private:
  unsigned NEWSGROUP_ID;
  std::map<int, Newsgroup> newsGroups;  
};




#endif
