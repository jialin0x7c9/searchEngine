//#include "../include/Configuration.h"
//#include "../include/DirScanner.h"
//#include "../include/PageLib.h"

#include "Configuration.h"
#include "DirScanner.h"
#include "PageLib.h"
#include "PageLibPreprocessor.h"
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;


int main()
{
    string confPath("../conf");
    Configuration conf(confPath);
    //offline--------------------------------------
    /* DirScanner dirScanner(conf); */
    /* PageLib pageLib(conf, dirScanner); */
    /* pageLib.create(); */
    /* pageLib.store(); */
    /* PageLibPreprocessor preProcess(conf); */
    /* preProcess.doProcess(); */
    //offlien---------------------------------------
    
    //online

    return 0;
}

