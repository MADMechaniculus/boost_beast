#ifndef ABSTRACTAPPLICATION_H
#define ABSTRACTAPPLICATION_H

#include <boost/program_options.hpp>
#include <boost/atomic.hpp>
#include <boost/memory_order.hpp>

#include "defines.h"

#include <queue>

#define INTERFACES \
    public:

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

    /**
     * @brief Добавление запроса в очередь сполнения приложения
     * @param request Функция, переданная через bind содержщая запрос
     * @return Структуру с результатом добавления запроса в очередь:
     * - bool - флаг успешности процесса добавления
     * - std::future - объект future для отслеживания состояния запроса
     */
    virtual pushResult_t pushRequest(request_func_t request) =0;
};

#endif // ABSTRACTAPPLICATION_H
