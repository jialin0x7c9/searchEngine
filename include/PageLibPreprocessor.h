#ifndef __PAGELIBPREPROCESSOR_H__
#define __PAGELIBPREPROCESSOR_H__
#include "Configuration.h"
#include "WebPage.h"

#include <vector>

//因为在"simhash/Simhasher.hpp"里面include了unordered_map
//而且在std做了不知道什么事情；所以不能直接using std::unordered_map
using std::vector;
using std::pair;

class PageLibPreprocessor
{
public:
    PageLibPreprocessor(Configuration &conf);
    void doProcess();
private:
    void readPageFromFile();
    void cutRedundantPage();



    void storeNewPageAndOffsetOnDisk();
    void buildInvertIndexTable();
    int getRipePageTxt(const string &indexFilePath, const string &filePath, string &txt);

    //将去重后的网页库、偏移库、倒排索引表写回磁盘；
    void storeInvertTableOnDisk();
private:
    vector<WebPage> _vecPage;
    Configuration &_conf;
    //docid offset length
    map<int, pair<int, int>> _newOffsetLib;
    //词语，docid, weight
    std::unordered_map<string, vector<pair<int, double>>> _invertIndexTable;
};
#endif
