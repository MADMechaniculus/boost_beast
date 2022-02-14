//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, synchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/json/src.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>

#include "requesthandler.h"
#include "messagehandler.h"
#include "eventloopapplication.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace po = boost::program_options;
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------
// Определение кастомных процессоров типов запросов

class CustomPOSTProcessor : public AbstractPOSTProc
{
    EventLoopApplication * appPtr;
public:
    CustomPOSTProcessor(EventLoopApplication * app) : AbstractPOSTProc(), appPtr(app) {}

    processorProcRet_t process(std::string targetJson, http::string_body::value_type & ansBody) override {
        processorProcRet_t ret;
        ret.first = false;

        boost::json::object answerJson;

        boost::json::object obj;
        boost::json::value value = boost::json::parse(targetJson);
        obj = value.as_object();

        answerJson["retCode"] = 0;

        EventLoopApplication::appDescription_t appDescription;

        pushResult_t pushReturn;

        auto wrapperOpenSwitch = [&]() {
            int bank = obj["bank"].as_int64();
            int channel = obj["channel"].as_int64();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execOpenSwitch, appPtr, bank, channel, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        auto wrapperCloseSwitch = [&]() {
            int bank = obj["bank"].as_int64();
            int channel = obj["channel"].as_int64();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execCloseSwitch, appPtr, bank, channel, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        auto wrapperSetVoltage = [&]() {
            int bank = obj["bank"].as_int64();
            std::string voltage = obj["voltage"].as_string().c_str();

            bool execRet{false};

            pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::execSetVoltage, appPtr, bank, voltage, std::ref(execRet)));

            if (!pushReturn.first) {
                ret.first = false;
            }

            pushReturn.second.get();

            if (execRet)
                answerJson["retCode"] = (int64_t)0;
            else
                answerJson["retCode"] = (int64_t)-1;
        };

        if (obj["requestedFuncIndex"].is_int64()) {
            switch (obj["requestedFuncIndex"].as_int64()) {
            case 0:

                pushReturn = appPtr->pushRequest(std::bind(&EventLoopApplication::getAppDescription, appPtr, std::ref(appDescription)));

                if (!pushReturn.first)
                    ret.first = false;

                pushReturn.second.get();

                answerJson["appVersion"] = { {"major", appDescription.version.first}, {"minor", appDescription.version.second} };
                answerJson["boostVersion"] = appDescription.boostVersion;
                answerJson["appName"] = appDescription.appName;
                answerJson["retCode"] = (int64_t)0;

                break;
            case 1:

                wrapperOpenSwitch();

                break;
            case 2:

                wrapperCloseSwitch();

                break;
            case 3:

                wrapperSetVoltage();

                break;
            default:
                answerJson["retCode"] = (int64_t)-1;
                break;
            }

            ret.second = boost::json::serialize(answerJson);
            ansBody = ret.second;
            ret.first = true;

            return ret;
        }
        return ret;
    }
};

class CustomGETProcessor : public AbstractGETProc
{
    EventLoopApplication * appPtr;
public:
    CustomGETProcessor(EventLoopApplication * app) : AbstractGETProc(), appPtr(app) {}

    processorProcRet_t process(std::string target, http::file_body::value_type & ansBody) override {
        (void)appPtr;
        (void)ansBody;

        processorProcRet_t ret;
        ret.first = false;

        std::cout << "Call from " << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "Target: " << target << std::endl;
        return ret;   // always false, for default static response
    }
};

// Определение типа используемого обрабочика запросов
typedef RequestHandler<CustomPOSTProcessor, CustomGETProcessor> reqHndlr_t;

// Report a failure
void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
template<class ReqHandler>
void do_session(tcp::socket& socket, std::shared_ptr<std::string const> const& doc_root, ReqHandler & handler);

template<>
void do_session<reqHndlr_t>(tcp::socket& socket, std::shared_ptr<std::string const> const& doc_root, reqHndlr_t & handler)
{
    bool close = false;
    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    // This lambda is used to send messages
    reqHndlr_t::send_lambda<tcp::socket> lambda{socket, close, ec};

    for(;;)
    {
        // Read a request
        http::request<http::string_body> req;
        http::read(socket, buffer, req, ec);
        if(ec == http::error::end_of_stream)
            break;
        if(ec)
            return fail(ec, "read");

        // Send the response
        handler(*doc_root, std::move(req), lambda);
        if(ec)
            return fail(ec, "write");
        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }

    // Send a TCP shutdown
    socket.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Init programm options ===================================================

    // Debug program options ===================================================
    std::array<const char *, 3> progOptions = { \
        "someKey",
        "--dir", "dist"
    };
    // =========================================================================

    po::options_description options("Available options");
    options.add_options()
            ("help", "Produce this message")
            ("listen", po::value<std::string>(), "Server listening IP family")
            ("port", po::value<uint16_t>(), "Server listening port")
            ("dir", po::value<std::string>(), "Server working directory")
            ("targetIP", po::value<std::string>(), "Target device IP")
            ("targetPORT", po::value<uint16_t>(), "Target device port")
            ("debug", po::value<int>(), "Debug mode");

    po::variables_map vm;
    if (argc < 2) {
        po::store(po::parse_command_line(progOptions.size(), (char **)progOptions.data(), options), vm);
    } else {
        po::store(po::parse_command_line(argc, argv, options), vm);
    }
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    // =========================================================================

    boost::thread appThread;

    try
    {

        // Constructor parameters for application ====
        std::array<const char *, 7> conArgv = { \
            "someKey",
            "--appName", "Awesome application",
            "--appMajorVersion", "0",
            "--appMinorVersion", "5"
        };
        // ===========================================

        initParams_t initP;

        if (vm.count("targetIP")) {
            initP.targetIpAddress = vm["targetIP"].as<std::string>();
        } else {
            initP.targetIpAddress = "172.16.13.46";
            std::cout << "Target device set to default IP: 172.16.13.46" << std::endl;
        }

        if (vm.count("targetPORT")) {
            initP.targetPort = vm["targetPORT"].as<uint16_t>();
        } else {
            initP.targetPort = 5025;
            std::cout << "Target device set to default PORT: 5025" << std::endl;
        }

        EventLoopApplication application(conArgv.size(), (char **)conArgv.data());

        CustomPOSTProcessor postProc(&application);
        CustomGETProcessor getProc(&application);

        processParams_t procPlaceholder;

        if (application.init(initP) != 0) {
            throw std::runtime_error("Error on init application!");
        }

        appThread = boost::thread([&]() {
            application.process(procPlaceholder);
        });

        net::ip::address address;

        if (vm.count("listen")) {
            address = net::ip::make_address(vm["listen"].as<std::string>());
        } else {
            address = net::ip::make_address("0.0.0.0");
            std::cout << "Running on default address: 0.0.0.0" << std::endl;
        }

        unsigned short port = 0;

        if (vm.count("port")) {
            port = vm["port"].as<unsigned short>();
        } else {
            port = 8080;
            std::cout << "Running in default port: 8080" << std::endl;
        }

        std::shared_ptr<std::string> doc_root;

        if (vm.count("dir")) {
            doc_root = std::make_shared<std::string>(vm["dir"].as<std::string>());
        } else {
            doc_root = std::make_shared<std::string>(".");
            std::cout << "Running in default directory: '.'" << std::endl;
        }

        reqHndlr_t handler(postProc, getProc);

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, {address, port}};
        for(;;)
        {
            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
            acceptor.accept(socket);

            // Launch the session, transferring ownership of the socket
            std::thread{std::bind(
                            &do_session<reqHndlr_t>,
                            std::move(socket),
                            doc_root,
                            handler)}.detach();
        }

        std::cout << "Halt application ..." << std::endl;
        application.halt();
        appThread.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
