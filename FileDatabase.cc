#include "FileDatabase.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

using std::ofstream;
using std::ifstream;

FileDatabase::FileDatabase(){

  DIR* dir = nullptr;
  struct dirent* ent;

  if((dir = opendir(root)) != nullptr){
    while((ent = readdir(dir)) != nullptr){
      std::cout << "File: " << ent->d_name <<std::endl;
    }
  }else{
      std::cout << "No root directory found. Creating...\n";
      int status = mkdir("./Database",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      if(status == -1 ){
        std::cerr << "Unable to create root directory in database." << '\n';
        exit(1);
      }else{
        std::cout << "Successfully created root directory" << std::endl;
      }
  }

  closedir(dir);

  ofstream manifest("./Database/manifest"); // list of Newsgroups

  manifest.close();


}

FileDatabase::~FileDatabase(){
  //delete dir;
}
// Borde return vector med strings istället blir asjobbigt annars!
std::vector<Newsgroup> FileDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  unsigned ID = 0;
  struct dirent* ent;
  DIR* tmp = nullptr, *tmp2 = nullptr;

  if((tmp = opendir(root)) == nullptr){
    std::cerr << "Error while opening root directory"  << std::endl;
    exit(1);
  }
  char source[100];


  // Gör om lägg en fil "manifest" längst upp i Database som innehålller info om ngs
  ifstream manifest("./Database/manifest");
  if(!manifest.good()){
    std::cerr << "Error opening manifest file!" << std::endl;
    exit(1);
  }
  std::string ngName;
  int id;
  while(!manifest.eof()){
    manifest >> ngName;
    manifest >> id;
    ngs.push_back(Newsgroup(ngName, id));
  }

  for(auto& ng : ngs){
    ng.articles = listArticles(ng.id);
  }
  return ng;
}


bool FileDatabase::createNewsgroup(string name){
  return false;
}


bool FileDatabase::deleteNewsgroup(unsigned ng_ID){
  return false;
}

// Presumes the newsgroup with ng_ID exists
std::vector<Article> FileDatabase::listArticles(unsigned ng_ID){
  std::vector<Article> arts;
  DIR *dir = nullptr;
  struct dirent* ent = nullptr;

  ifstream article;

  ifstream manifest("./Database/manifest");
  std::string name;
  int t;
  while(!manifest.eof()){
    manifest >> name;
    manifest >> t;
    if(t == ng_ID)
      break;
  }
  manifest.close();
  char dirLocation[512] = "./Database/\0";
  char filepath[512];
  strcat(dirLocation, &s[0]);


  if((dir = opendir(dirLocation)) == nullptr){
    std::cerr << "Error in list articles." << std::endl;
    exit(1);
  }
  strcat(dirLocation,'/');
  strcpy(filepath, dirLocation);
  std::string title,author,text;

  while((ent = readdir(dir)) != nullptr){
    strcpy(filepath,dirLocation);
    strcat(filepath, ent->d_name);
    article.open(filepath);

    


    article.close();
  }



  return std::vector<Article>();
}


bool FileDatabase::createArticle(unsigned ng_ID , string title, string author, string text){
  return false;
}


bool FileDatabase::deleteArticle(unsigned ng_ID , unsigned art_ID){
  return false;
}

Article FileDatabase::getArticle(unsigned ng_ID , unsigned art_ID){
  return Article();
}


bool FileDatabase::ngExists(unsigned ng_ID){
  return false;
}

bool FileDatabase::artExists(unsigned ng_ID, unsigned art_ID){
  return false;
}
