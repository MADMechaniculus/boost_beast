#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <iostream>
#include <sstream>
#include <memory>

template<class Stream>
class MessageHandler
{
private:
    Stream * stream;
public:
    MessageHandler();

    ~MessageHandler() {
        if (!std::is_same<Stream, std::ostream>::value) {
            delete stream;
        }
    }

    Stream & getStream();
};

#endif // MESSAGEHANDLER_H
