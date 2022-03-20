#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>

#include <cstdlib>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "abstractpostproc.h"
#include "abstractgetproc.h"

namespace beast = boost::beast;
namespace net = boost::asio;

/**
 * @brief Базовый класс обработчика http запросов
 */
template<class POSTPROC, class GETPROC>
class RequestHandler
{
private:
    POSTPROC & postProcessorRef;
    GETPROC & getProcessorRef;

    /**
     * @brief Генератор константных ответов
     */
    class ResponseGenerator {
    public:
        /**
         * @enum FAST_REQUESTS
         * @brief Перечисление возможных ошибок при подготовке тела
         */
        enum FAST_REQUESTS {
            IllegalRequestTarget,
            NotFound,
            ServerError,
            __undef
        };

        ResponseGenerator() {}

        /**
         * @brief Генератор bad_request ответа
         */
        template<class Body, class Allocator>
        static http::response<http::string_body> bad_request(http::request<Body, http::basic_fields<Allocator>>& req,
                                                             beast::string_view why)
        {
            http::response<http::string_body> res{http::status::bad_request, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
        };

        /**
         * @brief Генератор ответа 404
         */
        template<class Body, class Allocator>
        static http::response<http::string_body> not_found(http::request<Body, http::basic_fields<Allocator>>& req,
                                                           beast::string_view target) {
            http::response<http::string_body> res{http::status::not_found, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + std::string(target) + "' was not found.";
            res.prepare_payload();
            return res;
        };

        /**
         * @brief Генератор ответа server_error
         */
        template<class Body, class Allocator>
        static http::response<http::string_body> server_error(http::request<Body, http::basic_fields<Allocator>>& req,
                                                              beast::string_view what) {
            http::response<http::string_body> res{http::status::internal_server_error, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
        };
    };

protected:
    /**
     * @brief Получение абсолютного пути к запрашиваемому контенту
     * @param base Местоположение относительно приложения
     * @param path Запрашиваемый ресурс
     * @return Абсолютный путь к ресурсу
     */
    static std::string path_cat(beast::string_view base, beast::string_view path)
    {
        if (base.empty())
            return std::string(path);
        std::string result(base);
#ifdef BOOST_MSVC
        char constexpr path_separator = '\\';
        if(result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
        for(auto& c : result)
            if(c == '/')
                c = path_separator;
#else
        char constexpr path_separator = '/';
        if(result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
#endif
        return result;
    }

    /**
     * @brief Получение mime/type запрашиваемого контента
     * @param path Путь к контенту
     * @return mime/type
     */
    static beast::string_view mime_type(beast::string_view path)
    {
        using beast::iequals;
        auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if(pos == beast::string_view::npos)
                return beast::string_view{};
            return path.substr(pos);
        }();
        if(iequals(ext, ".htm"))  return "text/html";
        if(iequals(ext, ".html")) return "text/html";
        if(iequals(ext, ".php"))  return "text/html";
        if(iequals(ext, ".css"))  return "text/css";
        if(iequals(ext, ".txt"))  return "text/plain";
        if(iequals(ext, ".js"))   return "application/javascript";
        if(iequals(ext, ".json")) return "application/json";
        if(iequals(ext, ".xml"))  return "application/xml";
        if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
        if(iequals(ext, ".flv"))  return "video/x-flv";
        if(iequals(ext, ".png"))  return "image/png";
        if(iequals(ext, ".jpe"))  return "image/jpeg";
        if(iequals(ext, ".jpeg")) return "image/jpeg";
        if(iequals(ext, ".jpg"))  return "image/jpeg";
        if(iequals(ext, ".gif"))  return "image/gif";
        if(iequals(ext, ".bmp"))  return "image/bmp";
        if(iequals(ext, ".ico"))  return "image/x-icon";
        if(iequals(ext, ".tiff")) return "image/tiff";
        if(iequals(ext, ".tif"))  return "image/tiff";
        if(iequals(ext, ".svg"))  return "image/svg+xml";
        if(iequals(ext, ".svgz")) return "image/svg+xml";
        return "application/text";
    }

public:
    /**
     * @brief Конструктор класса
     * @param postRef Ссылка на пользовательский обработчик POST запросов
     * @param getRef Ссылка на пользовательский обработкич GET азпросов
     */
    RequestHandler(POSTPROC & postRef, GETPROC & getRef) : postProcessorRef(postRef), getProcessorRef(getRef) { };

    // This is the C++11 equivalent of a generic lambda.
    // The function object is used to send an HTTP message.
    template<class Stream>
    struct send_lambda
    {
        Stream& stream_;
        bool& close_;
        beast::error_code& ec_;

        explicit send_lambda(
                Stream& stream,
                bool& close,
                beast::error_code& ec)
            : stream_(stream)
            , close_(close)
            , ec_(ec) {}

        template<bool isRequest, class Body, class Fields>
        void operator()(http::message<isRequest, Body, Fields>&& msg) const
        {
            // Determine if we should close the connection after
            close_ = msg.need_eof();

            // We need the serializer here because the serializer requires
            // a non-const file_body, and the message oriented version of
            // http::write only works with const messages.
            http::serializer<isRequest, Body, Fields> sr{msg};
            http::write(stream_, sr, ec_);
        }
    };

    /**
     * @brief Функциональный оператор
     * @param doc_root Корневой каталог обработчика запросов
     * @param req HTTP запрос от клиента
     * @param send Функция отправки ответа на поступивший запрос (для того, чтобы не использовать логику очередей и определения какому клиенту что отдавать)
     */
    template<class Body, class Allocator, class Send>
    void operator()(beast::string_view doc_root,
                    http::request<Body, http::basic_fields<Allocator>>&& req,
                    Send&& send) {

        http::file_body::value_type body;
        http::string_body::value_type postBody;
        std::string path = "";
        beast::error_code ec;
        bool checkFlag = false;
        std::ostringstream postStream;

        // Обработчик ошибки пдготовки запроса
        auto processError = [&]() {
            //            std::cout << "processError()" << std::endl;
            if (checkFlag)
                return ResponseGenerator::FAST_REQUESTS::IllegalRequestTarget;
            if(ec == beast::errc::no_such_file_or_directory)
                return ResponseGenerator::FAST_REQUESTS::NotFound;
            if(ec)
                return ResponseGenerator::FAST_REQUESTS::ServerError;
            return ResponseGenerator::FAST_REQUESTS::__undef;
        };

        // Проверка запрашиваемого target
        auto targetCheck = [&]() {
            //            std::cout << "targetCheck()" << std::endl;
            return (req.target().empty() ||
                    req.target()[0] != '/' ||
                    req.target().find("..") != beast::string_view::npos);
        };

        // Подготовка тела запроса
        auto prepare = [&]() {
            //            std::cout << "prepare()" << std::endl;
            if ((checkFlag = targetCheck()) == true) {
                return false;
            }

            // Build the path to the requested file
            path = path_cat(doc_root, req.target());
            if(req.target().back() == '/')
                path.append("index.html");

            // Attempt to open the file
            body.open(path.c_str(), beast::file_mode::scan, ec);

            // Handle the case where the file doesn't exist
            if(ec == beast::errc::no_such_file_or_directory)
                return false; // send(ResponseGenerator::not_found(req, req.target()));

            // Handle an unknown error
            if(ec)
                return false; // send(ResponseGenerator::server_error(req, ec.message()));

            return true;
        };

        // Результат обработки запроса пользовательским обработчиком
        processorProcRet_t processorReturn;

        switch (req.method()) {
        case http::verb::post:
            // Custom POST processor work ===============================================================
            processorReturn = this->postProcessorRef.process(req.body(), postBody);

            if (!processorReturn.first) {
                if (!prepare()) {
                    switch (processError()) {
                    case ResponseGenerator::FAST_REQUESTS::IllegalRequestTarget:
                        return send(ResponseGenerator::bad_request(req, "Illegal request-target"));
                        break;
                    case ResponseGenerator::FAST_REQUESTS::NotFound:
                        return send(ResponseGenerator::not_found(req, req.target()));
                        break;
                    default:
                        return send(ResponseGenerator::server_error(req, ec.message()));
                        break;
                    }
                }
            }
            break;
            // ==========================================================================================
        case http::verb::get:
            // Custom GET processor work ================================================================
            processorReturn = this->getProcessorRef.process(req.target().to_string(), body);

            if (!processorReturn.first) {
                if (!prepare()) {
                    switch (processError()) {
                    case ResponseGenerator::FAST_REQUESTS::IllegalRequestTarget:
                        return send(ResponseGenerator::bad_request(req, "Illegal request-target"));
                        break;
                    case ResponseGenerator::FAST_REQUESTS::NotFound:
                        return send(ResponseGenerator::not_found(req, req.target()));
                        break;
                    default:
                        return send(ResponseGenerator::server_error(req, ec.message()));
                        break;
                    }
                }
            }
            break;
            // ==========================================================================================
        case http::verb::head:
            if (!prepare()) {
                switch (processError()) {
                case ResponseGenerator::FAST_REQUESTS::IllegalRequestTarget:
                    return send(ResponseGenerator::bad_request(req, "Illegal request-target"));
                    break;
                case ResponseGenerator::FAST_REQUESTS::NotFound:
                    return send(ResponseGenerator::not_found(req, req.target()));
                    break;
                default:
                    return send(ResponseGenerator::server_error(req, ec.message()));
                    break;
                }
            }
            break;
        default:
            send(ResponseGenerator::server_error(req, "I'm sorry, but I don't know how to process such a request yet, sorry :("));
            break;
        }

        size_t size = 0;
        if (req.method() != http::verb::post) {
            size = body.size();
        } else {
            size = postBody.size();
        }

        if(req.method() == http::verb::head) {
            // Respond to HEAD request
            http::response<http::empty_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        } else if (req.method() == http::verb::get) {
            // Respond to GET request
            http::response<http::file_body> res{
                std::piecewise_construct,
                        std::make_tuple(std::move(body)),
                        std::make_tuple(http::status::ok, req.version())};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        } else {
            // Respond to POST request
            http::response<http::string_body> res{
                std::piecewise_construct,
                        std::make_tuple(std::move(postBody)),
                        std::make_tuple(http::status::ok, req.version()) };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "application/json");
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }
    }
};

#endif // REQUESTHANDLER_H
