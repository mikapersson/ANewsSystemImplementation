#ifndef INMEMDATABASE_H
#define INMEMDATABASE_H
#include <unordered_map>
#include <vector>
#include <string>

using std::string;
using std::vector;


struct Article {
  string title;
  string author;
  unsigned article_ID;
  string text;
};

struct Newsgroup {
  string name;
  unsigned newsGroup_ID;
  unsigned article_IDs;
  std::unordered_map<unsigned,Article> articles;
};

class InMemDatabase {
public:

  InMemDatabase();

  ~InMemDatabase();

  // List newsGroups
  std::vector<Newsgroup> listNewsgroups();

  // Create newsGroup, newsgroup name as parameter
  //returns true if creation was successful, otherwise false
  // Should throw "ERR_NG_ALREADY_EXISTS"
  bool createNewsgroup(string name);  //ändrade till bool för enklare hantering i serverklassen

  // Delete newsgroup
  //  should throw "ERR_NG_DOES_NOT_EXIST"  //vi kastar väl inget här?
  bool deleteNewsgroup(unsigned ng_ID);

  // List articles in newsgroup
  // Should throw "ERR_NG_DOES_NOT_EXIST"
  std::vector<Article> listArticles(unsigned ng_ID);

  // create article in newsgroup
  // should throw "ERR_NG_DOES_NOT_EXIST"
  bool createArticle(unsigned ng_ID , string title, string author, string text);

  // Delete an article in newsgroup
  // Should throw "ERR_NG_DOES_NOT_EXIST" and "ERR_ART_DOES_NOT_EXIST"
  bool deleteArticle(unsigned ng_ID , unsigned art_ID);

  // Get an article in a newsGroup
  // Should throw "ERR_NG_DOES_NOT_EXIST" and "ERR_ART_DOES_NOT_EXIST"
  Article getArticle(unsigned ng_ID , unsigned art_ID);

private:
  unsigned NEWSGROUP_ID;
  std::unordered_map<unsigned,Newsgroup> newsgroups;


  // change to vector and add findartbyname and findartbyid
  // and findNgbyname and findNgByid ALTERNATIVT 'bool ngExists(unsigned ngID)'?
};




#endif
