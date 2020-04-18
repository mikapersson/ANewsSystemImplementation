#include "FileDatabase.h"
#include <iostream>
#include <vector>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace std;


int main(int argc, char *argv[]){

/*
  FileDatabase d;
  std::cout << "Directory created:\t" <<static_cast<bool> (d.createNewsgroup("News12"))
            << std::endl;
  std::cout << "Article created:\t" <<(d.createArticle(1,"Intresting things about","Me and myself", "Text about me"))
                      << std::endl;
  Article a = d.getArticle(1,1);
  std::cout << "title:\t" << a.title << "\tauthor:\t"<< a.author
            << "\n\n" << a.text << std::endl;*/


  //std::cout << "Article deleted:\t" <<(d.deleteArticle(1,1))
              //                  << std::endl;

  /*
  std::cout << "Directory created: " <<static_cast<bool> (d.createNewsgroup("News2"))
  << std::endl;
  std::cout << "Directory created: " <<static_cast<bool> (d.createNewsgroup("News3"))
  << std::endl;
  std::cout << "Article created: " << static_cast<bool>(d.createArticle(1,"title","author","Text text"))
  << std::endl;
  //std::cout << "Newsgroup deleted: " << static_cast<bool>(d.deleteNewsgroup(1))
  //<< std::endl;
  //std::vector<Newsgroup> ngs = d.listNewsgroups();
  std::cout << "Should exist: " << d.ngExists(1) << std::endl;
  std::cout << "Should not exist: " << d.ngExists(6) << std::endl;
  std::cout << "Should exist: " << d.artExists(1,1) << std::endl;
  std::cout << "should not exist: " << d.artExists(2,1) << std::endl;
  */
    /*
  std::vector<Article> arts = d.listArticles(1);
  for(auto& a : arts){
    std::cout << "First article:\t" << a.title << "\tauthor:\t" << a.author
              << std::endl << std::endl << a.text << std::endl;
  }
  */

 //MIKA TESTAR
  FileDatabase database;
  cout << "Created FileDatabase object" << endl;

  //auto ngList = database.listNewsgroups();

}