#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__
#include "Configuration.h"
#include <string>
#include <map>
#include "simhash/Simhasher.hpp"
using std::map;
using std::string;

class WebPage
{
public:
    WebPage(string &doc, int offset, Configuration &conf);
    int getDocid() const;
    string getContent() const;
    map<string, int>* getWordsMapPtr();
    void calSimhash();
    uint64_t getSimhash();
    int getOffset() const;
    int getLength() const;

private:
    void processDoc();
    void constructWordMap();
private:
    Configuration &_conf; //用来获得停用词:
    string _doc; //包含标签的
    int _docid;
    int _offset;
    string _docTitle;
    string _docUrl;
    string _docContent;
    string _docSummary;
    map<string, int> _wordsMap;
    uint64_t _u64;
};

#endif
