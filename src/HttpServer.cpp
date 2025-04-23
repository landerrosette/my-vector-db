#include "HttpServer.h"

#include <cstdint>
#include <utility>
#include <vector>

#include "FaissIndex.h"
#include "HNSWLibIndex.h"
#include "IndexFactory.h"
#include "logger.h"
#include "utils.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HttpServer::HttpServer(std::string host, int port, VectorDatabase *vector_database) : host(std::move(host)), port(port),
    vector_database_(vector_database) {
    server.Post("/search", [this](const httplib::Request &req, httplib::Response &res) {
        searchHandler(req, res);
    });
    server.Post("/insert", [this](const httplib::Request &req, httplib::Response &res) {
        insertHandler(req, res);
    });
    server.Post("/upsert", [this](const httplib::Request &req, httplib::Response &res) {
        upsertHandler(req, res);
    });
    server.Post("/query", [this](const httplib::Request &req, httplib::Response &res) {
        queryHandler(req, res);
    });
}

void HttpServer::searchHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    GlobalLogger->info("Search request parameters: {}", req.body);

    std::vector<float> query;
    for (const auto &q: json_request["vectors"].GetArray())
        query.push_back(q.GetFloat());
    int k = json_request["k"].GetInt();

    auto [first, second] = vector_database_->search(json_request);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    rapidjson::Value vectors(rapidjson::kArrayType);
    rapidjson::Value distances(rapidjson::kArrayType);
    for (int i = 0; i < first.size(); ++i) {
        if (first[i] == -1) continue;
        vectors.PushBack(first[i], allocator);
        distances.PushBack(second[i], allocator);
    }
    json_response.AddMember("vectors", vectors, allocator);
    json_response.AddMember("distances", distances, allocator);
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::insertHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    GlobalLogger->info("Insert request parameters: {}", req.body);

    std::vector<float> data;
    for (const auto &d: json_request["vectors"].GetArray())
        data.push_back(d.GetFloat());
    uint64_t label = json_request["id"].GetUint64();

    auto indexType = getIndexTypeFromRequest(json_request);
    void *index = getGlobalIndexFactory()->getIndex(indexType);

    switch (indexType) {
        case IndexFactory::IndexType::FLAT: {
            auto *faissIndex = static_cast<FaissIndex *>(index);
            faissIndex->insert_vectors(data, label);
            break;
        }
        case IndexFactory::IndexType::HNSW: {
            auto *hnswIndex = static_cast<HNSWLibIndex *>(index);
            hnswIndex->insert_vectors(data, label);
            break;
        }
        default:
            break;
    }

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::upsertHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    GlobalLogger->info("Upsert request parameters: {}", req.body);

    uint64_t label = json_request["id"].GetUint64();
    auto indexType = getIndexTypeFromRequest(json_request);
    vector_database_->upsert(label, json_request, indexType);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::queryHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    GlobalLogger->info("Query request parameters: {}", req.body);

    uint64_t label = json_request["id"].GetUint64();
    rapidjson::Document json_data = vector_database_->query(label);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    for (auto it = json_data.MemberBegin(); it != json_data.MemberEnd(); ++it)
        json_response.AddMember(it->name, it->value, allocator);
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::setJsonResponse(const rapidjson::Document &json_response, httplib::Response &res) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    json_response.Accept(writer);
    res.set_content(buffer.GetString(), "application/json");
}
