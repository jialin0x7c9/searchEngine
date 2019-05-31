#ifndef __PAGELIBPREPROCESSOR_H__
#define __PAGELIBPREPROCESSOR_H__
#include "Configuration.h"
/* #include <unordered_map> */
#include <vector>

/* using std::unordered_map; */
using std::vector;
using std::pair;

class PageLibPreprocessor
{
public:
    PageLibPreprocessor(Configuration &conf);
    void doProcess();

private:
    void readAndCutRedundantPage();
    void buildInvertIndexTable();

    //将去重后的网页库、偏移库、倒排索引表写回磁盘；
    void storeOnDisk();
private:
    Configuration &_conf;
    map<int, pair<int, int>> _newOffsetLib;
    /* unordered_map<string, vector<pair<int, double>>> _invertIndexTable; */
};

#endif
