#include "HttpServer.h"

#include <utility>
#include <vector>

#include "FaissIndex.h"
#include "HNSWLibIndex.h"
#include "IndexFactory.h"
#include "logger.h"
#include "utils.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HttpServer::HttpServer(std::string host, int port, VectorDatabase &vector_database) : host(std::move(host)), port(port),
    vector_database(vector_database) {
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

    global_logger->info("Search request parameters: {}", req.body);

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
    setJsonResponse(json_response, res);
}

void HttpServer::insertHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    global_logger->info("Insert request parameters: {}", req.body);

    std::vector<float> data(json_request["vectors"].Size());
    for (rapidjson::SizeType i = 0; i < json_request["vectors"].Size(); ++i)
        data[i] = json_request["vectors"][i].GetFloat();
    uint32_t id = json_request["id"].GetUint();

    switch (auto index_type = get_index_type_from_request(json_request); index_type) {
        case IndexFactory::IndexType::FLAT: {
            auto *index = get_global_index_factory().get_index<FaissIndex>(index_type);
            index->insert_vectors(data, id);
            break;
        }
        case IndexFactory::IndexType::HNSW: {
            auto *index = get_global_index_factory().get_index<HNSWLibIndex>(index_type);
            index->insert_vectors(data, id);
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

    global_logger->info("Upsert request parameters: {}", req.body);

    uint32_t id = json_request["id"].GetUint();
    auto index_type = get_index_type_from_request(json_request);
    vector_database.upsert(id, json_request, index_type);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::queryHandler(const httplib::Request &req, httplib::Response &res) {
    rapidjson::Document json_request;
    json_request.Parse(req.body.c_str());

    global_logger->info("Query request parameters: {}", req.body);

    uint32_t id = json_request["id"].GetUint();
    rapidjson::Document json_data = vector_database.query(id);

    rapidjson::Document json_response;
    json_response.SetObject();
    auto &allocator = json_response.GetAllocator();
    for (auto &member: json_data.GetObject())
        json_response.AddMember(member.name, member.value, allocator);
    json_response.AddMember("retCode", 0, allocator);
    setJsonResponse(json_response, res);
}

void HttpServer::setJsonResponse(const rapidjson::Document &json_response, httplib::Response &res) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    json_response.Accept(writer);
    res.set_content(buffer.GetString(), "application/json");
}
