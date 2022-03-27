#ifndef DEMOPOSTPROCESSOR_H
#define DEMOPOSTPROCESSOR_H

#include "abstractpostproc.h"
#include "demoapplication.h"

class DemoUserAbstractPOST : public virtual AbstractPOSTProc {
protected:
    DemoApplication * appPtr = nullptr;
public:
    DemoUserAbstractPOST(DemoApplication * app) : AbstractPOSTProc(), appPtr(app) {}
    DemoUserAbstractPOST() : AbstractPOSTProc() {}

    /**
     * @brief Присоединение приложения к обработчику
     * @param app Указатель на приложение
     */
    void connectApp(DemoApplication * app) {
        this->appPtr = app;
    }
};

class DemoPOSTProcessor : public DemoUserAbstractPOST
{
public:
    DemoPOSTProcessor(DemoApplication * ptr) : DemoUserAbstractPOST(ptr) {}
    DemoPOSTProcessor() : DemoUserAbstractPOST() {}

    processorProcRet_t process(std::string targetJson, http::string_body::value_type & ansBody) override {

        if (this->appPtr == nullptr)
            return {false, "nullptr application"};

        pushResult_t pushReturn;
        processorProcRet_t ret;
        ret.first = false;

        boost::json::object answerJson;

        boost::json::object obj;
        boost::json::value value = boost::json::parse(targetJson);
        obj = value.as_object();

        bool execRet{false};
        std::string direction{""};

        if (obj.contains("direction")) {
            if (obj["direction"].is_string()) {
                direction = obj["direction"].as_string().c_str();
            }
        }

        if (obj["requestedFuncIndex"].is_int64()) {
            switch (obj["requestedFuncIndex"].as_int64()) {
            case 0:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceGetMenuDescription, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 1:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceGetDevInfo, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 2:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceGetSettings, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 3:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceGetControls, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 4:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceGetInfo, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 5:
                pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceDoBeep, appPtr, std::ref(execRet), std::ref(answerJson)));
                break;
            case 6:
                if (!direction.empty()) {
                    pushReturn = appPtr->pushRequest(std::bind(&DemoApplication::ifaceDoMove, appPtr, std::ref(execRet), direction, std::ref(answerJson)));
                }
                break;
            default:
                answerJson["retCode"].as_int64() = (int64_t)-1;
                break;
            }

            if (pushReturn.first) {
                pushReturn.second.get();
                answerJson["retCode"].as_int64() = (int64_t)0;
            } else {
                answerJson["retCode"].as_int64() = (int64_t)-1;
            }

            ansBody = boost::json::serialize(answerJson);
            ret.first = true;

            return ret;
        }
        return ret;
    }
};

#endif // DEMOPOSTPROCESSOR_H
