#include "FileDatabase.h"
#include <iostream>
#include <vector>

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(int argc, char *argv[]){


  FileDatabase d;

  std::cout << "Directory created: " <<static_cast<bool> (d.createNewsgroup("News1"))
            << std::endl;
  d.deleteArticle(1,1);
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

}
