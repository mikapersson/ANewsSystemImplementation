#include "FileDatabase.h"
#include <iostream>
#include <stdio.h>

#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

using std::ofstream;
using std::ifstream;

FileDatabase::FileDatabase(){

  DIR* dir = nullptr;
  struct dirent* ent;

  if((dir = opendir(root)) != nullptr){
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
        std::cout << "Successfully created root directory" << std::endl;
      }
  }

  closedir(dir);

  std::ofstream manifest("./Database/manifest" , std::ofstream::app); // list of Newsgroups

  manifest.close();
  NEWSGROUP_ID = 0;

}

FileDatabase::~FileDatabase(){

}
// Borde return vector med strings istället blir asjobbigt annars!
std::vector<Newsgroup> FileDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  //std::unordered_map<int,Article> tmp;
  std::vector<Article> tmpA;
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
  unsigned id;
  while(!manifest.eof()){
    manifest >> ngName;
    manifest >> id;
    ngs.push_back(Newsgroup{ngName, id , 0 , std::unordered_map<unsigned,Article>()});
  }

  for(auto& ng : ngs){
    tmpA = listArticles(ng.newsGroup_ID);
    std::for_each(tmpA.begin(),tmpA.end(),[&](const Article &a){
      ng.articles[a.article_ID] = a;
    });
  }
  return ngs;
}


bool FileDatabase::createNewsgroup(string name){

  ifstream in_manifest("./Database/manifest");
  if(!in_manifest){
    std::cout << "An error occurred in createNewsgroup while opening manifest." << std::endl;
    exit(1);
  }
  std::string tmpName, tmpId;

  while(!in_manifest.eof()){
    in_manifest >> tmpName;
    in_manifest >> tmpId;
    if(tmpName == name){ // Already exists
      return false;
    }
  }
  in_manifest.close();

  ofstream out_manifest("./Database/manifest", std::ofstream::ate);
  if(!out_manifest){
    std::cout << "An error occurred while opening manifest." << std::endl;
    exit(1);
  }

  NEWSGROUP_ID++; // Make sure always unique
  out_manifest << name << " " << NEWSGROUP_ID << "\n";
  out_manifest.close();
  char dirPath[512] = "./Database/";
  strcat(dirPath, name.c_str());
  int status = mkdir(dirPath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  if(status == -1){
    std::cout << "An error occurred while creating newsgroup directory" <<std::endl;
    exit(1);
  }


  return true;
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
  if(!manifest.good()){
    std::cout << "An error occurred when opening manifest file \n";
    exit(1);
  }
  std::string name;
  unsigned ID;
  while(!manifest.eof()){ // Detta inte testat för flera newsgroups
    manifest >> name;
    manifest >> ID;
    if(ID == ng_ID)
      break;
  }
  manifest.close();
  char dirLocation[512] = "./Database/\0";
  char filepath[512];
  strcat(dirLocation, name.c_str());
  dirLocation[strlen(dirLocation) + 1] = '/';

  if((dir = opendir(dirLocation)) == nullptr){
    std::cerr << "Error in list articles." << std::endl;
    exit(1);
  }



  std::string title,author,text,tmp;
  unsigned art_ID;
//  char c;

  while((ent = readdir(dir)) != nullptr){
    strcpy(filepath,dirLocation); // ./Database/[NewsgroupName]
    filepath[strlen(filepath) + 1] = '\0'; // ./Database/[newsgroupName] \0
    filepath[strlen(filepath)] = '/'; // ./Database/[NewsgroupName]/\0
    strcat(filepath, ent->d_name); // full article filepath

    struct stat s;
    if(stat(filepath, &s) == -1){
      std::cout << "An error occurred when reading stats about an article." << std::endl;
      exit(1);
    }
    if(!S_ISREG(s.st_mode)){ // file is not a regular file
        continue;
    }

    article.open(filepath);
    if(!article){
      std::cout << "Error opening article\n";
      exit(1);
    }
    text = "";
    std::getline(article, title);
    std::getline(article, author);
    article >> art_ID;

    while(article >> tmp){ // FUlt men funkar
      text += tmp;
      text += " ";
    }
    /*
    while(!article.eof()){
      article.get(c);
      text += std::string(&c);
    }*/

    arts.push_back(Article{title,author,art_ID,text});
    article.close();
  }

  return arts;
}

// false om inte NG finns
bool FileDatabase::createArticle(unsigned ng_ID , string title, string author, string text){

  // kolla om NG finns manifest


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
