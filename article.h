#ifndef ARTICLE_H
#define ARTILE_H

#include <string>

using std::string;

class Article{
public:
    Article(string, string, string, int);
    string getText();
    string getTitle();
    string getAuthor();
    int getID();
private:
    string title;
    string author;
    string text;
    int id;
};

#endif