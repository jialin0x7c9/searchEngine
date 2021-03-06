#include "../include/DirScanner.h"
#include "../include/Mylogger.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>



DirScanner::DirScanner(Configuration &conf):_conf(conf)
{

}

void DirScanner::operator()(const string &dirName)
{
    traverse(dirName);
}

void DirScanner::traverse(const string &dirName)
{
    DIR *fDir = opendir(dirName.c_str());
    if (NULL == fDir)
    {
        LogError("opendir %s failed.", dirName);
    }
    struct dirent *pDirent;
    while (NULL != (pDirent = readdir(fDir)))
    {
        if (strcmp(pDirent->d_name, ".")==0 || strcmp(pDirent->d_name, "..")==0)
        {
            continue;
        }
        string filePath;
        filePath = dirName + "/" + pDirent->d_name;
        _vecFilesPath.push_back(filePath);
    }
}

vector<string> DirScanner::getFilesPath()
{
    return _vecFilesPath;
}

