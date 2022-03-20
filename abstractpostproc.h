#ifndef ABSTRACTPOSTPROC_H
#define ABSTRACTPOSTPROC_H

#include "proctypes.h"

/**
 * @brief Абстракция процессора POST запросов
 */
class AbstractPOSTProc {
public:
    AbstractPOSTProc() {};
    virtual processorProcRet_t process(std::string target, http::string_body::value_type & ansBody) = 0;
};

#endif // ABSTRACTPOSTPROC_H
