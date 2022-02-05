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

#define SERVER_VERSION_MAJOR 0
#define SERVER_VERSION_MINOR 3
#define APPLICATION_NAME "Boost beast testing server application"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/json/src.hpp>
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
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------
// Определение кастомных процессоров типов запросов

class CustomPOSTProcessor : public AbstractPOSTProc
{
    EventLoopApplication * appPtr;
public:
    CustomPOSTProcessor(EventLoopApplication * app) : AbstractPOSTProc(), appPtr(app) {}

    bool process(std::string targetJson, http::file_body::value_type & ansBody) {

        std::cout << "[POST]\n";
        std::cout << targetJson << std::endl;

        boost::json::object answerJson;

        boost::json::object obj;
        boost::json::value value = boost::json::parse(targetJson);
        obj = value.as_object();

        answerJson["retCode"] = 0;

        if (obj["requestedFuncIndex"].is_int64()) {
            switch (obj["requestedFuncIndex"].as_int64()) {
            case 0:

                answerJson["serverVersion"] = { {"major", SERVER_VERSION_MAJOR}, {"minor", SERVER_VERSION_MINOR} };
                answerJson["boostVersion"] = BOOST_LIB_VERSION;
                answerJson["appName"] = APPLICATION_NAME;
                answerJson["retCode"] = (int64_t)0;

                break;
            default:
                answerJson["retCode"] = (int64_t)-1;
                break;
            }

            boost::system::error_code ec;
            std::string serialized = boost::json::serialize(answerJson);
            std::string path = "";
            std::ofstream simpleJson("./simple.json");
            if (simpleJson.is_open()) {
                simpleJson.write(serialized.c_str(), serialized.size());
                simpleJson.close();

                boost::filesystem::path filePath("./simple.json");
                path = boost::filesystem::absolute(filePath).generic_string();
                ansBody.open(path.c_str(), beast::file_mode::scan, ec);
            } else {
                return false;
            }
            return true;
        }
        return false;
    }
};

class CustomGETProcessor : public AbstractGETProc
{
    EventLoopApplication * appPtr;
public:
    CustomGETProcessor(EventLoopApplication * app) : AbstractGETProc(), appPtr(app) {}

    bool process(std::string target, http::file_body::value_type & ansBody) {
        std::cout << "Call from " << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "Target: " << target << std::endl;
        return false;   // always false, for default static response
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
    try
    {
        // Check command line arguments.
        if (argc != 4)
        {
            std::cerr <<
                         "Usage: http-server-sync <address> <port> <doc_root>\n" <<
                         "Example:\n" <<
                         "    http-server-sync 0.0.0.0 8080 .\n";
            return EXIT_FAILURE;
        }

        EventLoopApplication application(argc, argv);

        CustomPOSTProcessor postProc(&application);
        CustomGETProcessor getProc(&application);

        auto const address = net::ip::make_address(argv[1]);
        auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
        auto const doc_root = std::make_shared<std::string>(argv[3]);

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
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
