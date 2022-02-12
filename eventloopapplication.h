#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include "abstractapplication.h"
#include "defines.h"

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>

#include <queue>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

namespace po = boost::program_options;

// Startup parameters =======================================
typedef struct {
    // Some params to prepare application in init function
} initParams_t;

typedef struct {
    // Some params to put in process function
} processParams_t;

typedef struct {
    // Some params to put in stop function
} stopParams_t;
// ==========================================================

/**
 * @brief Класс приложения, которое строится на принципе бесконечного цикла
 */
class EventLoopApplication : public AbstractApplication<initParams_t, processParams_t, stopParams_t>
{
private:
    po::options_description options{"Application options"};

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

public:
    EventLoopApplication(int argc, char * argv[]) : AbstractApplication<initParams_t, processParams_t, stopParams_t>(argc, argv) {
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

    int init(initParams_t & params) override {
        (void)params;
        return 0;
    }

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

            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));   // Some hard work

            // External tasks executing ==================
            // todo:
            // 1. check busy flag ???
            if (!taskQueue.empty()) {
                this->taskQueue.front().second();
                this->taskQueue.front().first.set_value();
                this->taskQueue.pop();
            }
            // ===========================================
        }
        return 0;
    }

    int stop(stopParams_t & params) override {
        (void)params;
        return 0;
    }

    pushResult_t pushRequest(request_func_t request) override {
        pushResult_t ret;
        if (this->taskQueue.size() < 10) {
            task_t temp;
            temp.second = request;
            ret.second = temp.first.get_future();
            this->taskQueue.push(std::move(temp));
            ret.first = true;
        } else {
            ret.first = false;
        }
        return ret;
    }

    INTERFACES

    static void getAppDescription(EventLoopApplication * self, appDescription_t & output) {
        output.appName = self->applicationName;
        output.boostVersion = BOOST_LIB_VERSION;
        output.version.first = self->applicationVersion.first;
        output.version.second = self->applicationVersion.second;
    }
};

#endif // EVENTLOOPAPPLICATION_H

