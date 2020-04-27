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
#include <sstream>

using std::ofstream;
using std::ifstream;
using std::fstream;
using std::cout;
using std::endl;
using std::string;


FileDatabase::FileDatabase(){

  DIR* dir = nullptr;


  if((dir = opendir(root)) != nullptr){
    std::fstream manifest_in(manifestPath);  // the database has a file 'manifest' used to keep track of newsGroups
    string tempNG;
    unsigned id, tempArtCount;
    if(manifest_in.is_open()){
        while(manifest_in){
            manifest_in >> tempNG >> id >> tempArtCount;
        }
        NEWSGROUP_ID = id;
        manifest_in.close();

    } else {
        std::cout << "Problem opening manifestfile." << std::endl;
    }

  }else{
      int status = mkdir(root, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      if(status == -1 ){
        std::cerr << "Unable to create root directory in database:\t" << std::strerror(errno)
                  << std::endl;;
        exit(1);
      }else{
        // std::cout << "Successfully created root directory" << std::endl;
        NEWSGROUP_ID = 0;  // +1 from manifest
      }
  }
  closedir(dir);

  std::ofstream manifest(manifestPath , std::ofstream::app); // list of Newsgroups

  manifest.close();
}

FileDatabase::~FileDatabase(){

}

// Extracts information from a line in the manifest file
// ~ is the delimiter for the name of the newsgroup
Newsgroup FileDatabase::extract(std::string s){
  std::string name;
  unsigned ID , art_Counter;
  std::stringstream ss(s);

  std::getline(ss, name,  '~');
  ss >> ID;
  ss >> art_Counter;

  return Newsgroup{name, ID , art_Counter, std::unordered_map<unsigned, Article>()};
}

std::vector<Newsgroup> FileDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  std::vector<Article> tmpA;

  ifstream manifest(manifestPath);
  if(!manifest.good()){
    std::cerr << "Error opening manifest file!" << std::endl;
    exit(1);
  }

  string tempRow;
  while(std::getline(manifest, tempRow)){
    Newsgroup tempNG = extract(tempRow);

    ngs.push_back(tempNG);
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

  ifstream in_manifest(manifestPath);
  if(!in_manifest){
    std::cout << "Unable to open manifest file in CreateNewsgroup." << std::endl;
    exit(1);
  }

  string tempRow;
  while(std::getline(in_manifest, tempRow)){
    Newsgroup tempNG = extract(tempRow);

    if(tempNG.name == name){  // already exists a newsgroup with that name
      in_manifest.close();
      return false;
    }

  }

  in_manifest.close();

  ofstream out_manifest(manifestPath, std::ofstream::app);

  if(!out_manifest){
    std::cout << "An error occurred while opening manifest." << std::endl;
    exit(1);
  }

  struct stat sb;
  char dirPath[512] = "./FileDatabase/";
  strcat(dirPath, name.c_str());

  if(stat(dirPath, &sb) == 0){ // Error should be no file with that name
    return false;
  }

  // Structure of manifest file:
  // [Newsgroup name] [newsgroup ID] [article counter]
  ++NEWSGROUP_ID;

  out_manifest << name << "~ " << NEWSGROUP_ID <<  " 0" << "\n";

  out_manifest.close();

  if(!S_ISDIR(sb.st_mode)){  // create the directory for the corresponding newsgroup
    int status = mkdir(dirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == -1){
      std::cout << "An error occurred while creating newsgroup directory." <<std::endl;
      exit(1);
    }
  }
  return true;
}


bool FileDatabase::deleteNewsgroup(unsigned ng_ID){

  if(!ngExists(ng_ID))
    return false;

  std::fstream manifest(manifestPath);

  if(!manifest){
    // Error opening manifest
    exit(1);
  }
  manifest.seekg(0,manifest.end);
  long filelength = manifest.tellg();
  char* contents1 = new char[filelength];
  char* contents2 = new char[filelength];
  manifest.seekg(0,manifest.beg);

  string tempRow, tmpName, name;
  unsigned tempID = 0;
  long pos1 = 0;

  while(std::getline(manifest, tempRow)){
    Newsgroup tempNG = extract(tempRow);

    if(tempNG.newsGroup_ID == ng_ID){
      tmpName = tempNG.name;
      tempID = ng_ID;
      break;
    }else{
      pos1 = manifest.tellg();
    }
  }

  if(tempID != ng_ID){ // Newsgroup not found
    return false;
  }

  name = tmpName;
  long pos2 = manifest.tellg();
  manifest.seekg(pos2);
  manifest.read(contents2, filelength - pos2);
  manifest.clear();
  manifest.seekg(0);
  manifest.read(contents1, pos1);
  manifest.close();
  manifest.open(manifestPath, std::fstream::out | std::fstream::trunc);

  manifest.write(contents1 , pos1); // Content before
  manifest.write(contents2, filelength - pos2); // content after

  manifest.close();

  // remove newsgroup line from manifest
  char path[512] = "./FileDatabase/";
  strcat(path, name.c_str());

  // remove folder
  int status = removeNewsgroup(path);
  if(status == -1){
    std::cout << "An error occured while deleting newsgroup:\t"
              << strerror(errno) << std::endl;
    exit(1);
  }

  delete[] contents1;
  delete[] contents2;
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
  closedir(dir);
  return remove(path);
}

// Presumes the newsgroup with ng_ID exists
std::vector<Article> FileDatabase::listArticles(unsigned ng_ID){
  std::vector<Article> arts;
  DIR *dir = nullptr;
  struct dirent* ent = nullptr;

  ifstream article;

  ifstream manifest(manifestPath);
  if(!manifest.good()){
    std::cout << "An error occurred when opening manifest file \n";
    exit(1);
  }

  string tempRow, name;
  while(std::getline(manifest, tempRow)){
    Newsgroup tempNG = extract(tempRow);
    if(tempNG.newsGroup_ID == ng_ID){
      name = tempNG.name;
      break;
    }
  }

  manifest.close();

  char dirLocation[512];
  strcpy(dirLocation , root);
  dirLocation[strlen(dirLocation) + 1] = '\0';
  dirLocation[strlen(dirLocation) ] = '/';
  char filepath[512];

  strcat(dirLocation, name.c_str());
  dirLocation[strlen(dirLocation)  + 1] = '\0';
  dirLocation[strlen(dirLocation) ] = '/';

  if((dir = opendir(dirLocation)) == nullptr){
    std::cerr << "Error opening newsgroup folder in listArticles:\t" <<  std::strerror(errno) <<std::endl;
    exit(1);
  }
  std::string title, author, text, tmp;
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
  closedir(dir);

  std::sort(arts.begin(),arts.end(), [&](Article &a1, Article &a2){return a1.article_ID < a2.article_ID;});  // sort articles

  return arts;
}

// Creates a new article in the ng_ID newsgroup
// Returns false if the newsgroup does not exist
bool FileDatabase::createArticle(unsigned ng_ID , string title, string author, string text){

  std::ifstream in_manifest(manifestPath);
  if(!in_manifest){
    std::cout << "An error occurred while opening manifest file in createArticle." <<std::endl;
    exit(1);
  }

  std::string tempRow, ngName;
  unsigned ng=99999999, artID = 0;
  while(std::getline(in_manifest, tempRow)){
    Newsgroup tempNG = extract(tempRow);
    if(tempNG.newsGroup_ID == ng_ID){  // found the correct newsgroup
      ngName = tempNG.name;
      ng = ng_ID;
      artID = tempNG.article_IDs;
      break;
    }

  }


  in_manifest.close();
  if(ng != ng_ID) // newsgroup not found
    return false;

  // increase counter of created articles in manifest
  increaseArtCounter(ng_ID);
  artID++; // necessary to match increased Article count

  char filePath[512] = "./FileDatabase/";
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
  std::fstream file(manifestPath);

  if(!file){
    std::cout << "Unable to open manifest file in increaseArtCounter."<< std::endl;
    exit(1);
  }
  // Find file length and reset file position
  file.seekg(0,file.end);
  int filelength = file.tellg();
  char* contents = new char[filelength];
  file.seekg(0,file.beg);

  string tempRow, str;
  int artCnt = 0;
  int position = 0;
  int tmp = 0;
  while(std::getline(file, tempRow)){ // Find correct postion of Article count of correct Newsgroup
    Newsgroup tempNG = extract(tempRow);
    if(tempNG.newsGroup_ID == ID){
      str = tempNG.name;
      file.unget();
      // Finds position in stream of artCount
      while(file.peek() != ' '){
        file.unget();
        //file // Backs upp until right before artCount
      }

      position = file.tellg();
      position++;  // For space in file
      artCnt = tempNG.article_IDs;
      file >> tmp; // Reads old artCount
      break;
    }
    artCnt = tempNG.article_IDs;
  }



  file.read(contents, filelength); // reads the rest of the file

  artCnt++; // increases Article count by one before writing it back into file
  str = std::to_string(artCnt);
  int i = 0;
  if(artCnt % 10 == 0)
    i++;

  char* newContents = new char[filelength - position + i];
  strcpy(newContents, str.c_str()); // adds new number
  strcat(newContents,contents); // adds rest of file after Article Count
  file.clear(); // resets file

  if(!file.good()){ // If file not ready to write too
    std::cout << "Unable to reset manifest stream in increaseArtCounter." << std::endl;
    exit(1);
  }

  file.seekg(position); // find correct position for Article count

  file.write(newContents, filelength - position + i); // overwrite rest of file with new content

  delete[] newContents; // Free up memory
  delete[] contents;

  file.close();
}


bool FileDatabase::deleteArticle(unsigned ng_ID , unsigned art_ID){

  std::ifstream manifest(manifestPath);
  if(!manifest){
    std::cout << "Error in deleteArticle. Unable to open manifest." << std::endl;
    exit(1);
  }

  unsigned tempID = 0;
  string tempRow, tempName;
  while(std::getline(manifest, tempRow)){ // Find correct postion of Article count of correct Newsgroup
    Newsgroup tempNG = extract(tempRow);
    if(tempNG.newsGroup_ID == ng_ID){
      tempName = tempNG.name;
      tempID = tempNG.newsGroup_ID;
      break;
    }
  }

  manifest.close();
  if(tempID != ng_ID){
    // Error newsgroup does not exist
    return false;
  }

  char path[512] = "./FileDatabase/";
  strcat(path , tempName.c_str());
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


  char path[512] = "./FileDatabase/";
  strcat(path , tmpNgName.c_str());
  path[strlen(path)] = '/';
  string tmpArtID = std::to_string(art_ID);
  strcat(path , tmpArtID.c_str());

  ifstream article(path);
  if(!article.good()){
    std::cout << "Unable to open manifest file in getArticle." << std::endl;
    exit(1);
  }


  string title , author, strID;
  unsigned ID = 0;

  std::getline(article , title);
  std::getline(article , author);
  std::getline(article , strID);

  try{
    ID = std::stoi(strID);
  }catch(std::exception& e){
    std::cerr << "Error during number conversion in getArticle:\t" << e.what() << std::endl;
  }

  int current_position = article.tellg();

  article.seekg(0,article.end);
  int textLength = static_cast<int>(article.tellg()) - current_position;
  char* textArr = new char[textLength + 1];
  article.seekg(current_position);

  article.read(textArr , textLength );
  textArr[textLength] = '\0';
  string text(textArr);
  delete[] textArr;

  return Article{title,author, ID, text};
}


bool FileDatabase::ngExists(unsigned ng_ID){

  std::ifstream manifest(manifestPath);

  if(!manifest){
    std::cout << "Error in ngExists. Unable to open manifest." << std::endl;
    exit(1);
  }

  unsigned tempID = 999999999;
  string tempRow, tempName;
  while(std::getline(manifest, tempRow)){ // Find correct postion of Article count of correct Newsgroup
    Newsgroup tempNG = extract(tempRow);
    if(tempNG.newsGroup_ID == ng_ID){
      tempName = tempNG.name;
      tempID = tempNG.newsGroup_ID;
      break;
    }
  }


  manifest.close();
  tmpNgName = tempName;
  return (tempID == ng_ID);
}



// Will not function properly if ngExists has not been run
bool FileDatabase::artExists(unsigned ng_ID, unsigned art_ID){

  char filepath[1024] = "./FileDatabase/";

  if(!ngExists(ng_ID)){ // check if ng exist by id
    return false;
  }

  // tmpNgName needs to be set by ngExisits
  strcat(filepath, tmpNgName.c_str());
  filepath[strlen(filepath)] = '/';
  std::string strArtID = std::to_string(art_ID);
  strcat(filepath, strArtID.c_str());

  struct stat sb;
  return (stat(filepath, &sb) == 0); // no error, if == -1 then could check errno
}
