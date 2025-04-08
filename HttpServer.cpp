#include "HttpServer.h"

#include <cstdint>
#include <utility>
#include <vector>

#include "FaissIndex.h"
#include "IndexFactory.h"
#include "logger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HttpServer::HttpServer(std::string host, int port) : host(std::move(host)), port(port) {
    server.Post("/search", [this](const httplib::Request &req, httplib::Response &res) {
        searchHandler(req, res);
    });
    server.Post("/insert", [this](const httplib::Request &req, httplib::Response &res) {
        insertHandler(req, res);
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

    auto indexType = getIndexTypeFromRequest(json_request);
    void *index = getGlobalIndexFactory()->getIndex(indexType);

    std::pair<std::vector<long>, std::vector<float> > results;
    switch (indexType) {
        case IndexFactory::IndexType::FLAT: {
            auto *faissIndex = static_cast<FaissIndex *>(index);
            results = faissIndex->search_vectors(query, k);
            break;
        }
        default:
            break;
    }

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    rapidjson::Value vectors(rapidjson::kArrayType);
    rapidjson::Value distances(rapidjson::kArrayType);
    for (int i = 0; i < results.first.size(); ++i) {
        if (results.first[i] == -1) continue;
        vectors.PushBack(results.first[i], allocator);
        distances.PushBack(results.second[i], allocator);
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
        default:
            break;
    }

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

IndexFactory::IndexType HttpServer::getIndexTypeFromRequest(const rapidjson::Document &json_request) {
    if (json_request.HasMember("indexType")) {
        std::string index_type_string = json_request["indexType"].GetString();
        if (index_type_string == "FLAT")
            return IndexFactory::IndexType::FLAT;
    }
    return IndexFactory::IndexType::UNKNOWN;
}

void HttpServer::setJsonResponse(const rapidjson::Document &json_response, httplib::Response &res) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_response.Accept(writer);
    res.set_content(buffer.GetString(), "application/json");
}
