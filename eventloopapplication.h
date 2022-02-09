#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include "abstractapplication.h"
#include "defines.h"

#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <queue>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

typedef boost::function<void()> request_t;

/**
 * @brief Очередь задач для приложения, поступающих извне
 */
typedef std::queue<request_t> taskQueue_t;

typedef struct {
    uint16_t major;
    uint16_t minor;
} version_t;

typedef struct {
    std::string boostLibVersion;
    std::string applicationName;
    version_t appVersion;
} appDescription_t;

typedef struct {
    // Some params to prepare application in init function
} initParams_t;

typedef struct {
    // Some params to put in process function
} processParams_t;

typedef struct {
    // Some params to put in stop function
} stopParams_t;

// Пример приложения, использующего очередь задач для внешнего http сервера
class EventLoopApplication : public AbstractApplication<initParams_t, processParams_t, stopParams_t>
{
    taskQueue_t internatlTaskQueue;

    std::string applicationName{"EventLoopApplication"};

    boost::atomic<bool> externStop{false};

public:
    EventLoopApplication(int argc, char * argv[]) : AbstractApplication<initParams_t, processParams_t, stopParams_t>(argc, argv) { }

    /**
     * @brief Помещение задачи в очередь исполнения
     * @param task Задача, запроса
     * @return
     */
    bool pullTask(std::function<void()> && task) {
        if (this->internatlTaskQueue.size() < 10) {
            this->internatlTaskQueue.push(task);
            return true;
        }
        return false;
    }

    int init(initParams_t & params) {
        (void)params;
        return 0;
    }

    int process(processParams_t & params) {
        (void)params;

        int cycles = 0;

        // Infinite loop
        while (true) {
            // main application procedures
            // Some hard work

            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

            /// Это решение в лоб :) (вряд ли правильное и эффективное)
            /// Задачи поступаемые от http сервера имеют самый низкий приоритет, поэтому на стороне
            /// клиента необходимо предусмотреть ожидание ответа от сервера, потому как это может занять
            /// много времени (пока приоритетные задачи будут выполнены)
            if (!internatlTaskQueue.empty()) {
                internatlTaskQueue.front()();
                internatlTaskQueue.pop();
            } else {
                cycles++;
            }

            if (cycles > 5) {
                std::cout << "Application have not requested work!\n";
                break;
            }
        }

        return 0;
    }

    /**
     * @brief Асинхронная остановка работы приложения
     */
    void stopAsync() {
        this->externStop.store(true);
    }

    /**
     * @brief Нормальная деинициализация приложения
     * @param params Параметры деинициализации
     * @return
     */
    int stop(stopParams_t & params) {
        (void)params;
        return 0;
    }

INTERFACES

    static void getAppDescription(EventLoopApplication * self, appDescription_t & output) {
        output.applicationName = self->applicationName;
        output.boostLibVersion = BOOST_LIB_VERSION;

        output.appVersion.major = 0;
        output.appVersion.minor = 5;
    }

    bool pushRequest(request_t & func) {
        if (this->internatlTaskQueue.size() < 10) {
            this->internatlTaskQueue.push(func);
            return true;
        } else {
            return false;
        }
    }
};

#endif // EVENTLOOPAPPLICATION_H

