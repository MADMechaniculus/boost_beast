#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>

#include "requesthandler.h"
#include "eventloopapplication.h"
#include "userpostproc.h"
#include "usergetproc.h"
#include "demopostprocessor.h"
#include "demoapplication.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace po = boost::program_options;
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#define DEMO

#ifdef DEMO
typedef HTTPHandler<DemoPOSTProcessor, CustomGETProcessor> reqHndlr_t;
#else
typedef HTTPHandler<CustomPOSTProcessor, CustomGETProcessor> reqHndlr_t;
#endif
// Определение типа используемого обрабочика запросов

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

    po::options_description options("Available options");
    options.add_options()
            ("help", "Produce this message")
            ("listen", po::value<std::string>(), "Server listening IP family")
            ("port", po::value<uint16_t>(), "Server listening port")
            ("dir", po::value<std::string>(), "Server working directory");
//            ("targetIP", po::value<std::string>(), "Target device IP")
//            ("targetPORT", po::value<uint16_t>(), "Target device port")
//            ("debug", po::value<int>(), "Debug mode")

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 0;
    }

    // Application init =================================================================
    boost::thread appThread;

#ifdef DEMO
    DemoPOSTProcessor postProc;
#else
    CustomPOSTProcessor postProc;
#endif

    CustomGETProcessor getProc;

    // Constructor parameters for application ====
    std::array<const char *, 7> conArgv = { \
        "someKey",
        "--appName", "Awesome application",
        "--appMajorVersion", "0",
        "--appMinorVersion", "5"
    };

#ifdef DEMO
    demoProcess_t procPlaceholder;
#else
    initParams_t initP;
    processParams_t procPlaceholder;
#endif

    // ===========================================

    net::ip::address address;
    unsigned short port{0};
    std::shared_ptr<std::string> doc_root;

#ifdef DEMO
    DemoApplication application(conArgv.size(), (char **)conArgv.data());
#else
    EventLoopApplication application(conArgv.size(), (char **)conArgv.data());
#endif

    try {
#ifndef DEMO
        if (vm.count("targetIP")) {
            initP.targetIpAddress = vm["targetIP"].as<std::string>();
        } else {
            initP.targetIpAddress = "172.16.13.46";
            std::cout << "Target device set to default IP: " << initP.targetIpAddress << std::endl;
        }

        if (vm.count("targetPORT")) {
            initP.targetPort = vm["targetPORT"].as<uint16_t>();
        } else {
            initP.targetPort = 5025;
            std::cout << "Target device set to default PORT: " << initP.targetPort << std::endl;
        }

        if (application.init(initP) != 0) {
            throw std::runtime_error("Error on init application!");
        }
#endif
        postProc.connectApp(&application);

        appThread = boost::thread([&]() {
            std::cout << "Application started\n";
            application.process(procPlaceholder);
        });
    }  catch (std::exception &ex) {
        std::cout << "Terminate called after: " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try
    {
        if (vm.count("listen")) {
            address = net::ip::make_address(vm["listen"].as<std::string>());
        } else {
            address = net::ip::make_address("0.0.0.0");
            std::cout << "Running on default address: " << address.to_string() << std::endl;
        }

        if (vm.count("port")) {
            port = vm["port"].as<unsigned short>();
        } else {
            port = 8080;
            std::cout << "Running in default port: " << std::to_string(port) << std::endl;
        }

        if (vm.count("dir")) {
            doc_root = std::make_shared<std::string>(vm["dir"].as<std::string>());
        } else {
            doc_root = std::make_shared<std::string>(".");
            std::cout << "Running in default directory: '" << *doc_root.get() << "'" << std::endl;
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
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Halt application ..." << std::endl;
    application.halt();
    appThread.join();

    return EXIT_SUCCESS;
}
