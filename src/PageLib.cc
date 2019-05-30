#include "../include/PageLib.h"
#include "../include/tinyxml2.h"
#include <regex>
#include <fstream>

using namespace tinyxml2;
using std::ofstream;

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
    for (const auto &path : vecRssPath)
    {
        XMLDocument doc;
        doc.LoadFile(path.c_str());
        XMLElement * rssNode = doc.FirstChildElement("rss");
        XMLElement * itemNode = rssNode->FirstChildElement("channel")->FirstChildElement("item");
        if (itemNode != nullptr)
        {
            XMLElement * titleNode = itemNode->FirstChildElement("title");
            XMLElement * linkNode = itemNode->FirstChildElement("link");
            XMLElement * contentNode = itemNode->FirstChildElement("content:encoded");
            const char * title = titleNode->GetText();
            const char * url = linkNode->GetText();
            const char * content = contentNode->GetText();
            string txt = 
                "<doc>\n<docid>" + docid + "</docid>\n" 
                + "<url>" + url + "</url>\n" 
                + "<title>" + title + "</title>\n"
                + "<content>" + content + "</content>\n</doc>\n";
        }
    }

}
