#include "FileDatabase.h"
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <errno.h>
#include <stdlib.h>


using std::ofstream;
using std::ifstream;
using std::fstream;
using std::string;


FileDatabase::FileDatabase(){

  DIR* dir = nullptr;
  struct dirent* ent;

  if((dir = opendir(root)) != nullptr){
    while((ent = readdir(dir)) != nullptr){
      //std::cout << "File: " << ent->d_name <<std::endl;
    }
  }else{
      std::cout << "No existing database found. Creating new...\n";
      int status = mkdir("./Database", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      if(status == -1 ){
        std::cerr << "Unable to create root directory in database:\t" << std::strerror(errno)
                  << std::endl;;
        exit(1);
      }else{
        // std::cout << "Successfully created root directory" << std::endl;
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
    in_manifest >> tmpName >> tmpId >> artCount;

    if(tmpName == name){ // Already exists
      in_manifest.close();
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
  // [Newsgroup name] [newsgroup ID] [article counter]

  out_manifest << name << " " << NEWSGROUP_ID <<  " 0" << "\n";
  out_manifest.close();

  struct stat sb;
  char dirPath[512] = "./Database/";
  strcat(dirPath, name.c_str());

  if(stat(dirPath, &sb) == 0){ // Error should be no file with that name
    // Delete that file
    return false;
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
    // Error opening manifest
    exit(1);
  }
  manifest.seekg(0,manifest.end);
  int filelength = manifest.tellg();
  char* contents = new char[filelength];
  manifest.seekg(0,manifest.beg);


  unsigned tmpID, tmpArtCounter;
  string tmpName,name;
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

  // remove folder
  int status = removeNewsgroup(path);
  if(status == -1){
    std::cout << "An error occured in delete newsgroup" << std::endl;
    std::cout << "Errno: \t" << strerror(errno) << std::endl;
    exit(1);
  }

  delete[] contents;
  return true;
}

int FileDatabase::removeNewsgroup(char path[]){

  DIR* dir = nullptr;
  struct dirent* ent = nullptr;
  struct stat sb; // Used to check file existence and type

  char filename[1024]; // Borde väl ha någon gemensam konstant för buffersize
  strcpy(filename , path);
  filename[strlen(filename)] = '/';

  if((dir = opendir(path)) == nullptr){
    // Unable to open newsgroup Directory
    // Failure, exit
  }
  while((ent = readdir(dir)) != nullptr){
    strcat( filename , ent->d_name); // fullname of file
    stat(filename, &sb);
    // std::cout << filename << std::endl;
    if(S_ISREG(sb.st_mode)){
      if(remove(filename) == -1){ // Removes file
        // An error occured check errno
        return -1;
      }
    }
    strcpy(filename, path); // reseting filename
    filename[strlen(filename) + 1] = '\0';
    filename[strlen(filename)] = '/';

  }

  return remove(path);
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
  std::string title, author, text,tmp;
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

    while(article >> tmp){ // Fult men funkar. Borde ändra till article.read!!
      text += tmp;
      text += " ";
    }

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
    if(ng == ng_ID){ // found correct newsgroup
      break;
    }
  }

  in_manifest.close();
  if(ng != ng_ID) // not found
    return false;

  // increase counter of created articles in manifest
  increaseArtCounter(ng_ID);
  artID++; // necessary to match increased Article count

  char filePath[512] = "./Database/";
  strcat(filePath, ngName.c_str());
  filePath[strlen(filePath)] = '/';
  std::string tmpArtID = std::to_string(artID);
  strcat(filePath, tmpArtID.c_str());

  std::ofstream article(filePath , std::ofstream::trunc);

  if(!article){
    std::cout << "Error opening article for output in createArticle."<<std::endl;
    exit(1);
  }

  article << title << std::endl;
  article << author << std::endl;
  article << tmpArtID << std::endl;
  article << text;

  article.close();

  return true;
}


// Increase article counter for Newsgroup with ID with one in manifest file
void FileDatabase::increaseArtCounter(unsigned ID){
  std::fstream file("./Database/manifest");

  if(!file){std::cout << "Error in increaseartcounter."<< std::endl;
    exit(1);
  }
  // Find file length and reset file position
  file.seekg(0,file.end);
  int filelength = file.tellg();
  char* contents = new char[filelength];
  file.seekg(0,file.beg);

  std::string str;
  int tmp2, position = 0;
  unsigned tmp1;
  // Find correct postion of Article count of correct Newsgroup
  while(!file.eof()){
    file >> str;
    file >> tmp1;
    if(tmp1 == ID){
      position = file.tellg();
      position++; // For space in file
      file >> tmp2;
      break;
    }
  }

  file.read(contents, filelength); // reads the rest of the file

  tmp2++; // increases Article count by one before writing it back into file
  str = std::to_string(tmp2);
  char* newContents = new char[filelength];
  strcat(newContents, str.c_str()); // adds new number
  strcat(newContents,contents); // adds rest of file after Article Count
  file.clear(); // resets file

  if(!file.good()){ // If file not ready to write too
    std::cout << "Error in artCounter.";
  }

  file.seekg(position); // find correct position for Article count
  file.write(newContents, filelength); // overwrite rest of file with new content

  delete[] newContents; // Free up memory
  delete[] contents;

  file.close();
}


bool FileDatabase::deleteArticle(unsigned ng_ID , unsigned art_ID){

  std::ifstream manifest("./Database/manifest");
  if(!manifest){
    std::cout << "Error in deleteArticle. Unable to open manifest." << std::endl;
    exit(1);
  }

  unsigned tmpID, tmpArtCounter;
  std::string tmpName;

  while(!manifest.eof()){
    manifest >> tmpName >> tmpID >> tmpArtCounter;
    if(tmpID == ng_ID)
      break;
}
  manifest.close();
  if(tmpID != ng_ID){
    // Error newsgroup does not exist
    return false;
  }

  char path[512] = "./Database/";
  strcat(path , tmpName.c_str());
  path[strlen(path)] = '/';
  string tmpArtID = std::to_string(art_ID);
  strcat(path , tmpArtID.c_str());
  // path now contains path to article file

  int status = remove(path); // removes article file

  if(status == -1){ // If error in remove
    if(errno == 2){ // No such file or directory
      return false;
    }else{
      std::cout << "Error in deleteArticle:\t" << std::strerror(errno) <<std::endl;
      exit(1);
    }
  }
  return true;
}

Article FileDatabase::getArticle(unsigned ng_ID , unsigned art_ID){

  // Find newsgroup
  if(!ngExists(ng_ID)){
    std::cout << "Ng not found:\t" << ng_ID << std::endl;
    exit(1);
  }


  char path[512] = "./Database/";
  strcat(path , tmpNgName.c_str());
  path[strlen(path)] = '/';
  string tmpArtID = std::to_string(art_ID);
  strcat(path , tmpArtID.c_str());

  ifstream article(path);
  if(!article.good()){
    std::cout << "Can not read manifest!" << std::endl;
    exit(1);
  }
  article.seekg(0,article.end);
  int filelength = article.tellg();
  char* textArr = new char[filelength];
  article.seekg(0);

  string title , author;
  unsigned ID;

  std::getline(article , title);
  std::getline(article , author);
  article >> ID;

  article.read(textArr , filelength);
  string text(textArr);

  return Article{title,author, ID, text};
}


bool FileDatabase::ngExists(unsigned ng_ID){

  std::ifstream manifest("./Database/manifest");

  if(!manifest){
    std::cout << "Error in ngExists. Unable to open manifest." << std::endl;
    exit(1);
  }

  unsigned tmpID, tmpArtCounter;
  std::string tmpName;

  while(!manifest.eof()){
    manifest >> tmpName >> tmpID >> tmpArtCounter;
    if(tmpID == ng_ID)
      break;
}
  manifest.close();
  tmpNgName = tmpName;
  return (tmpID == ng_ID);
}

bool FileDatabase::artExists(unsigned ng_ID, unsigned art_ID){

  char filepath[1024] = "./Database/";

  if(!ngExists(ng_ID)){
    return false;
  }

  /*
  std::ifstream manifest("./Database/manifest");

  if(!manifest){
    std::cout << "Error in ngExists. Unable to open manifest." << std::endl;
    exit(1);
  }

  unsigned tmpID, tmpArtCounter;
  std::string tmpName;

  while(!manifest.eof()){
    manifest >> tmpName >> tmpID >> tmpArtCounter;
    if(tmpID == ng_ID)
      break;
}
  manifest.close();

  if(tmpID != ng_ID)
    return false;
  */
  strcat(filepath, tmpNgName.c_str());
  filepath[strlen(filepath)] = '/';
  std::string strArtID = std::to_string(art_ID);
  strcat(filepath, strArtID.c_str());

  struct stat sb;
  return (stat(filepath, &sb) == 0); // no error, if == -1 then could check errno
}
