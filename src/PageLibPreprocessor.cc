#include "PageLibPreprocessor.h"
#include <fstream>
#include <sstream>
#include "simhash/Simhasher.hpp"

using std::vector;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::make_pair;
using std::endl;
using std::cout;
using namespace simhash;

PageLibPreprocessor::PageLibPreprocessor(Configuration &conf):_conf(conf)
{
}

void PageLibPreprocessor::readAndCutRedundantPage()
{
    //读offset.lib文件；
    //记录docid
    //通过offset和length从ripepage.lib文件中读文章string
    //计算simhash
    //遍历vector看是否放入；
    //若放入则记录入map<int, pair<int, int>>中；
    
    vector<uint64_t> _vecNewFile;
    ifstream pageIfs(_conf.getConfigMap()["ripepagePath"]);
    ifstream offsetIfs(_conf.getConfigMap()["offsetPath"]);
    string oneLine;
    while(getline(offsetIfs, oneLine))
    {
        istringstream iss(oneLine);
        bool isSamePage = false;
        int docid;
        int offset;
        int pageLength;
        iss >> docid;
        iss >> offset;
        iss >> pageLength;
        pageIfs.seekg(offset, pageIfs.beg);
        string txt(pageLength, '\0');
        pageIfs.read(&txt[0], pageLength);
        //计算simhash
        Simhasher simhasher("../include/dict/jieba.dict.utf8", "../include/dict/hmm_model.utf8", "../include/dict/idf.utf8", "../include/dict/stop_words.utf8");
        uint64_t u64;
        simhasher.make(txt, 5, u64);
        cout << "docid = " << docid << " " << "offset = " << offset << " " << "pageLength = " << pageLength << endl;
        cout << "simhash= " << u64 << endl;
        cout << "txt = " << endl;
        cout << txt << endl;
        for (const auto &i : _vecNewFile)
        {
            //如果相等说明是同一片文章，不写入新索引
            if (Simhasher::isEqual(u64, i))
            {
                isSamePage = true;
                break;
            }
        }
        //如果是相同的文章，则继续读下一篇文章；
        if (isSamePage)
        {
            cout << "same page" << endl;
            continue;
        }
        //如果没有相同的文章，则将simhash值加入vector中；
        _vecNewFile.push_back(u64);
        _newOffsetLib[docid] = make_pair(offset, pageLength);
    }

    /* //测试新的map元素 */
    /* auto it = _newOffsetLib.begin(); */
    /* while (it != _newOffsetLib.end()) */
    /* { */
    /*     cout << it->first << " " << it->second.first << " " << it->second.second << endl; */
    /*     it++; */
    /* } */
}

void PageLibPreprocessor::storeOnDisk()
{
    string newRipepagePath = _conf.getConfigMap()["newRipepagePath"];
    string newOffsetPath = _conf.getConfigMap()["newOffsetPath"];
    string oldRipepagePath = _conf.getConfigMap()["ripepagePath"];
    //写offset的时候就可以写new ripepage
    auto it = _newOffsetLib.begin();
    ofstream offsetOfs(newOffsetPath);
    ifstream oldPageIfs(oldRipepagePath);
    ofstream pageOfs(newRipepagePath);

    while(it != _newOffsetLib.end())
    {
        int docid = it->first;
        int offset = it->second.first;
        int length = it->second.second;
        //docid 可以直接写入newOffsetPath
        offsetOfs << docid;
        offsetOfs << " ";
        
        //写offset和length的时候可以获得文章写入新的ripePage
        offsetOfs << offset;
        offsetOfs << " ";
        offsetOfs << length;

        //读出一片文章；
        string txt(length, '\0');
        oldPageIfs.seekg(offset, oldPageIfs.beg);
        oldPageIfs.read(&txt[0], length);

        pageOfs << txt;
        it++;
    }
}


void PageLibPreprocessor::doProcess()
{
    readAndCutRedundantPage();
    /* buildInvertIndexTable(); */
    storeOnDisk();
}

