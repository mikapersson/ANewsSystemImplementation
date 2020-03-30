#include "InMemDatabase.h"


InMemDatabase::InMemDatabase() {

  NEWSGROUP_ID = 0;
}

std::vector<Newsgroup> InMemDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  for(auto& n : newsgroups){
    ngs.push_back(n.second);
  }
  return ngs;
}

bool InMemDatabase::createNewsgroup(string name){

  for(auto& n : newsgroups){ // If there is newsgroup with that name
    if(n.second.name == name){
      //throw std::invalid_argument("ERR_NG_ALREAD_EXISTS");  //ta bort?
      return false;
    }
  }
  NEWSGROUP_ID++; // To make sure that every newsgroup is unique
  Newsgroup n = {name, NEWSGROUP_ID};
  newsgroups[NEWSGROUP_ID] =n;
  return true;
}


void InMemDatabase::deleteNewsgroup(unsigned ng_ID){

  if(newsgroups.count(ng_ID) == 0)
    throw std::invalid_argument("ERR_NG_DOES_NOT_EXIST");

  newsgroups.erase(ng_ID);


}

vector<Article> InMemDatabase::listArticles(unsigned ng_ID){
  Newsgroup ng = newsgroups[ng_ID];
  vector<Article> articles;

  for(auto& a : ng.articles){
    
  }


  return vector<Article>();
}
