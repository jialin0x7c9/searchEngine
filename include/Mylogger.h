#ifndef __MYLOGGER__
#define __MYLOGGER__
#include <log4cpp/Category.hh>
#include <string>
using std::string;

//c_str() c风格字符串; 即:"xxx"
#define prefix(msg, ...) string("[").append(__FILE__).append(":").append(__FUNCTION__).append(":").append(std::to_string(__LINE__)).append("] ").append(msg).c_str(), ##__VA_ARGS__
#define LogError(msg, ...) Mylogger::getInstance()->error(prefix(msg, ## __VA_ARGS__))
#define LogInfo(msg, ...) Mylogger::getInstance()->info(prefix(msg, ## __VA_ARGS__))
#define LogWarn(msg, ...) Mylogger::getInstance()->warn(prefix(msg, ## __VA_ARGS__))
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(prefix(msg, ## __VA_ARGS__))

class Mylogger
{
public:
    static Mylogger * getInstance();
    static void destroy();

    template<typename ...Args>
        void error(Args...args);

    template<typename ...Args>
        void info(Args...args);

    template<typename ...Args>
        void warn(Args...args);

    template<typename ...Args>
        void debug(Args...args);
private:
    //单例模式要求，封装构造函数和析构函数；
    Mylogger();
    ~Mylogger();

private:
    static Mylogger *_pInstance;
    //记录器
    log4cpp::Category &_cat;
};

    template<typename ...Args>
void Mylogger::info(Args...args)
{
    _cat.info(args...);
}
    template<typename ...Args>
void Mylogger::error(Args...args)
{
    _cat.error(args...);
}

    template<typename ...Args>
void Mylogger::warn(Args...args)
{
    _cat.warn(args...);
}
    template<typename ...Args>
void Mylogger::debug(Args...args)
{
    _cat.debug(args...);
}

#endif
