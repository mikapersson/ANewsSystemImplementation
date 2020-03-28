#ifndef INMEMDATABASE_H
#define INMEMDATABASE_H

#include <string>
#include <vector>

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

class InMemDatabase {
public:

  InMemDatabase();
  // en enda lång string, messagehandler måste omvandla den till bytes som går att skicka
  string listNewsGroups();

  string listArticles(string newsgroup);
  string listArticles(unsigned newsgroup);

  bool createNewsGroup();

private:
  unsigned NEWSGROUP_ID;
  std::vector<Newsgroup> newsGroups;



};




#endif
