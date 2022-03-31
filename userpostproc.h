#ifndef USERPOSTPROC_H
#define USERPOSTPROC_H

#include "abstractpostproc.h"
#include "eventloopapplication.h"

class UserAbstractPOST : public virtual AbstractPOSTProc {
protected:
    EventLoopApplication * appPtr = nullptr;
public:
    UserAbstractPOST(EventLoopApplication * app) : AbstractPOSTProc(), appPtr(app) {}
    UserAbstractPOST() : AbstractPOSTProc() {}

    /**
     * @brief Присоединение приложения к обработчику
     * @param app Указатель на приложение
     */
    void connectApp(EventLoopApplication * app) {
        this->appPtr = app;
    }
};

class CustomPOSTProcessor : public UserAbstractPOST
{
public:
    CustomPOSTProcessor(EventLoopApplication * app);
    CustomPOSTProcessor();

    processorProcRet_t process(std::string targetJson, http::string_body::value_type & ansBody) override;
};

#endif // USERPOSTPROC_H
