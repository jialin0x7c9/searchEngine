#include "WebPage.h"
#include <vector>
#include <sstream>
#include "tinyxml2.h"
#include "deps/cppjieba/Jieba.hpp"

using std::vector;
using std::endl;
using std::cout;
using std::istringstream;

using namespace tinyxml2;

WebPage::WebPage(string &doc, int offset): 
    _doc(doc),
    _offset(offset)
{
    /* processDoc(); */
}

WebPage::WebPage(string content):
    _docContent(content)
{

}

string WebPage::getTitle() const
{
    return _docTitle;
}

string WebPage::summary(const vector<string> &queryWords)
{
    vector<string> summaryVec;
    istringstream iss(_docContent);
    string line;
    while(iss >> line)
    {
        for (auto word : queryWords)
        {
            //如果某行有这个单词，则保存这个行；
            if (line.find(word) != string::npos)
            {
                summaryVec.push_back(line);
                break;
            }
        }

        if (summaryVec.size() >= 5)
        {
            break;
        }
    }

    string summary;
    for (auto line : summaryVec)
    {
        summary.append(line).append("\n");
    }
    return summary;
}

string WebPage::getUrl() const
{
    return _docUrl;
}

void WebPage::calSimhash()
{
    simhash::Simhasher simhasher("../include/dict/jieba.dict.utf8", "../include/dict/hmm_model.utf8", "../include/dict/idf.utf8", "../include/dict/stop_words.utf8");
    /* cout << "----------cal simHash Content--------" << endl; */
    /* cout << _docContent << endl; */
    simhasher.make(_docContent, 15, _u64);
    /* cout << _u64 << endl; */
    /* cout << "------------------- " << endl; */
}

uint64_t WebPage::getSimhash()
{
    return _u64;
}
int WebPage::getDocid() const
{
    return _docid;
}

int WebPage::getOffset() const
{
    return _offset;
}

int WebPage::getLength() const 
{
    return _doc.length();
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
    if (rootElement == NULL)
    {
        cout << "NULL" << endl;
    }
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
}

void WebPage::constructWordMap(Configuration &conf)
{
    vector<string> cutWord;
    set<string> stopWordList = conf.getStopWordList();
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
