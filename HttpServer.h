#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <string>

#include "httplib.h"
#include "IndexFactory.h"
#include "rapidjson/document.h"

class HttpServer {
public:
    HttpServer(std::string host, int port);

    void start() { server.listen(host, port); }

private:
    httplib::Server server;
    std::string host;
    int port;

    void searchHandler(const httplib::Request &req, httplib::Response &res);

    void insertHandler(const httplib::Request &req, httplib::Response &res);

    IndexFactory::IndexType getIndexTypeFromRequest(const rapidjson::Document &json_request);

    void setJsonResponse(const rapidjson::Document &json_response, httplib::Response &res);
};


#endif //HTTPSERVER_H
