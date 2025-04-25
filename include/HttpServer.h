#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <string>

#include "httplib.h"
#include "VectorDatabase.h"
#include "rapidjson/document.h"

class HttpServer {
public:
    HttpServer(std::string host, int port, VectorDatabase &vector_database);

    void start() { server.listen(host, port); }

private:
    httplib::Server server;
    std::string host;
    int port;
    VectorDatabase &vector_database;

    void searchHandler(const httplib::Request &req, httplib::Response &res);

    void insertHandler(const httplib::Request &req, httplib::Response &res);

    void upsertHandler(const httplib::Request &req, httplib::Response &res);

    void queryHandler(const httplib::Request &req, httplib::Response &res);

    void setJsonResponse(const rapidjson::Document &json_response, httplib::Response &res);
};


#endif //HTTPSERVER_H
