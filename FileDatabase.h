#ifndef FILEDATABASE_H
#define FILEDATABASE_H
#include "Database.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


class FileDatabase : public Database {
public:
  FileDatabase();
  ~FileDatabase();

  // List newsGroups
  std::vector<Newsgroup> listNewsgroups();

  // Create newsGroup, newsgroup name as parameter
  //returns true if creation was successful, otherwise false
  // Should throw "ERR_NG_ALREADY_EXISTS"
  bool createNewsgroup(string name);

  // Delete newsgroup
  //  should throw "ERR_NG_DOES_NOT_EXIST"
  bool deleteNewsgroup(unsigned ng_ID);

  // List articles in newsgroup
  std::vector<Article> listArticles(unsigned ng_ID);

  // Create article in newsgroup ng_ID. If successfull returns true, if
  // no newsgroup with that ID present, returns false.
  bool createArticle(unsigned ng_ID , string title, string author, string text);

  // Delete an article in newsgroup
  bool deleteArticle(unsigned ng_ID , unsigned art_ID);

  // Get an article in a newsGroup
  // Should throw "ERR_NG_DOES_NOT_EXIST" and "ERR_ART_DOES_NOT_EXIST"
  Article getArticle(unsigned ng_ID , unsigned art_ID);

  // Kollar om newsgroup ng_ID finns
  bool ngExists(unsigned ng_ID);

  // kollar om artikeln art_ID finns i newgsgroupen ng_ID finns
  // Använd alltid ngExisits först innan denna!!!! så man kan skilja dem
  bool artExists(unsigned ng_ID, unsigned art_ID);


  void increaseArtCounter(unsigned ID);
  int removeNewsgroup(char path[]);
  // Helper functions

  // Make a newsgroup from a folder path


private:
  char root[11] = "./Database";
  unsigned NEWSGROUP_ID;

};




#endif
