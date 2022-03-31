#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/json.hpp>

#include <queue>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

#include "abstractapplication.h"
#include "defines.h"
#include "keysightcommand/keysightcommand.h"

namespace po = boost::program_options;

// Startup parameters =======================================
typedef struct {
    // Some params to put in init function
} initParams_t;

typedef struct {
    // Some params to put in process function
} processParams_t;

typedef struct {
    // Some params to put in stop function
} stopParams_t;
// ==========================================================

/**
 * @namespace JSON_FIELDS
 * @brief Пространство имён для определения полей в JSON описывающем файловую структуру
 */
namespace JSON_FIELDS {
const std::string fieldName_RetCode = "retCode";
const std::string fieldName_Id = "id";
const std::string fieldName_Name = "name";
const std::string fieldName_IsAbsolute = "isAbsolute";
const std::string fieldName_Childrens = "childrens";
const std::string fieldName_FsModel = "fs_model";
const std::string filedName_AppName = "appName";
const std::string filedName_AppVersion = "appVersion";

const std::string filedName_AppVersion_minor = "minor";
const std::string filedName_AppVersion_major = "major";

const std::string filedName_BoostVersion = "boostVersion";

const std::string fieldName_Error = "__err";
}

/**
 * @brief Класс пользовательского приложения.
 *
 * Данная реализация строится на основе бесконенчого цикла в основной рбочей функции.
 * В основе приложения лежит абстракция, позволяющая организовать очередь внешних запросов.
 *
 * Данное приложение реализует интерфейс управления оборудованием KeySight с использованием TCP и протокола SCPI.
 */
class EventLoopApplication : public AbstractApplication<initParams_t, processParams_t, stopParams_t>
{
private:
    po::options_description options{"Application options"};

    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;

public:
    // Пробрасываем публичную функцию из родительского класса в дочерний
    using AbstractApplication::pushRequest;

    /**
     * @typedef appDescription_t
     * @brief Структура описания приложения
     */
    typedef struct {
        std::string appName;
        std::string boostVersion;
        std::pair<int, int> version;
    } appDescription_t;

    EventLoopApplication(int argc, char * argv[]) : AbstractApplication<initParams_t, processParams_t, stopParams_t>(), socket(ioc), resolver(ioc) {
        this->options.add_options()
                ("help", "Produce this message")
                ("appName", po::value<std::string>(), "Application name")
                ("appMajorVersion", po::value<int>(), "Application major version")
                ("appMinorVersion", po::value<int>(), "Application minor version");
        // ========================================================

        // Parse constructor input ======================================
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, this->options), vm);
        po::notify(vm);
        // ==============================================================

        if (vm.count("appName")) {
            this->applicationName = vm["appName"].as<std::string>();
        } else {
            this->applicationName = "__unnamed_app__";  //Default application name is "unnamed"
        }

        if (vm.count("appMajorVersion")) {
            this->applicationVersion.first = vm["appMajorVersion"].as<int>();
        } else {
            this->applicationVersion.first = 0;     // Default value for major version index
        }

        if (vm.count("appMinorVersion")) {
            this->applicationVersion.second = vm["appMinorVersion"].as<int>();
        } else {
            this->applicationVersion.second = 1;    // Default value for minor version index
        }
    }

    /**
     * @brief Перегрузка оператора поточного вывода для вывода доступных ключей инициализации для приложения
     * @param out Поток вывода (ostringstream)
     * @param item Экземпляр приложения (EventLoopApplication)
     * @return Поток вывода (ostringstream)
     */
    friend std::ostringstream & operator<<(std::ostringstream & out, EventLoopApplication & item) {
        out << item.options;
        return out;
    }

    /**
     * @brief Инициализация приложени
     * @param params Startup параметры
     * @return Код выполнения
     */
    int init(initParams_t & params) override {
        (void)params;
        return 0;
    }

    /**
     * @brief Основная рабочая функция приложения
     * @param params Runtime параметры
     * @return Код выполнения
     */
    int process(processParams_t & params) override {
        (void)params;

        bool haltSnapshot{false};

        // Infinite loop
        while (true) {
            // Halt check ================
            haltSnapshot = this->stopFlag;
            if (haltSnapshot)
                break;
            // ===========================

            // External tasks executing ==================
            if (!taskQueue.empty()) {
                boost::lock_guard<boost::mutex>(this->queueMx);
                this->taskQueue.front().second();
                this->taskQueue.front().first.set_value();
                this->taskQueue.pop();
            }
            // ===========================================
        }
        return 0;
    }

    /**
     * @brief Функция деинициализации приложения
     * @param params Endtime параметры
     * @return Код выполнения
     */
    int stop(stopParams_t & params) override {
        (void)params;
        return 0;
    }

    // Описание интерфейсных функций (функций, которые могут быть добавлены в запрос)
    INTERFACES:

    /**
     * @brief Получение описания приложения
     * @param self Экземпляр приложения
     * @param output Ссылка на структуру описания приложения
     */
    static void getAppDescription(EventLoopApplication * self, boost::json::object & description) {
        try {
            if (!description.contains(JSON_FIELDS::filedName_AppName)) {
                description.emplace(JSON_FIELDS::filedName_AppName, boost::json::value(self->applicationName));
            } else {
                description[JSON_FIELDS::filedName_AppName].as_string() = self->applicationName;
            }

            if (!description.contains(JSON_FIELDS::filedName_BoostVersion)) {
                description.emplace(JSON_FIELDS::filedName_BoostVersion, boost::json::value(BOOST_LIB_VERSION));
            } else {
                description[JSON_FIELDS::filedName_BoostVersion].as_string() = BOOST_LIB_VERSION;
            }

            if (!description.contains(JSON_FIELDS::filedName_AppVersion)) {
                description.emplace(JSON_FIELDS::filedName_AppVersion, boost::json::object({\
                                                                                               {JSON_FIELDS::filedName_AppVersion_major, self->applicationVersion.first}, \
                                                                                               {JSON_FIELDS::filedName_AppVersion_minor, self->applicationVersion.second} \
                                                                                           }));
            } else {
                description[JSON_FIELDS::filedName_AppVersion].as_object() = boost::json::object({\
                                                                                                     {JSON_FIELDS::filedName_AppVersion_major, self->applicationVersion.first}, \
                                                                                                     {JSON_FIELDS::filedName_AppVersion_minor, self->applicationVersion.second} \
                                                                                                 });
            }
        } catch (std::exception &ex) {
            description.emplace(JSON_FIELDS::fieldName_Error, boost::json::value(ex.what()));
        }
    }

    /**
     * @brief Выполнение функции openSwitch из набора функций KeysightSCPI::SwitchDriverCmd
     * @param self Экземпляр приложения
     * @param bank Номер банка в устройстве
     * @param channel Номер канала в банке
     * @param result Ссылка на внешний флаг для контроля выполнения
     */
    static void execOpenSwitch(EventLoopApplication * self, int bank, int channel, bool & result) {
        std::string commandLine;

        result = KeysightSCPI::SwitchDriverCmd::openSwitch(bank, channel, commandLine);

        if (result) {
            boost::asio::write(self->socket, boost::asio::buffer(commandLine, commandLine.size()));
        }
    }

    /**
     * @brief Выполнение функции closeSwitch из набора функций KeysightSCPI::SwitchDriverCmd
     * @param self Экземпляр приложения
     * @param bank Номер банка в устройстве
     * @param channel Номер канала в банке
     * @param result Ссылка на внешний флаг для контроля выполнения
     */
    static void execCloseSwitch(EventLoopApplication * self, int bank, int channel, bool & result) {
        std::string commandLine;

        result = KeysightSCPI::SwitchDriverCmd::closeSwitch(bank, channel, commandLine);

        if (result) {
            boost::asio::write(self->socket, boost::asio::buffer(commandLine, commandLine.size()));
        }
    }

    /**
     * @brief Выполнение функции setVoltage из набора функций KeysightSCPI::SwitchDriverCmd
     * @param self Экземпляр приложения
     * @param bank Номер банка в устройстве
     * @param volt Значение устанавливаемого напряжения в целевом банке
     * @param result Ссылка на внешний флаг для контроля выполнения
     */
    static void execSetVoltage(EventLoopApplication * self, int bank, std::string volt, bool result) {
        std::string commandLine;

        result = KeysightSCPI::SwitchDriverCmd::setVoltage(bank, volt, commandLine);

        if (result) {
            boost::asio::write(self->socket, boost::asio::buffer(commandLine, commandLine.size()));
        }
    }

    /**
     * @brief Получение файловой структуры внутри директории
     * @param self Экземплря приложения
     * @param path Путь в котором будет производиться поиск файловой структруы
     * @param dataForJson Объект json в который будет помещён результат
     * @param count_id Указатель на счётчик идентификаторов (по умолчанию nullptr)
     */
    static void getFileStruct(EventLoopApplication *self, boost::filesystem::path path, boost::json::object &dataForJson, int * count_id){
        int idCounter = 0;
        if (count_id == nullptr)
            count_id = &idCounter;

        boost::filesystem::directory_iterator begin;
        if (dataForJson.contains(JSON_FIELDS::fieldName_FsModel)) {
            if (boost::filesystem::exists(dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Name].as_string().c_str())) {
                begin = boost::filesystem::directory_iterator(\
                            boost::filesystem::path(dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Name].as_string().c_str()));
            } else {
                dataForJson.emplace("__err", boost::json::value("Target directory not exists"));
                return;
            }
        } else {
            begin = boost::filesystem::directory_iterator(path);
        }

        boost::filesystem::directory_iterator end;

        for(; begin != end; ++begin){

            boost::filesystem::file_status file_status = boost::filesystem::status(begin->path());
            if(file_status.type() == boost::filesystem::regular_file)
            {
                *count_id += 1;
                if(dataForJson.contains(JSON_FIELDS::fieldName_FsModel)){
                    dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Childrens].as_array().emplace_back(boost::json::object(\
                                                                                                                                                        {\
                                                                                                                                                            {JSON_FIELDS::fieldName_Id, *count_id}, \
                                                                                                                                                            {JSON_FIELDS::fieldName_Name, begin->path().filename().string()}, \
                                                                                                                                                            {JSON_FIELDS::fieldName_IsAbsolute, true}\
                                                                                                                                                        }));
                }else if(dataForJson[JSON_FIELDS::fieldName_Childrens].is_array()) {
                    dataForJson[JSON_FIELDS::fieldName_Childrens].as_array().emplace_back(boost::json::object(\
                                                                                              {\
                                                                                                  {JSON_FIELDS::fieldName_Id, *count_id}, \
                                                                                                  {JSON_FIELDS::fieldName_Name, begin->path().filename().string()}, \
                                                                                                  {JSON_FIELDS::fieldName_IsAbsolute, true}\
                                                                                              }));
                } else
                    throw std::logic_error("Childrens is not array!");
            }
            else if(file_status.type() == boost::filesystem::directory_file)
            {
                *count_id += 1;
                if (dataForJson.contains(JSON_FIELDS::fieldName_FsModel)) {
                    dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Childrens].as_array().emplace_back(\
                                boost::json::object(\
                                    {\
                                        {JSON_FIELDS::fieldName_Id, *count_id}, \
                                        {JSON_FIELDS::fieldName_Name, begin->path().filename().string()}, \
                                        {JSON_FIELDS::fieldName_IsAbsolute, false}, \
                                        {JSON_FIELDS::fieldName_Childrens, boost::json::array()}\
                                    }));
                    size_t arraySize = dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Childrens].as_array().size();
                    boost::json::object &prev = dataForJson[JSON_FIELDS::fieldName_FsModel].as_array()[0].as_object()[JSON_FIELDS::fieldName_Childrens].as_array()[arraySize - 1].as_object();
                    getFileStruct(self, begin->path(), prev, count_id);
                } else {
                    dataForJson[JSON_FIELDS::fieldName_Childrens].as_array().emplace_back(\
                                boost::json::object(\
                                    {\
                                        {JSON_FIELDS::fieldName_Id, *count_id}, \
                                        {JSON_FIELDS::fieldName_Name, begin->path().filename().string()}, \
                                        {JSON_FIELDS::fieldName_IsAbsolute, false}, \
                                        {JSON_FIELDS::fieldName_Childrens, boost::json::array()}\
                                    }));
                    size_t arraySize = dataForJson[JSON_FIELDS::fieldName_Childrens].as_array().size();
                    boost::json::object &prev = dataForJson[JSON_FIELDS::fieldName_Childrens].as_array()[arraySize - 1].as_object();
                    getFileStruct(self, begin->path(), prev, count_id);
                }
            }
        }
    }
};

#endif // EVENTLOOPAPPLICATION_H


