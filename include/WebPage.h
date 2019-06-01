#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__
#include "Configuration.h"
#include <string>
#include <map>
using std::map;
using std::string;

class WebPage
{
public:
    WebPage(string &doc, Configuration &conf);
    int getDocid() const;
    string getContent() const;
    map<string, int>* getWordsMapPtr();
private:
    void processDoc();
    void constructWordMap();
private:
    Configuration &_conf; //用来获得停用词:
    string _doc; //包含标签的
    int _docid;
    string _docTitle;
    string _docUrl;
    string _docContent;
    string _docSummary;
    map<string, int> _wordsMap;
};

#endif
