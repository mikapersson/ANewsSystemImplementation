


FileDatabase::FileDatabase(){

  int status = mkdir("/Database");

  if(status == -1 ){
    std::std::cerr << "Unable to create root directory in database." << '\n';
    exit(1);
  }else{
    std::cout << "successfully created directory" << std::endle;
  }

}

FileDatabase::~FileDatabase(){

}
std::vector<Newsgroup> listNewsgroups(){
  return std::vector<Newsgroup>();
}


bool createNewsgroup(string name){
  return false;
}


bool deleteNewsgroup(unsigned ng_ID){
  return false;
}


std::vector<Article> listArticles(unsigned ng_ID){
  return std::vector<Article>();
}


bool createArticle(unsigned ng_ID , string title, string author, string text){
  return false;
}


bool deleteArticle(unsigned ng_ID , unsigned art_ID){
  return false;
}

Article getArticle(unsigned ng_ID , unsigned art_ID){
  return Article();
}


bool ngExists(unsigned ng_ID){
  return false;
}

bool artExists(unsigned ng_ID, unsigned art_ID){
  return false;
}
