#ifndef ABSTRACTAPPLICATION_H
#define ABSTRACTAPPLICATION_H

#include <boost/program_options.hpp>
#include <boost/atomic.hpp>

#define INTERFACES \
    public:

/**
 * @class AbstractApplication
 * @brief Абстрактный класс приложения
 */
template<class InitParams, class ProcessParams, class StopParams>
class AbstractApplication
{
private:
    boost::atomic<bool> isBusy;

public:
    AbstractApplication(int argc, char * argv[]) {
        (void)argc;
        (void)argv;
    };

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

    virtual int init(InitParams &params) =0;
    virtual int process(ProcessParams &params) =0;
    virtual int stop(StopParams &params) =0;
};

#endif // ABSTRACTAPPLICATION_H
