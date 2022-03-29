#ifndef DEMOAPPLICATION_H
#define DEMOAPPLICATION_H

#include "abstractapplication.h"

#include <boost/thread.hpp>
#include <boost/json.hpp>

#include <iostream>

typedef struct {

} demoInit_t;

typedef struct {

} demoProcess_t;

typedef struct {

} demoStop_t;

/**
 * @brief Класс демо-приложения для проверки навыков проектирования генерируемых в runtime интерфейсов
 */
class DemoApplication : public AbstractApplication<demoInit_t, demoProcess_t, demoStop_t>
{
    boost::mutex queueMx;

public:
    DemoApplication(int ac, char ** av) : AbstractApplication<demoInit_t, demoProcess_t, demoStop_t>() {
        (void)ac;
        (void)av;
    };

    int init(demoInit_t &) override {
        return 0;
    }

    int stop(demoStop_t &) override {
        return 0;
    }

    /**
     * @brief Основная рабочая функция приложения
     * @param params Runtime параметры
     * @return Код выполнения
     */
    int process(demoProcess_t & params) override {
        (void)params;

        bool haltSnapshot{false};

        // Infinite loop
        while (true) {
            // Halt check ================
            haltSnapshot = this->stopFlag;
            if (haltSnapshot)
                break;
            // ===========================

            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

            // External tasks executing ==================
            if (!taskQueue.empty()) {
                boost::lock_guard<boost::mutex>(this->queueMx);
                this->taskQueue.front().second();
                this->taskQueue.front().first.set_value();
                this->taskQueue.pop();
            }
            // ===========================================
        }
        return 0;
    }

    /**
     * @brief Добавление запроса в очередь
     * @param request Запрос к приложению
     * @return Результат добавления (флаг добавления в очередь и объект передачи результата)
     */
    pushResult_t pushRequest(request_func_t request) override {
        pushResult_t ret;
        if (this->taskQueue.size() < 10) {
            task_t temp;
            temp.second = request;
            ret.second = temp.first.get_future();
            boost::lock_guard<boost::mutex>(this->queueMx);
            this->taskQueue.push(std::move(temp));
            ret.first = true;
        } else {
            ret.first = false;
        }
        return ret;
    }

    INTERFACES:

    static void ifaceGetMenuDescription(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Get menu request handled\n";
        result = true;
        json["appVersion"] = boost::json::object({{"minor", 2}, {"major", "2"}});
        json["boostVersion"] = boost::json::value(BOOST_VERSION);
        json.emplace("menuItems", boost::json::array());
        int64_t id = 0;
        int64_t index = 1;
        json["menuItems"].as_array().push_back(boost::json::object({{"id", id++}, {"name", "Information"}, {"request", index++}}));
        json["menuItems"].as_array().push_back(boost::json::object({{"id", id++}, {"name", "Settings"}, {"request", index++}}));
        json["menuItems"].as_array().push_back(boost::json::object({{"id", id++}, {"name", "Controls"}, {"request", index++}}));
        json["menuItems"].as_array().push_back(boost::json::object({{"id", id++}, {"name", "Help"}, {"request", index++}}));
        json["menuItems"].as_array().push_back(boost::json::object({{"id", id++}, {"name", "Do \'beep\' :)"}, {"request", index++}}));
    }

    static void ifaceGetDevInfo(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Get device info request handled\n";
        result = true;

        const std::string descriptionPlaceholder = "Lorem ipsum...";
        json.emplace("devices", boost::json::array());

        for (size_t idx = 0; idx < 4; idx++) {
            json["devices"].as_array().push_back(boost::json::object({\
                                                                         {"id", idx}, \
                                                                         {"name", std::string("Device Block ") + std::to_string(idx)}, \
                                                                         {"serialNumber", "1234-5678-9012-3456"}, \
                                                                         {"description", descriptionPlaceholder}, \
                                                                         {"status", idx % 2 ? false : true} \
                                                                     }));
        }
    }

    static void ifaceGetSettings(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Get settings request handled\n";
        result = true;

        json.emplace("sections", boost::json::array());

        json["sections"].as_array().push_back(boost::json::object({{"id", 0}, {"name", "Section 0"}, {"options", boost::json::array()}}));
        json["sections"].as_array().push_back(boost::json::object({{"id", 1}, {"name", "Section 0"}, {"options", boost::json::array()}}));

        json["sections"].as_array()[0].as_object()["options"].as_array().push_back(boost::json::object({{"id", 0}, {"name", "Option 0"}, {"type", "__checkbox"}, {"state", false}}));
        json["sections"].as_array()[0].as_object()["options"].as_array().push_back(boost::json::object({{"id", 1}, {"name", "Option 1"}, {"type", "__switch"}, {"state", false}}));
        json["sections"].as_array()[0].as_object()["options"].as_array().push_back(boost::json::object({{"id", 2}, {"name", "Option 2"}, {"type", "__checkbox"}, {"state", true}}));

        json["sections"].as_array()[1].as_object()["options"].as_array().push_back(boost::json::object({{"id", 0}, {"name", "Option 0"}, {"type", "__checkbox"}, {"state", true}}));
        json["sections"].as_array()[1].as_object()["options"].as_array().push_back(boost::json::object({{"id", 1}, {"name", "Option 1"}, {"type", "__checkbox"}, {"state", false}}));
    }

    static void ifaceGetInfo(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Get information request handled\n";
        result = true;

        json.emplace("content", boost::json::value("Help text"));
    }

    static void ifaceGetControls(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Get controls request handled\n";
        result = true;

        json.emplace("controls", boost::json::array());

        json["controls"].as_array().push_back(boost::json::object({{"id", 0}, {"name", "Move left"}, {"type", "__button"}, {"request", boost::json::object({{"request", 6}, {"direction", "l"}})}}));
        json["controls"].as_array().push_back(boost::json::object({{"id", 1}, {"name", "Move right"}, {"type", "__button"}, {"request", boost::json::object({{"request", 6}, {"direction", "r"}})}}));
        json["controls"].as_array().push_back(boost::json::object({{"id", 2}, {"name", "Move up"}, {"type", "__button"}, {"request", boost::json::object({{"request", 6}, {"direction", "u"}})}}));
        json["controls"].as_array().push_back(boost::json::object({{"id", 3}, {"name", "Move down"}, {"type", "__button"}, {"request", boost::json::object({{"request", 6}, {"direction", "d"}})}}));
    }

    static void ifaceDoBeep(DemoApplication * self, bool result, boost::json::object & json) {
        std::cout << "Do BEEP request handled\n";
        result = true;
        json.emplace("message", boost::json::value("BEEP!"));
    }

    static void ifaceDoMove(DemoApplication * self, bool result, std::string direction, boost::json::object & json) {
        std::cout << "Do move request handled\n";
        std::cout << "So, and we go ";
        json.emplace("direction", boost::json::value(direction));
        if (direction == "l") {
            std::cout << "left!\n";
            result = true;
        } else if (direction == "r") {
            std::cout << "right!\n";
            result = true;
        } else if (direction == "u") {
            std::cout << "up!\n";
            result = true;
        } else if (direction == "d") {
            std::cout << "down!\n";
            result = true;
        } else {
            std::cout << "... NO! We stand place!\n";
            result = false;
        }

        std::cout << std::flush;
    }
};

#endif // DEMOAPPLICATION_H
