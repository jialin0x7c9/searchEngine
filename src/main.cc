//#include "../include/Configuration.h"
//#include "../include/DirScanner.h"
//#include "../include/PageLib.h"

#include "Configuration.h"
#include "DirScanner.h"
#include "PageLib.h"
#include <iostream>
#include <string>
#include "simhash/Simhasher.hpp"
using std::cout;
using std::endl;
using std::string;

using namespace simhash;

int main()
{
    string confPath("../conf");
    Configuration conf(confPath);
    DirScanner dirScanner(conf);
    PageLib pageLib(conf, dirScanner);
    pageLib.create();
    pageLib.store();

    Simhasher simhasher("../include/dict/jieba.dict.utf8", "../include/dict/hmm_model.utf8", "../include/dict/idf.utf8", "../include/dict/stop_words.utf8");
    string s("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    size_t topN = 5;
    uint64_t u64 = 0;
    vector<pair<string ,double> > res;
    simhasher.extract(s, res, topN);
    simhasher.make(s, topN, u64);
    cout<< "文本：\"" << s << "\"" << endl;
    cout<< "simhash值是: " << u64<<endl;

    string s2("我是蓝翔技工计算机学院机器学习专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    uint64_t u64_2 = 0;
    vector<pair<string, double>> res2;
    simhasher.extract(s2, res2, topN);
    simhasher.make(s2, topN, u64_2);
    cout<< "simhash值是: " << u64_2<<endl;
    cout<< s << "和" << s2 << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为3，则isEqual结果为：" << (Simhasher::isEqual(u64, u64_2)) << endl; 



    return 0;
}

