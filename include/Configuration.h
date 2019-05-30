#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <iostream>
#include <map>
#include <set>

using std::map;
using std::string;
using std::set;

class Configuration
{
public:
    Configuration(const string &filepath);
    map<string, string>& getConfigMap();
    set<string> getStopWordList();
private:
    string _filepath;
    map<string,string> _configMap;
    set<string> _stopWordList;
};


#endif
