#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <string>

#include "httplib.h"
#include "VectorDatabase.h"

class HttpServer {
public:
    HttpServer(std::string host, int port, VectorDatabase &vector_database);

    void start() { server.listen(host, port); }

private:
    httplib::Server server;
    std::string host;
    int port;
    VectorDatabase &vector_database;

    void search_handler(const httplib::Request &req, httplib::Response &res);

    void upsert_handler(const httplib::Request &req, httplib::Response &res);

    void query_handler(const httplib::Request &req, httplib::Response &res);

    void snapshot_handler(const httplib::Request &req, httplib::Response &res);
};


#endif //HTTPSERVER_H
