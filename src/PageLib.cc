#include "../include/PageLib.h"
#include "../include/tinyxml2.h"
#include <regex>
#include <fstream>
#include <sstream>
#include "../include/Mylogger.h"
using std::stringstream;
using namespace tinyxml2;
using std::ofstream;
using std::regex;
using std::make_pair;
using std::endl;
using std::cout;

PageLib::PageLib(Configuration &conf, DirScanner &dirScanner):
    _conf(conf),
    _dirScanner(dirScanner)
{

}


void PageLib::create()
{
    string rssPath = _conf.getConfigMap()["RSSyuliaoPath"];
    string ripepage = _conf.getConfigMap()["ripepagePath"];
    _dirScanner(rssPath);
    vector<string> vecRssPath = _dirScanner.getFilesPath();
    ofstream ofs(ripepage);
    int docid = 0;
    int offset = 0;
    for (const auto &path : vecRssPath)
    {
        cout << "handling: " << path << endl;
        XMLDocument doc;
        int ret = doc.LoadFile(path.c_str());
        if (ret)
        {
            LogError("%s LoadFile failed", path.c_str());
        }
        XMLElement * rssNode = doc.FirstChildElement("rss");
        if (rssNode == NULL)
        {
            LogError("%s rss is null", path.c_str());
        }
        XMLElement * itemNode = rssNode->FirstChildElement("channel")->FirstChildElement("item");
        if (NULL == itemNode)
        {
            LogError("%s itenNode is null", path.c_str());
            return;
        }
        //每个XML的所有文字进行处理；
        do
        {
            XMLElement * titleNode = itemNode->FirstChildElement("title");
            XMLElement * linkNode = itemNode->FirstChildElement("link");
            XMLElement * descriptNode = itemNode->FirstChildElement("description");
            XMLElement * contentNode = itemNode->FirstChildElement("content:encoded");
            const char * content = NULL;
            if (NULL == contentNode)
            {
                content = descriptNode->GetText();
            }
            else
            {
                content = contentNode->GetText();
            }

            const char * title = titleNode->GetText();
            const char * url = linkNode->GetText();
            if (title == NULL || url == NULL || content == NULL)
            {
                continue;
            }
            ++docid;
            string sdocid;
            stringstream ss;
            ss << docid;
            ss >> sdocid;
            string cont(content);
            regex re("<.*?>");
            string editContent = std::regex_replace(cont, re, string(""));
            string txt = 
                "<doc>\n\t<docid>" + sdocid + "</docid>\n" 
                + "\t<url>" + url + "</url>\n" 
                + "\t<title>" + title + "</title>\n"
                + "\t<content>" + editContent + "</content>\n</doc>\n";

            _vecFiles.push_back(txt);

            _offsetLib[docid] = make_pair(offset, txt.length());
            offset += txt.length();
        }while(NULL!=(itemNode = itemNode->NextSiblingElement()));

    }
    ofs.close();
}

void PageLib::store()
{
    string ripepagePath = _conf.getConfigMap()["ripepagePath"];
    string offsetPath = _conf.getConfigMap()["offsetPath"];
    ofstream ofs(ripepagePath);
    for (const auto &i : _vecFiles)
    {
        ofs << i;
    }
    ofs.close();
    ofs.open(offsetPath);
    auto itBegin = _offsetLib.begin();
    while(itBegin != _offsetLib.end())
    {
        ofs << itBegin->first;
        ofs << " ";
        ofs << itBegin->second.first;
        ofs << " ";
        ofs << itBegin->second.second;
        ofs << "\n";
        ++itBegin;
    }
    ofs.close();
}


