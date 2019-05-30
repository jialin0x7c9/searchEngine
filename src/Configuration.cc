#include "../include/Configuration.h"
#include "../include/Mylogger.h"
#include <fstream>
#include <sstream>

using std::ifstream;
using std::istringstream;


Configuration::Configuration(const string &filepath):_filepath(filepath)
{
    //将配置文件conf的内容读入到map里面
    ifstream ifs(_filepath);
    if (!ifs.is_open())
    {
        LogError("'%s' open failed", _filepath.c_str());
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string key;
        string value;
        iss >> key;
        iss >> value;
        _configMap[key] = value;
    }
    //读入停用词
    {
        ifstream ifs(_configMap["stopWord"]);
        string stopWord;
        while (ifs >> stopWord)
        {
            _stopWordList.insert(stopWord);
        }
    }
}

set<string> Configuration::getStopWordList()
{
    return _stopWordList;
}

map<string, string>& Configuration::getConfigMap()
{
    return _configMap;
}

