#include <fstream>
#include <sstream>
#include <cmath>
#include "PageLibPreprocessor.h"

using std::vector;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::make_pair;
using std::endl;
using std::cout;
using std::istreambuf_iterator;



PageLibPreprocessor::PageLibPreprocessor(Configuration &conf):_conf(conf)
{
}


void PageLibPreprocessor::storeInvertTableOnDisk()
{

    string invertTablePath = _conf.getConfigMap()["invertIndexPath"];
    ofstream invertITOfs(invertTablePath);
    auto invertTableIt =  _invertIndexTable.begin();
    while (invertTableIt != _invertIndexTable.end())
    {
        string word = invertTableIt->first;
        invertITOfs << word;
        invertITOfs << " ";
        for (auto &wordFreq : invertTableIt->second)
        {
            int docid = wordFreq.first;
            double weight = wordFreq.second;
            invertITOfs << docid;
            invertITOfs << " ";
            invertITOfs << weight;
            invertITOfs << " ";
        }
        invertITOfs.seekp(-1, invertITOfs.cur);
        invertITOfs << "\n";
        ++invertTableIt;
    }
}


void PageLibPreprocessor::buildInvertIndexTable()
{
    /* //构造vector<WebPage> */
    /* vector<WebPage> vecPage; */
    /* string newPagePath = _conf.getConfigMap()["newRipepagePath"]; */
    /* ifstream ifs(newPagePath); */
    /* auto it = _newOffsetLib.begin(); */
    /* while(it != _newOffsetLib.end()) */
    /* { */
    /*     int offset = it->second.first; */
    /*     int length = it->second.second; */
    /*     //读出一片文章； */
    /*     string txt(length, '\0'); */
    /*     ifs.seekg(offset, ifs.beg); */
    /*     ifs.read(&txt[0], length); */
    /*     /1* cout << "------<>txt:" << endl; *1/ */
    /*     /1* cout << txt << endl; *1/ */
    /*     //得到了带标签的txt */
    /*     /1* WebPage newWebPage(txt, _conf); *1/ */
    /*     /1* vecPage.push_back(newWebPage); *1/ */
    /*     it++; */
    /* } */
    //得到一个vector<WebPage>
    int totalPage = 0;
    _vecPage.clear();
    ifstream pageIfs(_conf.getConfigMap()["newRipepagePath"]);
    ifstream offsetIfs(_conf.getConfigMap()["newOffsetPath"]);
    istreambuf_iterator<char> begin(pageIfs);
    istreambuf_iterator<char> end;
    string wholePage(begin, end);
    string oneLine;
    while(getline(offsetIfs, oneLine))
    {
        istringstream iss(oneLine);
        int docid;
        int offset;
        int pageLength;
        iss >> docid;
        iss >> offset;
        iss >> pageLength;
        string txt = wholePage.substr(offset, pageLength);
        WebPage newWebPage(txt, offset);
        newWebPage.processDoc();
        newWebPage.constructWordMap(_conf);
        _vecPage.push_back(newWebPage);
    }

    totalPage = _vecPage.size();
    //遍历vector得到unordered_map<string, vector<pair<docid, 词频>>>
    for (auto &onePage : _vecPage) //遍历每一个WebPage，里面有文章的信息；
    {
        int docid = onePage.getDocid();
        const map<string, int>* wordsMap = onePage.getWordsMapPtr();
        auto it = wordsMap->begin();
        while(it != wordsMap->end())
        {
            string word = it->first;
            double wordFrequency = it->second;
            _invertIndexTable[word].push_back(make_pair(docid, wordFrequency));
            ++it;
        }
    }

    //查看unordered_map<string, vector<pair<docid, 词频>>
    /* { */
    /*     cout << "------查看docid 和 词频----------" << endl; */
    /*     auto tableIt = _invertIndexTable.begin(); */
    /*     while(tableIt !=  _invertIndexTable.end()) */
    /*     { */
    /*         string word = tableIt->first; */
    /*         cout << word; */
    /*         cout << " "; */
    /*         for (const auto &i : tableIt->second) */
    /*         { */
    /*             cout << i.first; */
    /*             cout << " "; */
    /*             cout << i.second; */
    /*             cout << ","; */
    /*         } */
    /*         cout << endl; */
    /*         ++tableIt; */
    /*     } */
    /*     cout << "---------------------------------" << endl; */
    /* } */

    //遍历一遍_invertIndexTable得到unordered_map<string, vector<pair<docid, 该词在该篇的权重>>>
    auto tableIt = _invertIndexTable.begin();
    while(tableIt !=  _invertIndexTable.end())
    {
        double totalOccursPage = tableIt->second.size(); //一共有多少篇文章出现了该单词；
        double idf = log(totalPage/totalOccursPage);
        for (auto &docFreq : tableIt->second)
        {
            //词在该篇的权重 w = tf * idf；(idf = log(N/出现该词的总次数))
            double timesInOnePage = docFreq.second;
            double wInOnePage = timesInOnePage * idf; 
            docFreq.second = wInOnePage;
        }
        ++tableIt;
    }
    //查看unordered_map<string, vector<pair<docid, 本篇权重>>
    /* { */
    /*     cout << "------查看docid 和 本篇权重----------" << endl; */
    /*     auto tableIt = _invertIndexTable.begin(); */
    /*     while(tableIt !=  _invertIndexTable.end()) */
    /*     { */
    /*         string word = tableIt->first; */
    /*         cout << word; */
    /*         cout << " "; */
    /*         for (const auto &i : tableIt->second) */
    /*         { */
    /*             cout << i.first; */
    /*             cout << " "; */
    /*             cout << i.second; */
    /*             cout << ","; */
    /*         } */
    /*         cout << endl; */
    /*         ++tableIt; */
    /*     } */
    /*     cout << "---------------------------------" << endl; */
    /* } */

    //再遍历一遍得到unordered_map<string, vector<pair<docid, 归一化后的权重>>>
    tableIt = _invertIndexTable.begin();
    while(tableIt != _invertIndexTable.end())
    {
        //计算分母；sqrt(w1^2 + w2^2 +...+ wn^2)
        double totalWeight = 0;
        for (auto &docFreq : tableIt->second)
        {
            double w = docFreq.second;
            totalWeight += w*w;
        }
        double sqrtTotalWeight = sqrt(totalWeight);
        //计算归一化后的权重;
        for (auto &docFreq : tableIt->second)
        {
            if (0 == sqrtTotalWeight)
            {
                docFreq.second = 0;
            }
            else
            {
                docFreq.second /= sqrtTotalWeight;
            }
        }
        ++tableIt;
    }
}

void PageLibPreprocessor::storeNewPageAndOffsetOnDisk()
{
    cout << "Begin newPageAndOffset store." << endl;
    string newRipepagePath = _conf.getConfigMap()["newRipepagePath"];
    string newOffsetPath = _conf.getConfigMap()["newOffsetPath"];
    string oldRipepagePath = _conf.getConfigMap()["ripepagePath"];
    //写offset的时候就可以写new ripepage
    auto it = _newOffsetLib.begin();
    ofstream offsetOfs(newOffsetPath);
    ofstream pageOfs(newRipepagePath);
    ifstream oldPageIfs(oldRipepagePath);

    int pos = 0;
    while(it != _newOffsetLib.end())
    {
        int docid = it->first;
        int offset = it->second.first;
        int length = it->second.second;
        //docid 可以直接写入newOffsetPath
        /* offsetOfs << docid; */
        /* offsetOfs << " "; */
        
        //写offset和length的时候可以获得文章写入新的ripePage
        /* offsetOfs << offset; */
        /* offsetOfs << " "; */
        /* offsetOfs << length; */
        /* offsetOfs << "\n"; */

        //读出一片文章；
        string txt(length, '\0');
        oldPageIfs.seekg(offset, oldPageIfs.beg);
        oldPageIfs.read(&txt[0], length);
        pageOfs << txt;
        cout << "Finish Store docid= " << docid << endl;
        //写入newOffset.lib
        offsetOfs << docid;
        offsetOfs << " ";
        offsetOfs << pos;
        offsetOfs << " ";
        offsetOfs << length;
        offsetOfs << "\n";
        pos += length;

        it++;
    }
    pageOfs.close();
    offsetOfs.close();
    oldPageIfs.close();
    cout << "NewPageAndOffset store Complete." << endl;

}

void PageLibPreprocessor::readPageFromFile()
{
    ifstream pageIfs(_conf.getConfigMap()["ripepagePath"]);
    ifstream offsetIfs(_conf.getConfigMap()["offsetPath"]);
    istreambuf_iterator<char> begin(pageIfs);
    istreambuf_iterator<char> end;
    string wholePage(begin, end);
    string oneLine;
    while(getline(offsetIfs, oneLine))
    {
        istringstream iss(oneLine);
        int docid;
        int offset;
        int pageLength;
        iss >> docid;
        iss >> offset;
        iss >> pageLength;
        string txt = wholePage.substr(offset, pageLength);
        //WebPage不需要统计单词和词频;
        //但是要去标签；
        WebPage newWebPage(txt, offset);
        newWebPage.processDoc();
        cout << "finish WebPage: " << docid << endl;
        _vecPage.push_back(newWebPage);
    }
    cout << "finish all WebPage" << endl;
}

void PageLibPreprocessor::cutRedundantPage()
{
    //计算每一篇WebPage的simhash
    for (auto &onePage : _vecPage)
    {
        onePage.calSimhash();
        cout << "calSimhas docid =" << onePage.getDocid() << endl; 
    }
    //然后遍历vector将simhash放入一个新的vector内如果放的进则构建新索引
    vector<uint64_t> vecSimhash;
    for (auto &onePage : _vecPage)
    {
        uint64_t u64 = onePage.getSimhash();
        bool isSamePage = false;
        for (const auto &i : vecSimhash)
        {
            if (simhash::Simhasher::isEqual(u64, i))
            {
                cout << u64 << " and " << i << " is same" << endl;
                isSamePage = true;
                break;
            }
        }
        if (isSamePage)
        {
            continue;
        }
        vecSimhash.push_back(u64);
        _newOffsetLib[onePage.getDocid()] = make_pair(onePage.getOffset(), onePage.getLength());
    }
}

void PageLibPreprocessor::doProcess()
{
    //通过旧的offset将每篇文章构建一个WebPage存在vector内
    /* readPageFromFile(); */
    //遍历_vecPage将不多余的的WebPage组成_newOffsetLib；
    /* cutRedundantPage(); */
    //通过_newOffsetLib和旧的ripePage来存；
    /* storeNewPageAndOffsetOnDisk(); */

    buildInvertIndexTable();
    storeInvertTableOnDisk();
}

