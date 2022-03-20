#ifndef USERPOSTPROC_H
#define USERPOSTPROC_H

#include <boost/json/src.hpp>

#include "abstractpostproc.h"
#include "eventloopapplication.h"

class UserAbstractPOST : public virtual AbstractPOSTProc {
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
    EventLoopApplication * appPtr = nullptr;
public:
    CustomPOSTProcessor(EventLoopApplication * app) : UserAbstractPOST(app), appPtr(app) {}
    CustomPOSTProcessor() : UserAbstractPOST() {}

    processorProcRet_t process(std::string targetJson, http::string_body::value_type & ansBody) override {

        if (this->appPtr == nullptr)
            return {false, "nullptr application"};

        processorProcRet_t ret;
        ret.first = false;

        boost::json::object answerJson;

        boost::json::object obj;
        boost::json::value value = boost::json::parse(targetJson);
        obj = value.as_object();

        answerJson["retCode"] = 0;

        EventLoopApplication::appDescription_t appDescription;

        pushResult_t pushReturn;

        auto wrapperOpenSwitch = [&]() {
            int bank = obj["bank"].as_int64();
            int channel = obj["channel"].as_int64();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execOpenSwitch, appPtr, bank, channel, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        auto wrapperCloseSwitch = [&]() {
            int bank = obj["bank"].as_int64();
            int channel = obj["channel"].as_int64();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execCloseSwitch, appPtr, bank, channel, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        auto wrapperSetVoltage = [&]() {
            int bank = obj["bank"].as_int64();
            std::string voltage = obj["voltage"].as_string().c_str();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execSetVoltage, appPtr, bank, voltage, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        if (obj["requestedFuncIndex"].is_int64()) {
            switch (obj["requestedFuncIndex"].as_int64()) {
            case 0:

                pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getAppDescription, appPtr, std::ref(appDescription)));

                if (!pushReturn.first)
                    ret.first = false;

                pushReturn.second.get();

                answerJson["appVersion"] = { {"major", appDescription.version.first}, {"minor", appDescription.version.second} };
                answerJson["boostVersion"] = appDescription.boostVersion;
                answerJson["appName"] = appDescription.appName;
                answerJson["retCode"] = (int64_t)0;

                break;
            case 1:

                wrapperOpenSwitch();

                break;
            case 2:

                wrapperCloseSwitch();

                break;
            case 3:

                wrapperSetVoltage();

                break;
            default:
                answerJson["retCode"] = (int64_t)-1;
                break;
            }

            ret.second = boost::json::serialize(answerJson);
            ansBody = ret.second;
            ret.first = true;

            return ret;
        }
        return ret;
    }
};

#endif // USERPOSTPROC_H
