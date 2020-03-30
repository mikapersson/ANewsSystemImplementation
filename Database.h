#ifndef DATABASE_H
#define DATABASE_H
#include <unordered_map>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include "InMemDatabase.h"
//#include "FileDatabase.h" // Inte gjord än

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

class Database {
public:
  //virtual Database();
  // List newsGroups
  virtual std::vector<Newsgroup> listNewsgroups() = 0;

  // Create newsGroup, newsgroup name as parameter
  //returns true if creation was successful, otherwise false
  // Should throw "ERR_NG_ALREADY_EXISTS"
  virtual bool createNewsgroup(string name) = 0;

  // Delete newsgroup
  //  should throw "ERR_NG_DOES_NOT_EXIST"  //vi kastar väl inget här?
  virtual bool deleteNewsgroup(unsigned ng_ID) = 0;

  // List articles in newsgroup
  // Should throw "ERR_NG_DOES_NOT_EXIST"
  virtual std::vector<Article> listArticles(unsigned ng_ID) = 0;

  virtual bool createArticle(unsigned ng_ID , string title, string author, string text)=0;

  // Delete an article in newsgroup
  // ska ändras till void eftersom den utgår från att både newsgroupen och artiklen finns
  virtual bool deleteArticle(unsigned ng_ID , unsigned art_ID) = 0;

  // Get an article in a newsGroup
  // Should throw "ERR_NG_DOES_NOT_EXIST" and "ERR_ART_DOES_NOT_EXIST"
  virtual Article getArticle(unsigned ng_ID , unsigned art_ID) = 0;

  // Kollar om newsgroup ng_ID finns
  virtual bool ngExists(unsigned ng_ID) = 0;

  // kollar om artikeln art_ID finns i newgsgroupen ng_ID finns
  // Använd alltid ngExisits först innan denna!!!! så man kan skilja dem
  virtual bool artExists(unsigned ng_ID, unsigned art_ID) = 0;



};



#endif
