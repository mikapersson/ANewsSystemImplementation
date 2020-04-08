#include "FileDatabase.h"
#include <iostream>
#include <vector>

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(int argc, char *argv[]){


  FileDatabase d;
  std::cout << "Directory created: " <<static_cast<bool> (d.createNewsgroup("Ne ws1"))
  << std::endl;
  //std::vector<Newsgroup> ngs = d.listNewsgroups();

  /*
  std::vector<Article> arts = d.listArticles(1);
  for(auto& a : arts){
    std::cout << "First article:\t" << a.title << "\tauthor:\t" << a.author
              << std::endl << std::endl << a.text << std::endl;
  }
  */

}
