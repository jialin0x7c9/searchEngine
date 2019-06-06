#ifndef __WORDQUERY_H__
#define __WORDQUERY_H__
#include "WebPage.h"
#include <vector>
#include <string>
#include <map>
/* #include <unordered_map> */

class Configuration;
using std::vector;
using std::string;
using std::map;
using std::pair;
/* using std::unordered_map; */

class WordQuery
{
public:
    WordQuery(Configuration &conf);
    string doQuery(const string &msg);
private:
    void loadLibrary();
    vector<double> getQueryWordsWeightVector(map<string, int> &mapWordFreq, const vector<string> &baseWord);
    bool executeQuery(const vector<string> &baseWord, vector<pair<int, vector<double>>> &resultVec);
    string createJson(vector<int> &docidVec, const vector<string> &queryWords);
    string returnNoAnswer();
private:
    Configuration &_conf;
    std::unordered_map<int, WebPage> _pageLib;
    map<int, pair<int, int>> _offsetLib;
    std::unordered_map<string, vector<pair<int, double>>> _invertIndexTable;

};

#endif
