#include <vector>
#include "WebPage.h"
#include "tinyxml2.h"
#include "deps/cppjieba/Jieba.hpp"

using std::vector;
using std::endl;
using std::cout;

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
    XMLElement* contentNode = rootElement->FirstChildElement("content");
    _docid = atoi(docidNode->GetText());
    _docUrl = urlNode->GetText();
    _docTitle = titleNode->GetText();
    _docContent = contentNode->GetText();
    /* cout << "----WebPage:----" << endl; */
    /* cout << "docid =" << _docid << endl; */
    /* cout << "link =" << _docUrl << endl; */
    /* cout << "content = " << endl; */
    /* cout << _docContent << endl; */
    constructWordMap();
}

void WebPage::constructWordMap()
{
    vector<string> cutWord;
    set<string> stopWordList = _conf.getStopWordList();
    //对_docContent进行分词并统计；
    const char* const DICT_PATH = "../include/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "../include/dict/hmm_model.utf8";
    cppjieba::MixSegment mSeg(DICT_PATH,HMM_PATH);
    /* cppjieba::Jieba jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH); */
    /* jieba.CutForSearch(_docContent, cutWord); */
    mSeg.Cut(_docContent, cutWord);
    for (const auto &i : cutWord)
    {
        //如果是停用词则不加入单词的map里面；
        if (stopWordList.end() == stopWordList.find(i) && i != "\n" && i != " ") 
        {
            ++_wordsMap[i];
        }
    }
}
