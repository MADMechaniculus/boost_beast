#include "userpostproc.h"


CustomPOSTProcessor::CustomPOSTProcessor(EventLoopApplication *app) : UserAbstractPOST(app) {}

CustomPOSTProcessor::CustomPOSTProcessor() : UserAbstractPOST() {}

processorProcRet_t CustomPOSTProcessor::process(std::string targetJson, http::string_body::value_type &ansBody) {

    if (this->appPtr == nullptr)
        return {false, "nullptr application"};

    processorProcRet_t ret;
    ret.first = false;

    boost::json::object answerJson;
    boost::json::object obj;
    boost::json::value value = boost::json::parse(targetJson);

    obj = value.as_object();

    answerJson.emplace(JSON_FIELDS::fieldName_RetCode, boost::json::value((int64_t)-1));

    EventLoopApplication::appDescription_t appDescription;

    pushResult_t pushReturn;

    if (obj["requestedFuncIndex"].is_int64()) {
        switch (obj["requestedFuncIndex"].as_int64()) {
        case 0:
            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getAppDescription, appPtr, std::ref(answerJson)));
            break;
        case 32:
            answerJson.emplace(JSON_FIELDS::fieldName_FsModel, boost::json::array());
            answerJson[JSON_FIELDS::fieldName_FsModel].as_array().emplace_back(boost::json::object(\
                                                                                   {\
                                                                                       {JSON_FIELDS::fieldName_Id, 0},\
                                                                                       {JSON_FIELDS::fieldName_Name, "./files"},\
                                                                                       {JSON_FIELDS::fieldName_IsAbsolute, false},\
                                                                                       {JSON_FIELDS::fieldName_Childrens, boost::json::array()}\
                                                                                   }));
            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getFileStruct, appPtr, "./files", std::ref(answerJson), nullptr));
            break;
        default:
            answerJson[JSON_FIELDS::fieldName_RetCode] = (int64_t)-1;
            break;
        }

        if (pushReturn.first == false) {
            ret.first = false;
            answerJson[JSON_FIELDS::fieldName_RetCode] = (int64_t)-1;
        } else {
            pushReturn.second.get();
            answerJson[JSON_FIELDS::fieldName_RetCode] = (int64_t)0;
        }

        ret.second = boost::json::serialize(answerJson);
        ansBody = ret.second;
        ret.first = true;

        return ret;
    }
    return ret;
}
