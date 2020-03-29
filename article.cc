#include "article.h"

Article::Article(string t, string a, string text, int id) : title{t}, author{a}, text{text}, id{id}{}

string Article::getAuthor(){
    return author;
}

string Article::getTitle(){
    return title;
}

string Article::getText(){
    return text;
}

int Article::getID(){
    return id;
}