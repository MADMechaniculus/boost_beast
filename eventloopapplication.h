#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include "abstractapplication.h"
#include "defines.h"

#include <boost/function.hpp>

#include <queue>

/**
 * @brief Очередь задач для приложения, поступающих извне
 */
typedef std::queue<boost::function<void()>> taskQueue_t;

typedef struct {

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

public:
    EventLoopApplication(int argc, char * argv[]) : AbstractApplication<initParams_t, processParams_t, stopParams_t>(argc, argv) { }

    INTERFACE_GET_DECL(getAppDescription, appDescription_t);

    bool pullTask(std::function<void()> && task) {
        if (this->internatlTaskQueue.size() < 10) {
            this->internatlTaskQueue.push(task);
            return true;
        } else {
            // Task queue overflowed
            return false;
        }
    }

    bool init(initParams_t & params) {
        (void)params;
        return true;
    }

    bool process(processParams_t & params) {
        (void)params;

        // Infinite loop
        while (true) {
            // main application procedures
            // Some hard work

            /// Это решение в лоб :) (вряд ли правильное и эффективное)
            /// Задачи поступаемые от http сервера имеют самый низкий приоритет, поэтому на стороне
            /// клиента необходимо предусмотреть ожидание ответа от сервера, потому как это может занять
            /// много времени (пока приоритетные задачи будут выполнены)
            if (!internatlTaskQueue.empty()) {
                internatlTaskQueue.front()();
                internatlTaskQueue.pop();
            }
        }

        return true;
    }

    bool stop(stopParams_t & params) {
        (void)params;
        return true;
    }
};

INTERFACE_GET_IMPL(EventLoopApplication, getAppDescription, appDescription_t) {

}

#endif // EVENTLOOPAPPLICATION_H
