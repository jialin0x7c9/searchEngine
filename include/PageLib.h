#ifndef __PAGELIB_H__
#define __PAGELIB_H__
#include "Configuration.h"
#include "DirScanner.h"



#include <vector>
#include <map>

using std::vector;
using std::map;
using std::pair;

class PageLib
{
public:
    PageLib(Configuration &conf, DirScanner &dirScanner);
    void create();
    void store();
private:
    Configuration &_conf;
    DirScanner &_dirScanner;
    //存XML格式化后的文本内容
    vector<string> _vecFiles;
    //docid pair<offset, length>
    map<int, pair<int, int>> _offsetLib;
};


#endif
