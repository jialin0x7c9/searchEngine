#ifndef __DIRSCANNER_H__
#define __DIRSCANNER_H__
#include "Configuration.h"
#include <vector>
using std::vector;

class DirScanner
{
public:
    DirScanner(Configuration &conf);
    void operator()(const string &dirName);
    void traverse(const string &dirName);
    vector<string> getFilesPath();
private:
    vector<string> _vecFilesPath;
    Configuration & _conf;
};



#endif
