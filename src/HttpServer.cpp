#include "HttpServer.h"

#include <utility>
#include <vector>

#include "logger.h"
#include "utils.h"
#include "rapidjson/document.h"

HttpServer::HttpServer(std::string host, int port, VectorDatabase &vector_database) : host(std::move(host)), port(port),
    vector_database(vector_database) {
    server.Post("/search", [this](const httplib::Request &req, httplib::Response &res) {
        search_handler(req, res);
    });
    server.Post("/upsert", [this](const httplib::Request &req, httplib::Response &res) {
        upsert_handler(req, res);
    });
    server.Post("/query", [this](const httplib::Request &req, httplib::Response &res) {
        query_handler(req, res);
    });
    server.Get("/admin/snapshot", [this](const httplib::Request &req, httplib::Response &res) {
        snapshot_handler(req, res);
    });
}

void HttpServer::search_handler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    get_global_logger()->info("Search request parameters: {}", req.body);

    auto [vecs, dists] = vector_database.search(json_request);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    rapidjson::Value vectors(rapidjson::kArrayType), distances(rapidjson::kArrayType);
    for (size_t i = 0; i < vecs.size(); ++i) {
        vectors.PushBack(vecs[i], allocator);
        distances.PushBack(dists[i], allocator);
    }
    json_response.AddMember("vectors", vectors, allocator);
    json_response.AddMember("distances", distances, allocator);
    json_response.AddMember("retCode", 0, allocator);
    set_json_response(json_response, res);
}

void HttpServer::upsert_handler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    get_global_logger()->info("Upsert request parameters: {}", req.body);

    uint32_t id = json_request["id"].GetUint();
    auto index_type = get_index_type_from_request(json_request);
    vector_database.upsert(id, json_request, index_type);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    set_json_response(json_response, res);
}

void HttpServer::query_handler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    get_global_logger()->info("Query request parameters: {}", req.body);

    uint32_t id = json_request["id"].GetUint();
    rapidjson::Document json_data = vector_database.query(id);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    if (json_data.IsObject()) {
        for (auto &member: json_data.GetObject())
            json_response.AddMember(member.name, member.value, allocator);
    }
    json_response.AddMember("retCode", 0, allocator);
    set_json_response(json_response, res);
}

void HttpServer::snapshot_handler(const httplib::Request &req, httplib::Response &res) {
    vector_database.take_snapshot();

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    set_json_response(json_response, res);
}
