#include "../include/SpellCorrectServer.h"
#include "../include/DictProducer.h"
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

int main()
{
    string confPath("../conf");
    Configuration conf(confPath);
    /* string yuliaoPath = conf.getConfigMap()["yuliaoPath"]; */
    /* string dictionaryPath = conf.getConfigMap()["dictionaryPath"]; */
    /* string yuliaoZhPath = conf.getConfigMap()["yuliaoZhPath"]; */
    /* string dictionaryZhPath = conf.getConfigMap()["dictionaryZhPath"]; */
    /* //对原始的语料库进行处理，去除标点符号…… */
    /* DictProducer dictP(yuliaoPath, yuliaoZhPath); */
    /* //建立词典：单词和词频 */
    /* dictP.buildDict(); */
    /* //建立中文词典：单词和词频 */
    /* //并保存到了磁盘； */
    /* dictP.buildStoreCnDict(dictionaryZhPath.c_str()); */

    /* //将词典保持到磁盘中； */
    /* dictP.storeDict(dictionaryPath.c_str()); */
    //以上：为离线部分
    //一下：为在线部分；
    
    //会从词典的存放路径里面读取词典并构造索引;
    SpellCorrectServer spellSer(conf);
    spellSer.start();
    return 0;
}

