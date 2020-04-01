#include "InMemDatabase.h"
#include <iostream>
#include <algorithm>

InMemDatabase::InMemDatabase() {
  NEWSGROUP_ID = 0;
}

InMemDatabase::~InMemDatabase(){}

std::vector<Newsgroup> InMemDatabase::listNewsgroups(){
  std::vector<Newsgroup> ngs;
  for(auto& n : newsgroups){
    ngs.push_back(n.second);
  }
  std::sort(ngs.begin(),ngs.end(), [&](Newsgroup &n1, Newsgroup &n2){return n1.newsGroup_ID < n2.newsGroup_ID;});
  return ngs;
}

bool InMemDatabase::createNewsgroup(string name){

  for(auto& n : newsgroups){
    if(n.second.name == name){
      return false;
    }
  }
    NEWSGROUP_ID++; // To make sure that every newsgroup is unique
    Newsgroup n = {name, NEWSGROUP_ID, 0,std::unordered_map<unsigned,Article>()};
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

  for(auto& a : ng.articles){
    articles.push_back(a.second);
  }

  std::sort(articles.begin(),articles.end(), [&](Article &a1, Article &a2){return a1.article_ID < a2.article_ID;});
  return articles;
}



bool InMemDatabase::createArticle(unsigned ng_ID , string title, string author, string text){
    Newsgroup* ng;
    try{
        ng = &newsgroups.at(ng_ID);
    }catch(std::out_of_range e){
        std::cout << "no ng id\n";
        return false;
    }
    ng->article_IDs++;
    ng->articles.insert({ng->article_IDs,Article{title,author,ng->article_IDs,text}});

    return true;

}
// Utgår från att den går igenom
bool InMemDatabase::deleteArticle(unsigned ng_ID , unsigned art_ID){

  Newsgroup* ng = &newsgroups.at(ng_ID);
  ng->articles.erase(art_ID);
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
