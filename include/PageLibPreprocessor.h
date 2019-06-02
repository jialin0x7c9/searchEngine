#ifndef __PAGELIBPREPROCESSOR_H__
#define __PAGELIBPREPROCESSOR_H__
#include "Configuration.h"
#include <vector>
#include "simhash/Simhasher.hpp"

//因为在"simhash/Simhasher.hpp"里面包含了所以会出现error 具体原因还不知如何排除；
using std::unordered_map;
using std::vector;
using std::pair;

class PageLibPreprocessor
{
public:
    PageLibPreprocessor(Configuration &conf);
    void doProcess();

private:
    void storeNewPageAndOffsetOnDisk();
    void readAndCutRedundantPage();
    void buildInvertIndexTable();
    int getRipePageTxt(const string &indexFilePath, const string &filePath, string &txt);

    //将去重后的网页库、偏移库、倒排索引表写回磁盘；
    void storeInvertTableOnDisk();
private:
    Configuration &_conf;
    map<int, pair<int, int>> _newOffsetLib;
    //词语，docid, weight
    unordered_map<string, vector<pair<int, double>>> _invertIndexTable;
};

#endif
