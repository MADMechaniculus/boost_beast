#ifndef ABSTRACTGETPROC_H
#define ABSTRACTGETPROC_H

#include "proctypes.h"

/**
 * @brief Абстракция процессора GET запросов
 */
class AbstractGETProc {
public:
    AbstractGETProc() {};
    virtual processorProcRet_t process(std::string target, http::file_body::value_type & ansBody) = 0;
};

#endif // ABSTRACTGETPROC_H
