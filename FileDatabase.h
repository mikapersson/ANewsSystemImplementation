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
  // returns true if creation was successful, otherwise false
  bool createNewsgroup(string name);

  // Delete newsgroup
  bool deleteNewsgroup(unsigned ng_ID);

  // List articles in newsgroup
  std::vector<Article> listArticles(unsigned ng_ID);

  // Create article in newsgroup ng_ID. If successfull returns true, if
  // no newsgroup with that ID present, returns false.
  bool createArticle(unsigned ng_ID , string title, string author, string text);

  // Delete an article in newsgroup
  bool deleteArticle(unsigned ng_ID , unsigned art_ID);

  // Get an article in a newsGroup
  Article getArticle(unsigned ng_ID , unsigned art_ID);

  // Checks if the newsgroup with ID ng_ID exists
  bool ngExists(unsigned ng_ID);

  // Checks if the article with ID art_ID
  //  in newsgroup with ID ng_ID exists
  bool artExists(unsigned ng_ID, unsigned art_ID);

  // Helper functions

  // Increases the unique article counter
  // in the manifest file by one
  void increaseArtCounter(unsigned ID);

  // Removes the newsgroupfolder and all its contents
  int removeNewsgroup(char path[]);

private:
  std::string tmpNgName; // is set by ngExists function
  char root[11] = "./Database";
  char manifestPath[20] = "./Database/manifest";
  unsigned NEWSGROUP_ID;
};




#endif
