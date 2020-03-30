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

  for(auto& n : newsgroups){
    if(n.second.name == name){
      return false;
    }
  }
    NEWSGROUP_ID++; // To make sure that every newsgroup is unique
    Newsgroup n = {name, NEWSGROUP_ID};
    newsgroups[NEWSGROUP_ID] =n;
    return true;
}


bool InMemDatabase::deleteNewsgroup(unsigned ng_ID){

  if(newsgroups.count(ng_ID) == 0)
    return false;

  newsgroups.erase(ng_ID);
  return true;


}

vector<Article> InMemDatabase::listArticles(unsigned ng_ID){
  Newsgroup ng = newsgroups[ng_ID];
  vector<Article> articles;
  //std::for_each(ng.articles.begin(),ng.articles.end(),[]( std::pair<unsigned,Article> &p){articles.push_back(p.second);});

  for(auto& a : ng.articles)
    articles.push_back(a.second);

  return articles;
}



bool InMemDatabase::createArticle(unsigned ng_ID , string title, string author, string text){
    Newsgroup ng;
    try{
        ng = newsgroups.at(ng_ID);
    }catch(std::out_of_range e){
        return false;
    }
    ng.article_IDs++;
    ng.articles.insert({ng.article_IDs,Article{title,author,ng.article_IDs,text}});

    return true;

}
// Utgår från att den går igenom
bool InMemDatabase::deleteArticle(unsigned ng_ID , unsigned art_ID){

  Newsgroup ng = newsgroups.at(ng_ID);
  ng.articles.erase(art_ID);
  return true;
}


Article InMemDatabase::getArticle(unsigned ng_ID , unsigned art_ID){
  return newsgroups.at(ng_ID).articles.at(art_ID);
}

bool InMemDatabase::ngExists(unsigned ng_ID){

  try{
    newsgroups.at(ng_ID);
  }catch(std::out_of_range e){
    return false;
  }
  return true;
}


bool InMemDatabase::artExists(unsigned ng_ID, unsigned art_ID) {
  try{
    newsgroups.at(ng_ID).articles.at(art_ID);

  }catch(std::out_of_range e){
    return false;
  }

  return true;
}
