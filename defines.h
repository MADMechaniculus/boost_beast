#ifndef DEFINES_H
#define DEFINES_H

#include <string>
#include <future>
#include <queue>

/**
 * @brief Барьерный промис
 */
typedef std::promise<void> barrierPromise_t;

typedef std::future<void> barrierFuture_t;

/**
 * @brief Пара <bool, промис> для возврата из функции pushRequest
 */
typedef std::pair<bool, barrierFuture_t> pushResult_t;

/**
 * @brief Тип функции, который может быть помещён в качестве запроса
 */
typedef std::function<void()> request_func_t;

/**
 * @brief Структура для ранения события, которое должно произойти после того, как приложение выполнит запрос,
 * привязанный к future в данной структуре
 */
typedef std::pair<std::future<void>, std::function<void()>> response_t;

/**
 * @brief task_t Структура для хранения запроса (задачи) для приложения,
 * который должен быть выполнен в отложенном режиме
 */
typedef std::pair<std::promise<void>, request_func_t> task_t;

#endif // DEFINES_H
