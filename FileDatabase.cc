#include "FileDatabase.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
FileDatabase::FileDatabase(){


  struct dirent* ent;

  if((dir = opendir("./Database")) != nullptr){
    while((ent = readdir(dir)) != nullptr){
      //std::cout << "File: " << ent->d_name <<std::endl;
    }
  }else{
      std::cout << "No root directory found. Creating...\n";
      int status = mkdir("./Database",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      if(status == -1 ){
        std::cerr << "Unable to create root directory in database." << '\n';
        exit(1);
      }else{
        std::cout << "successfully created root directory" << std::endl;
      }
  }





}

FileDatabase::~FileDatabase(){
  //delete dir;
}
// Borde return vector med strings istället blir asjobbigt annars!
std::vector<Newsgroup> FileDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;

  struct dirent* ent;
  DIR* tmp;
  char d[256];
  char root[] = "./Database";
  // Det här funkar inte
  while((ent = readdir(dir)) != nullptr){
    d[0] = '\0';
    strcat(d,root);
    strcat(d, ent->d_name);
    std::cout << d << std::endl;
    if((tmp = opendir(d)) != nullptr){
        std::cout << "Found directory: " << ent->d_name;

    }
  }


  return std::vector<Newsgroup>();
}


bool FileDatabase::createNewsgroup(string name){
  return false;
}


bool FileDatabase::deleteNewsgroup(unsigned ng_ID){
  return false;
}


std::vector<Article> FileDatabase::listArticles(unsigned ng_ID){
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
