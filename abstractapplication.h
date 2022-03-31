#ifndef ABSTRACTAPPLICATION_H
#define ABSTRACTAPPLICATION_H

#include <boost/program_options.hpp>
#include <boost/atomic.hpp>
#include <boost/memory_order.hpp>
#include <boost/thread.hpp>

#include "defines.h"

#include <queue>

#define INTERFACES \
    public

/**
 * @class AbstractApplication
 * @brief Абстрактный класс приложения
 */
template<class InitParams, class ProcessParams, class StopParams>
class AbstractApplication
{
protected:
    boost::atomic<bool> isBusy{false};
    boost::atomic<bool> stopFlag{false};
    std::queue<task_t> taskQueue;

    std::string applicationName;
    std::pair<int, int> applicationVersion;
    boost::mutex queueMx;

public:
    AbstractApplication(void) {};

    /**
     * @brief
     *
     * Предусматривается, что приложение может выполнять большую работу,
     * которую нельзя прерывать ни при каких обстоятельствах, поэтому, нужно отслеживать
     * этот процесс с помощью отдельно стоящей атомарной переменной, которая будет указывать,
     * что приложение в данный момент занято.
     *
     * @return Значение флага занятости
     */
    bool getBusy(void) const {
        bool ret = isBusy;
        return ret;
    }

    /**
     * @brief Прерывание работы приложения, этот флаг должен учитываться в
     * главной исполняющей функции приложения (process)
     */
    void halt(void) {
        this->stopFlag.store(true);
    }

    /**
     * @brief Добавление запроса в очередь сполнения приложения
     * @param request Функция, переданная через bind содержщая запрос
     * @return Структуру с результатом добавления запроса в очередь:
     * - bool - флаг успешности процесса добавления
     * - std::future - объект future для отслеживания состояния запроса
     */
    pushResult_t pushRequest(request_func_t request)
    {
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

    /**
     * @brief Инициализация
     * @param params Параметры инициализации
     * @return Результат процесса инициализации
     */
    virtual int init(InitParams &params) =0;

    /**
     * @brief Функция исполнения функционала приложения. В теории, может содержать всё что угодно.
     * @param params Параметры исполнения
     * @return Код исполнения
     */
    virtual int process(ProcessParams &params) =0;

    /**
     * @brief Функция деинициализации (завершения работы приложения)
     * @param params Параметры деинициализации
     * @return Код исполнения
     */
    virtual int stop(StopParams &params) =0;
};

#endif // ABSTRACTAPPLICATION_H
