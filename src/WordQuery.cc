#include <stdio.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>
#include <algorithm>
#include <json/json.h>
#include "WordQuery.h"
#include "Configuration.h"
using std::istreambuf_iterator;
using std::ifstream;
using std::make_pair;
using std::istringstream;
using std::priority_queue;
using std::endl;
using std::cout;

struct SimilarityCompare
{
    SimilarityCompare(vector<double> &base):_base(base){}
    bool operator()(const pair<int, vector<double>> &lhs, const pair<int, vector<double>> &rhs)
    {
        double lhsCrossProduct = 0;
        double rhsCrossProduct = 0;
        double lhsVectorLength = 0;
        double rhsVectorLength = 0;
        for (int idx = 0; idx != static_cast<int>(_base.size()); ++idx)
        {
            lhsCrossProduct += (lhs.second)[idx] * _base[idx];
            rhsCrossProduct += (rhs.second)[idx] * _base[idx];
            lhsVectorLength += pow((lhs.second)[idx], 2);
            rhsVectorLength += pow((rhs.second)[idx], 2);
        }
        //如果左边小于右边就交换；return true
        //如果左边大于右边就不交换：return false
        if (lhsCrossProduct / sqrt(lhsVectorLength) < rhsCrossProduct / sqrt(rhsVectorLength))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    vector<double> _base;
};



WordQuery::WordQuery(Configuration &conf):_conf(conf), _redisCache(_conf)
{
    loadLibrary();
}

bool WordQuery::executeQuery(const vector<string> &baseWord, vector<pair<int, vector<double>>> &resultVec)
{
    if (baseWord.size() == 0)
    {
        cout << "empty base word" << endl;
        return false;
    }
    typedef vector<pair<int, double>>::iterator vecIter;
    //在倒排索引内每个单词对应vector的迭代器和int当前迭代器指向的位置
    vector<pair<vecIter, int>> iterVec;
    //int 所能表示的最大正数；
    int minIterNum = 0x7FFFFFFF;
    

    for (auto item : baseWord)
    {
        int size = _invertIndexTable[item].size();
        if (0 == size)
        {
            //代表在倒排索引里面没有找到该单词；
            return false;
        }
        if (minIterNum > size)
        {
            minIterNum = size;
        }
        iterVec.push_back(make_pair(_invertIndexTable[item].begin(), 0));
    }
    bool isExiting = false;
    while(!isExiting)
    {
        int idx = 0;
        for(; idx != static_cast<int>(iterVec.size() - 1); ++idx)
        {
            //前面一个单词的docid和后一个单词的docid不一样就break
            if (iterVec[idx].first->first != iterVec[idx+1].first->first)
            {
                break;
            }
        }
        if (idx == static_cast<int>(iterVec.size() - 1))
        {
            //找到了所有单词相同的docid
            
            vector<double> weightVec;//包含所有单词的权重；docid所对应的一个向量
            int docid = iterVec[0].first->first;
            for (idx = 0; idx != static_cast<int>(iterVec.size()); ++idx)
            {
                weightVec.push_back(iterVec[idx].first->second);
                ++(iterVec[idx].first);//记录下权重以后将迭代器++
                ++(iterVec[idx].second);//迭代器所指的位置也++;
                if (iterVec[idx].second == minIterNum)
                {
                    isExiting = true;
                }
                resultVec.push_back(make_pair(docid, weightVec));
            }
        }
        else
        {
            //docid不相等，找到最小的docid将它的迭代器++
            int minDocid = 0x7FFFFFFF;
            int iterIdx;
            for(idx = 0; idx != static_cast<int>(iterVec.size()); ++idx)
            {
                if (iterVec[idx].first->first < minDocid)
                {
                    minDocid = iterVec[idx].first->first;
                    iterIdx = idx;
                }
            }
            ++(iterVec[iterIdx].first);
            ++(iterVec[iterIdx].second);
            if(iterVec[iterIdx].second == minIterNum)
            {
                isExiting = true;
            }
        }
    }
    return true;
}
    
vector<double> WordQuery::getQueryWordsWeightVector(map<string, int> &mapWordFreq, const vector<string> &baseWord)
{
    //计算 w = tf * idf;
    vector<double> baseVector;
    double totalPageNum = static_cast<double>(_pageLib.size());
    double weightSum = 0;
    for (const auto &word : baseWord)
    {
        double df = _invertIndexTable[word].size();
        double idf = log(totalPageNum/df);
        double tf = mapWordFreq[word];
        double w = tf * idf;
        weightSum += pow(w,2);
        baseVector.push_back(w);
    }

    for (auto &w : baseVector)
    {
        w /= sqrt(weightSum);
    }
    return baseVector;
}



string WordQuery::doQuery(const string &msg)
{
    //查询redis有的话直接得到Json格式的字符串返回
    string result;
    if (_redisCache.getValue(msg, result))
    {
        cout << msg << "-->Find in Redis Cache" << endl;
        return result;
    }

    //每个元素: docid 和包含关键字的向量值
    vector<pair<int, vector<double>>> vecAllVector;
    //基向量
    vector<double> baseVector;
    map<string, int> *pMsgWordFreq;
    //对msg进行分词和统计词频
    WebPage msgWebPage(msg);
    msgWebPage.constructWordMap(_conf);
    pMsgWordFreq = msgWebPage.getWordsMapPtr();
    //将单词保存到一个vector里面，以后才知道对应的单词对应什么权重
    vector<string> baseWord;
    auto mapIt = pMsgWordFreq->begin();
    while (mapIt != pMsgWordFreq->end())
    {
        baseWord.push_back(mapIt->first);
        ++mapIt;
    }

    //查看分好的单词；
    {
        cout << "split word:" << endl;
        for (const auto &i : baseWord)
        {
            cout << i << endl;
        }
    }

    //计算msg分词的归一化后的权重；
    baseVector= getQueryWordsWeightVector(*pMsgWordFreq, baseWord);

    //打印基向量的权重;
    {
        cout << "base Vector:" << endl;
        for (const auto &vec : baseVector)
        {
            cout << vec << " ";
        }
        cout << endl;
    }
    
    SimilarityCompare similarityCmp(baseVector);
    //计算得到倒排索引中包含msg分词的向量值；
    if (executeQuery(baseWord, vecAllVector))
    {
        //将向量和基向量比较，大的放在vector前面；
        stable_sort(vecAllVector.begin(), vecAllVector.end(), similarityCmp);
        vector<int> docidVec;
        for (auto item : vecAllVector)
        {
            docidVec.push_back(item.first);
        }
        result = createJson(docidVec, baseWord);
        _redisCache.set(msg, result);
        return result;
    }
    else
    {
        cout << "No result" << endl;
        return returnNoAnswer();
    }
}


string WordQuery::returnNoAnswer()
{
    Json::Value root;
    Json::Value arr;

    Json::Value elem;
    elem["title"] = "404 Not Found";
    elem["summary"] = "Nothing here";
    elem["url"] = "";
    arr.append(elem);
    root["file"] = arr;
    Json::StyledWriter writer;
    return writer.write(root);
}


string WordQuery::createJson(vector<int> &docidVec, const vector<string> &queryWords)
{
    Json::Value root;
    Json::Value arr;

    int cnt = 0;
    for (auto id : docidVec)
    {
        string summary = _pageLib[id].summary(queryWords);
        string title = _pageLib[id].getTitle();
        string url = _pageLib[id].getUrl();

        Json::Value elem;
        elem["title"] = title;
        elem["summary"] = summary;
        elem["url"] = url;
        arr.append(elem);
        if (++cnt == 100)
        {
            break;
        }
    }
    root["files"] = arr;
    Json::StyledWriter writer;
    return writer.write(root);
}



void WordQuery::loadLibrary()
{
    string newOffsetPath = _conf.getConfigMap()["newOffsetPath"];
    string newRipepagePath = _conf.getConfigMap()["newRipepagePath"];
    string invertIndexPath = _conf.getConfigMap()["invertIndexPath"];

    ifstream offsetIfs(newOffsetPath);
    string offsetLine;
    //读取_offsetLib;
    while (getline(offsetIfs, offsetLine))
    {
        int docid;
        int offset;
        int length;
        sscanf(offsetLine.c_str(), "%d %d %d", &docid, &offset, &length);
        _offsetLib[docid] = make_pair(offset, length);
    }
    cout << "Finish read Offset" << endl;
    //构建_vecPageLib
    auto mapIt = _offsetLib.begin();
    ifstream ripePage(newRipepagePath);
    istreambuf_iterator<char> beg(ripePage);
    istreambuf_iterator<char> end;
    string wholePage(beg, end);
    while (mapIt != _offsetLib.end())
    {
        int docid = mapIt->first;

        int offset = mapIt->second.first;
        int length = mapIt->second.second;
        string txt = wholePage.substr(offset, length);
        //WebPage不需要进行统计单词
        //但是要做去标签之类的；
        WebPage newWebPage(txt, offset);
        newWebPage.processDoc();
        _pageLib.insert(make_pair(docid, newWebPage));
        ++mapIt;
    }

    cout << "Finsih construct WebPage" << endl;
    //构建倒排索引表；_invertIndexTable
    //读取一行后先：
    ifstream invertTableIfs(invertIndexPath);
    string wordLine;
    while (getline(invertTableIfs, wordLine))
    {
        istringstream wordIss(wordLine);
        string word;
        wordIss >> word;
        int docid;
        double weight;
        while (wordIss >> docid)
        {
            wordIss >> weight;
            _invertIndexTable[word].push_back(make_pair(docid, weight));
        }
    }
    cout << "Finish read invert index table" << endl;

}





