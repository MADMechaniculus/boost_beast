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
    CustomPOSTProcessor(EventLoopApplication * app) : UserAbstractPOST(app) {}
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

        auto wrapperOpenSwitch = [&]() -> void {
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

        auto wrapperCloseSwitch = [&]() -> void {
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

        auto wrapperSetVoltage = [&]() -> void {
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

        auto wrapperGetAppDescription = [&]() -> void {
            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getAppDescription, appPtr, std::ref(appDescription)));

            if (!pushReturn.first) {
                ret.first = false;
                answerJson["retCode"] = (int64_t)-1;
            } else {
                pushReturn.second.get();
                answerJson["appVersion"] = { {"major", appDescription.version.first}, {"minor", appDescription.version.second} };
                answerJson["boostVersion"] = appDescription.boostVersion;
                answerJson["appName"] = appDescription.appName;
                answerJson["retCode"] = (int64_t)0;
            }
        };

        auto wrapperGetFileStruct = [&]() -> void {
            answerJson.emplace(JSON_FIELDS::fieldName_FsModel, boost::json::array());
            answerJson[JSON_FIELDS::fieldName_FsModel].as_array().emplace_back(boost::json::object(\
                                                                                   {\
                                                                                       {JSON_FIELDS::fieldName_Id, 0},\
                                                                                       {JSON_FIELDS::fieldName_Name, "./files"},\
                                                                                       {JSON_FIELDS::fieldName_IsAbsolute, false},\
                                                                                       {JSON_FIELDS::fieldName_Childrens, boost::json::array()}\
                                                                                   }));

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getFileStruct, appPtr, "./files", std::ref(answerJson), nullptr));

            if (pushReturn.first == false) {
                ret.first = false;
                answerJson["retCode"] = (int64_t)-1;
            } else {
                pushReturn.second.get();
                answerJson[JSON_FIELDS::fieldName_RetCode] = (int64_t)0;
            }
        };

        if (obj["requestedFuncIndex"].is_int64()) {
            switch (obj["requestedFuncIndex"].as_int64()) {
            case 0:
                wrapperGetAppDescription();
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
            case 32:
                wrapperGetFileStruct();
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
