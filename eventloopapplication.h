#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

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
    std::string targetIpAddress;
    uint16_t targetPort;
} initParams_t;

typedef struct {
    // Some params to put in process function
} processParams_t;

typedef struct {
    // Some params to put in stop function
} stopParams_t;
// ==========================================================

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
    boost::mutex queueMx;

public:
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
        // Application options description generatign =============
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

        try {
            // Тело инициализации отключено для отладки
            // Подключение к целевмоу устройству =================================================================================
            // boost::asio::connect(this->socket, this->resolver.resolve(params.targetIpAddress, std::to_string(params.targetPort)));
            // ===================================================================================================================
        }  catch (std::exception & ex) {
            return -1;
        }

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

    /**
     * @brief Добавление запроса в очередь
     * @param request Запрос к приложению
     * @return Результат добавления (флаг добавления в очередь и объект передачи результата)
     */
    pushResult_t pushRequest(request_func_t request) override {
        pushResult_t ret;
        if (this->taskQueue.size() < 10) {
            task_t temp;
            temp.second = request;
            ret.second = temp.first.get_future();
            boost::lock_guard<boost::mutex>(this->queueMx);
            this->taskQueue.push(std::move(temp));
            ret.first = true;
        } else {
            ret.first = false;
        }
        return ret;
    }

    // Описание интерфейсных функций (функций, которые могут быть добавлены в запрос)
    INTERFACES

    /**
     * @brief Получение описания приложения
     * @param self Экземпляр приложения
     * @param output Ссылка на структуру описания приложения
     */
    static void getAppDescription(EventLoopApplication * self, appDescription_t & output) {
        output.appName = self->applicationName;
        output.boostVersion = BOOST_LIB_VERSION;
        output.version.first = self->applicationVersion.first;
        output.version.second = self->applicationVersion.second;
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
};

#endif // EVENTLOOPAPPLICATION_H

