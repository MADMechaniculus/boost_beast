#ifndef KEYSIGHTCOMMAND_H
#define KEYSIGHTCOMMAND_H

#include <iostream>
#include <list>
#include <map>

namespace KeysightSCPI {
/**
 * @brief Класс формирования команд управления генератором.
 *
 * Предназначен для формирования строк, содержащих команды на языке SCPI, посылаемые на усройство Waveform Generator 33600A.
 */
class keysightCommand
{
public:
    /**
     * @brief signalSINFields - содержит названия параметров функции signalSIN.
     * Предназначено для хранения наименований параметров функции signalSIN.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalSINFields;
    /**
     * @brief signalSQUFields - содержит названия параметров функции signalSQU.
     * Предназначено для хранения наименований параметров функции signalSQU.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalSQUFields;
    /**
     * @brief signalRampFields - содержит названия параметров функции signalRamp.
     * Предназначено для хранения наименований параметров функции signalRamp.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalRampFields;
    /**
     * @brief signalPulsFields - содержит названия параметров функции signalPuls.
     * Предназначено для хранения наименований параметров функции signalPuls.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalPulsFields;
    /**
     * @brief signalTRIFields - содержит названия параметров функции signalTRI.
     * Предназначено для хранения наименований параметров функции signalTRI.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalTRIFields;
    /**
     * @brief signalNOISFields - содержит названия параметров функции signalNOIS.
     * Предназначено для хранения наименований параметров функции signalNOIS.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalNOISFields;
    /**
     * @brief signalPRBSFields - содержит названия параметров функции signalPRBS.
     * Предназначено для хранения наименований параметров функции signalPRBS.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalPRBSFields;
    /**
     * @brief signalDCFields - содержит названия параметров функции signalDC.
     * Предназначено для хранения наименований параметров функции signalDC.
     * Если параметр введён некорректно, функция обращаетс к этому контейнеру, ищет параметр по ключу и присваивает значение своему параметру wrongParam.
     */
    static std::map<uint16_t, std::string> signalDCFields;

public:
    keysightCommand();

    /**
     * @brief signalErr - формирует команду запроса ошибки
     * @return - возвращает команду запроса ошибки
     */
    static std::string signalErr();

    /**
     * @brief signalSIN - формирует команду переключения на синусоидальный сигнал
     * @param channel - канал (1 или 2)
     * @param freq - частота
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param phase - фаза
     * @param command - список команд (в функцию нужно подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalSIN(int channel, double freq, double voltH, double voltL, double phase, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalSQU - формирует команду переключения на прямоугольный сигнал
     * @param channel - канал (1 или 2)
     * @param dcyc - коэффициент заполнения
     * @param freq - частота
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param command - список команд (в функцию нужно подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalSQU(int channel, double dcyc, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalRamp - формирует команду переключения на пилообразный сигнал
     * @param channel - канал (1 или 2)
     * @param symmetry - симметрия
     * @param freq - частота
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param command - список команд (в функцию нужно подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalRamp(int channel, double symmetry, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalPuls - формирует команду переключения на импульсный сигнал
     * @param channel - канал (1 или 2)
     * @param lead - передний фронт
     * @param trail - задний фронт
     * @param width - длительность импульса
     * @param freq - частота
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param command - список команд (в функцию подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalPuls(int channel, double lead, double trail, double width, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalARB - формирует команду переключения на сигнал произвольной формы
     * @param file - файл ARB, содержащий сигнал
     * @param command - список команд (в функцию подавать пустым)
     * @return  - true
     */
    static bool signalARB(std::string file, std::list<std::string> & command);

    /**
     * @brief signalTRI - формирует команду переключения на треугольный сигнал
     * @param channel - канал (1 или 2)
     * @param freq - частота
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param phase - фаза
     * @param command - список команд (в функцию подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalTRI(int channel, double freq, double voltH, double voltL, double phase, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalNOIS - формирует команду переключения на белый гауссов шум
     * @param channel - канал (1 или 2)
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param band - полоса пропускания функции шума
     * @param command - список команд (в функцию подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalNOIS(int channel, double voltH, double voltL, double band, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalPRBS - формирует команду переключения на псевдослучайный сигнал
     * @param channel - канал (1 или 2)
     * @param brate - скорость в битах
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @param command - список команд (в функцию подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalPRBS(int channel, double brate, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief signalDC - формирует команду переключения на постоянный ток
     * @param channel - канал (1 или 2)
     * @param offset - смещение
     * @param command - список команд (в функцию подавать пустым)
     * @param wrongParam - наименование неверно введённого параметра
     * @return  - true, если все параметры переданы корректно, согласно ограничениям прибора
     */
    static bool signalDC(int channel, double offset, std::list<std::string> & command, std::string & wrongParam);

    /**
     * @brief catalogARB - формирует команду для запроса списка arb файлоа
     * @param path - путь к директории, в которой будет произволиться поиск
     * @param command - список команд (в функцию подавать пустым)
     * @return  - true
     */
    static bool catalogARB(std::string path, std::string & command);

    /**
     * @brief volatileCatalog - формирует команду для запроса списка произвольных (arb) сигналов, загруженных в канал channel
     * @param channel - канал (1 или 2)
     * @param command - список команд (в функцию подавать пустым)
     * @return  - true
     */
    static bool volatileCatalog(int channel, std::list<std::string> & command);

    /**
     * @brief loadARB - формирует команду для загрузки сиглана arb на выбранный канал
     * @param channel - канал (1 или 2)
     * @param file - путь к файлу с сигналом arb
     * @param command - список команд (в функцию подавать пустым)
     * @return  - true
     */
    static bool loadARB(int channel, std::string file, std::list<std::string> & command);

private:
    /**
     * @brief checkFreq - проверка корректности частоты
     * @param freq частота
     * @param signal - название сигнала
     * @param ampl - разница между максимальным и минимальным напряжением
     * @return  - true, если передана корректная частота freq
     */
    static bool checkFreq(double freq, std::string signal, double ampl);

    /**
     * @brief checkVolt - проверка корректности максимального и минимального напряжения
     * @param voltH - максимальное напряжение
     * @param voltL - минимальное напряжение
     * @return  - true, если корректно передано напряжение
     */
    static bool checkVolt(double voltH, double voltL);

    /**
     * @brief checkPhase - проверка корректности фазы
     * @param phase - фаза
     * @return  - true, если корректно передана фаза
     */
    static bool checkPhase(double phase);

    /**
     * @brief checkDcycle - проверка корректности коэффициента заполнения
     * @param dcyc - коэффициент заполнения
     * @return  - true, если корректно передан коэффициента заполнения
     */
    static bool checkDcycle(double dcyc);

    /**
     * @brief checkSymmetry - проверка корректности симметрии
     * @param symmetry - симметрия
     * @return  - true, если корректно передана симметрия
     */
    static bool checkSymmetry(double symmetry);

    /**
     * @brief checkWidth - проверка корректности длительности испульса
     * @param width - длительность испульса
     * @param ampl - разница между максимальным и минимальным напряжением
     * @param period - период
     * @return  - true, если корректно передана длительность импульса
     */
    static bool checkWidth(double width, double ampl, double period);

    /**
     * @brief checkFrontInRange - проверка корректности фронтов (нахождение их в допустимых диапазонах)
     * @param front - фронт сигнала (левый или правый)
     * @param ampl - разница между максимальным и минимальным напряжением
     * @return  - true, если корректно передан фронт
     */
    static bool checkFrontInRange(double front, double ampl);

    /**
     * @brief checkFrontConflict - проверка корректности (чтобы они были совместимы с заданными длительностью импульса и периодом)
     * @param lead - левый фронт сигнала
     * @param trail - правый фронт сигнала
     * @param width - длительность импульса
     * @param period - период
     * @return  - true, если корректно переданы фронты
     */
    static bool checkFrontConflict(double lead, double trail, double width, double period);

    /**
     * @brief checkBand - проверка корректности полосы пропускания
     * @param band - полоса пропускания
     * @param ampl - разница между максимальным и минимальным напряжением
     * @return  - true, если корректно передана полоса пропускания
     */
    static bool checkBand(double band, double ampl);

    /**
     * @brief checkOffset - проверка корректности смещения
     * @param offset - смещение
     * @return  - true, если корректно передано смещение
     */
    static bool checkOffset(double offset);

    /**
     * @brief checkBrate - проверка корректности скорости
     * @param brate - скорость в битах
     * @param ampl - разница между максимальным и минимальным напряжением
     * @return  - true, если корректно передана скорость
     */
    static bool checkBrate(double brate, double ampl);

};

/**
 * @brief Класс формирования команд управления контроллером коммутаторов
 *
 * Предназначен для формирования строк, содержащих команды на языке SCPI, посылаемые на усройство Attenuator / Switch Driver 11713C.
 */
class SwitchDriverCmd
{
public:
    SwitchDriverCmd();
    /**
     * @brief openSwitch - функция открытия свитча
     * @param bankNum - номер банка
     * @param channelNum - номер канала
     * @param command - строка, в которую будет записана команда, отправляемая на устройство
     * @return true, если bankNum и channelNum введены верно.
     */
    static bool openSwitch(int bankNum, int channelNum, std::string & command);

    /**
     * @brief closeSwitch - функция закрытия свитча
     * @param bankNum - номер банка
     * @param channelNum - номер канала
     * @param command - строка, в которую будет записана команда, отправляемая на устройство
     * @return true, если bankNum и channelNum введены верно.
     */
    static bool closeSwitch(int bankNum, int channelNum, std::string & command);

    /**
     * @brief setVoltage - функция установки напряжения на банке
     * @param bankNum - номер банка
     * @param volt - напряжение
     * @param command - строка, в которую будет записана команда, отправляемая на устройство
     * @return true, если bankNum введен верно.
     */
    static bool setVoltage(int bankNum, std::string volt, std::string & command);

    /**
     * @brief checkBank - функция проверки корректности ввода номера банка
     * @param bankNum - номер банка
     * @return true, если bankNum 1 или 2.
     */
    static bool checkBank(int bankNum);

    /**
     * @brief checkChannel - функция проверки корректности ввода номера канала
     * @param channelNum - номер канала
     * @return true, если channelNum от 0 до 9.
     */
    static bool checkChannel(int channelNum);

};

}
#endif // KEYSIGHTCOMMAND_H
