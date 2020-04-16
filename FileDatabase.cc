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

#include <cstring>
#include <cerrno>
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
  // Kna inte ha en variable med newsgroup id måste läsa in antal rader i manifest
  NEWSGROUP_ID = 0;

}

FileDatabase::~FileDatabase(){

}
// Borde return vector med strings istället blir asjobbigt annars!
std::vector<Newsgroup> FileDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  std::vector<Article> tmpA;


  DIR* tmp = nullptr;

  if((tmp = opendir(root)) == nullptr){
    std::cerr << "Error while opening root directory"  << std::endl;
    exit(1);
  }

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
    std::cout << "Unable to open manifest file in CreateNewsgroup." << std::endl;
    exit(1);
  }
  std::string tmpName, tmpId, artCount;

  while(!in_manifest.eof()){
    in_manifest >> tmpName;
    in_manifest >> tmpId;
    in_manifest >> artCount;
    if(tmpName == name){ // Already exists
      return false;
    }
  }
  in_manifest.close();

  ofstream out_manifest("./Database/manifest", std::ofstream::app);
  if(!out_manifest){
    std::cout << "An error occurred while opening manifest." << std::endl;
    exit(1);
  }

  NEWSGROUP_ID++;
  // Structure of manifest file
  // Newsgroup name newsgroup ID then article counter

  out_manifest << name << " " << NEWSGROUP_ID <<  " 0" << "\n";
  out_manifest.close();

  struct stat sb;
  char dirPath[512] = "./Database/";
  strcat(dirPath, name.c_str());

  if(stat(dirPath, &sb) == -1){
    std::cout << std::strerror(errno) << std::endl;
  }

  if(!S_ISDIR(sb.st_mode)){
    int status = mkdir(dirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == -1){
      std::cout << "An error occurred while creating newsgroup directory." <<std::endl;
      exit(1);
    }
}

  return true;
}


bool FileDatabase::deleteNewsgroup(unsigned ng_ID){


  std::fstream manifest("./Database/manifest");

  if(!manifest){
    // something wrong
  }
  manifest.seekg(0,manifest.end);
  int filelength = manifest.tellg();
  char* contents = new char[filelength];
  manifest.seekg(0,manifest.beg);


  unsigned tmpID, tmpArtCounter;
  std::string tmpName,name;
  unsigned line = 0;
  // check manifest for newsgroup id
  while(!manifest.eof()){
    manifest >> tmpName;
    manifest >> tmpID;
    manifest >> tmpArtCounter;

    if(tmpID == ng_ID){
      break;
    }else{
      line++;
    }
  }
  if(tmpID != ng_ID){ // Newsgroup not found
    return false;
  }

  name = tmpName;

  manifest.read(contents, filelength);
  manifest.clear();
  manifest.seekg(0);

  for(unsigned i=0; i != line;i++){
    manifest >> tmpName;
    manifest >> tmpID;
    manifest >> tmpArtCounter;
  }
  manifest.write(contents, filelength);



  // remove newsgroup line from manifest
  char path[512] = "./Database/";
  strcat(path, name.c_str());
  int status = remove(path);

  std::cout << "Status of deletion operation: " << status << std::endl;


  // remove folder
  delete[] contents;
  return true;
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
  unsigned ID,artID;
  while(!manifest.eof()){ // Detta inte testat för flera newsgroups
    manifest >> name;
    manifest >> ID;
    manifest >> artID;
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

// Creates a new article in the ng_ID newsgroup
// Returns false if the newsgroup does not exist
bool FileDatabase::createArticle(unsigned ng_ID , string title, string author, string text){

  std::ifstream in_manifest("./Database/manifest");
  if(!in_manifest){
    std::cout << "An error occurred while opening manifest file in createArticle." <<std::endl;
    exit(1);
  }
  std::string ngName;
  unsigned ng,artID;
  while(!in_manifest.eof()){
    in_manifest >> ngName;
    in_manifest >> ng;
    in_manifest >> artID;
    if(ng == ng_ID){ // found correct
      break;
    }
  }
  in_manifest.close();
  if(ng != ng_ID) // not found
    return false;

  // increase counter of created articles in manifest
  increaseArtCounter(ng_ID);
  artID++; // to match increased Article count

  char filePath[512] = "./Database/";
  strcat(filePath, ngName.c_str());
  filePath[strlen(filePath)] = '/';
  std::string tmpArtID = std::to_string(artID);
  strcat(filePath, tmpArtID.c_str());

  std::ofstream article(filePath, std::ofstream::trunc);

  if(!article){
    std::cout<< "error opening article for output!"<<std::endl;
    exit(1);
  }

  article << title << std::endl;
  article << author << std::endl;
  article << tmpArtID << std::endl;
  article << text;

  return true;
}


// Increase article counter for Newsgroup with ID with one in manifest file
void FileDatabase::increaseArtCounter(unsigned ID){
  std::fstream file("./Database/manifest");

  if(!file){std::cout << "Error in increaseartcountrer."<< std::endl;
    exit(1);
  }
  file.seekg(0,file.end);
  int filelength = file.tellg();
  char* contents = new char[filelength];
  file.seekg(0,file.beg);

  std::string str;
  int tmp2, position = 0;
  unsigned tmp1;
  while(!file.eof()){
    file >> str;
    file >> tmp1;
    if(tmp1 == ID){
      position = file.tellg();
      position++; // For space
      file >> tmp2;
      break;
    }
  }
  file.read(contents, filelength);
  tmp2++;
  str = std::to_string(tmp2);
  char* newContents = new char[filelength];
  strcat(newContents, str.c_str());
  strcat(newContents,contents);
  file.clear();

  if(!file.good()){
    std::cout << "något är fel!";
  }
  file.seekg(position);
  file.write(newContents, filelength);

  delete[] newContents;
  delete[] contents;

  file.close();
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
