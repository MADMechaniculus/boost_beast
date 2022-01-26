#include "messagehandler.h"

template<>
MessageHandler<std::stringstream>::MessageHandler() {
    this->stream = new std::stringstream;
}

template<>
MessageHandler<std::ostream>::MessageHandler() {
    this->stream = &std::cout;
}

template<>
std::ostream &MessageHandler<std::ostream>::getStream()
{
    return *this->stream;
}

template<>
std::stringstream &MessageHandler<std::stringstream>::getStream()
{
    return *this->stream;
}
