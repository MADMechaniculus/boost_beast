#ifndef EVENTLOOPAPPLICATION_H
#define EVENTLOOPAPPLICATION_H

#include "abstractapplication.h"

typedef struct {

} initParams_t;

typedef struct {

} processParams_t;

typedef struct {

} stopParams_t;

class EventLoopApplication : public AbstractApplication<initParams_t, processParams_t, stopParams_t>
{
public:
    EventLoopApplication(int argc, char * argv[]) : AbstractApplication<initParams_t, processParams_t, stopParams_t>(argc, argv) { }

    bool init(initParams_t & params) {
        (void)params;
        return true;
    }

    bool process(processParams_t & params) {
        (void)params;

        // Infinite loop
        while (true) {
            // main application procedures
        }

        return true;
    }

    bool stop(stopParams_t & params) {
        (void)params;
        return true;
    }
};

#endif // EVENTLOOPAPPLICATION_H
