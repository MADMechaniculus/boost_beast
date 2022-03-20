#ifndef USERGETPROC_H
#define USERGETPROC_H

#include <boost/json/src.hpp>

#include "abstractgetproc.h"
#include "eventloopapplication.h"

class UserAbstractGET : public virtual AbstractGETProc {
    EventLoopApplication * appPtr = nullptr;
public:
    UserAbstractGET(EventLoopApplication * app) : AbstractGETProc(), appPtr(app) {}
    UserAbstractGET() : AbstractGETProc() {}

    /**
     * @brief Присоединение приложения к обработчику
     * @param app Указатель на приложение
     */
    void connectApp(EventLoopApplication * app) {
        this->appPtr = app;
    }
};

class CustomGETProcessor : public UserAbstractGET
{
    EventLoopApplication * appPtr = nullptr;
public:
    CustomGETProcessor(EventLoopApplication * app) : UserAbstractGET(app), appPtr(app) {}
    CustomGETProcessor() : UserAbstractGET() {}

    processorProcRet_t process(std::string target, http::file_body::value_type & ansBody) override {

        if (appPtr == nullptr)
            return {false, "nullptr application"};

        (void)appPtr;
        (void)ansBody;

        processorProcRet_t ret;
        ret.first = false;

        std::cout << "Call from " << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "Target: " << target << std::endl;
        return ret;   // always false, for default static response
    }
};

#endif // USERGETPROC_H
