#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__
#include "Configuration.h"
#include "simhash/Simhasher.hpp"
#include <string>
#include <map>
#include <vector>
using std::map;
using std::string;
using std::vector;

class WebPage
{
public:
    WebPage() {}
    WebPage(string &doc, int offset);
    WebPage(string content);
    int getDocid() const;
    string getContent() const;
    string getTitle() const; 
    string getUrl() const;
    map<string, int>* getWordsMapPtr();
    void calSimhash();
    uint64_t getSimhash();
    int getOffset() const;
    int getLength() const;
    void constructWordMap(Configuration &conf);
    void processDoc();
    string summary(const vector<string> &queryWords);
private:
    /* Configuration &_conf; //用来获得停用词: */
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
