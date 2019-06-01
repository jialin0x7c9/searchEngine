#include <vector>
#include "WebPage.h"
#include "tinyxml2.h"
#include "deps/cppjieba/Jieba.hpp"

using std::vector;

using namespace tinyxml2;

WebPage::WebPage(string &doc, Configuration &conf): _conf(conf), _doc(doc)
{
    processDoc();
}

int WebPage::getDocid() const
{
    return _docid;
}

string WebPage::getContent() const
{
    return _docContent;
}

map<string, int>* WebPage::getWordsMapPtr() 
{
    return &_wordsMap;
}

void WebPage::processDoc()
{
    //去标签
    //提取docid、标题、url、内容
    XMLDocument *myDocument = new XMLDocument();
    myDocument->Parse(_doc.c_str());
    XMLElement* rootElement = myDocument->RootElement();
    XMLElement* docidNode = rootElement->FirstChildElement("docid");
    XMLElement* urlNode = rootElement->FirstChildElement("url");
    XMLElement* titleNode = rootElement->FirstChildElement("title");
    _docid = atoi(docidNode->GetText());
    _docUrl = urlNode->GetText();
    _docContent = titleNode->GetText();
    constructWordMap();
}

void WebPage::constructWordMap()
{
    vector<string> cutWord;
    //对_docContent进行分词并统计；
    const char* const DICT_PATH = "../include/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "../include/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "../include/dict/user.dict.utf8";
    cppjieba::Jieba jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH);
    jieba.CutForSearch(_docContent, cutWord);
    for (const auto &i : cutWord)
    {
        //如果是停用词则不加入单词的map里面；
        if (_conf.getStopWordList().end()!= _conf.getStopWordList().find(i))
        {
            _wordsMap[i]++;
            std::cout << i << std::endl;
        }
    }
    
}
