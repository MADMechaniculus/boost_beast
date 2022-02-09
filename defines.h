#ifndef DEFINES_H
#define DEFINES_H

#include <string>
#include <future>
#include <queue>

/**
 * @brief Барьерный промис
 */
typedef std::promise<void> barrier_promise_t ;

/**
 * @brief Пара <bool, промис> для возврата из функции pushRequest
 */
typedef std::pair<bool, barrier_promise_t> pushResult_t;

/**
 * @brief Тип функции, который может быть помещён в качестве запроса
 */
typedef std::function<void()> request_func_t;
typedef std::pair<std::future<void>, std::function<void()>> response_t;

#endif // DEFINES_H
