
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include "../include/Mylogger.h"

#include <iostream>
using std::cout;
using std::endl;

//饿汉模式；
Mylogger * Mylogger::_pInstance = nullptr;

Mylogger * Mylogger::getInstance()
{
    if (!_pInstance)
    {
        _pInstance = new Mylogger();
    }
    return _pInstance;
}

void Mylogger::destroy()
{
    if (_pInstance)
    {
        delete _pInstance;
    }
}

Mylogger::Mylogger():
    _cat(log4cpp::Category::getRoot().getInstance("logger"))
{
    using namespace log4cpp;
    PatternLayout *ptn1 = new PatternLayout();
    ptn1->setConversionPattern("%d %c [%p] %m%n");
    PatternLayout *ptn2 = new PatternLayout();
    ptn2->setConversionPattern("%d %c [%p] %m%n");

    OstreamAppender * os = new OstreamAppender("os", &cout);
    os->setLayout(ptn1);

    RollingFileAppender * rollfile = new RollingFileAppender("rollfile", "../log/log", 10*1024*1024,10);
    rollfile->setLayout(ptn2);

    _cat.addAppender(os);
    _cat.addAppender(rollfile);
    _cat.setPriority(Priority::DEBUG);
}

Mylogger::~Mylogger()
{
    log4cpp::Category::shutdown();
}


