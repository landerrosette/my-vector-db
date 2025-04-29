#include "utils.h"

#include <string>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

IndexFactory::IndexType get_index_type_from_request(const rapidjson::Document &json_request) {
    if (json_request.HasMember("indexType") && json_request["indexType"].IsString()) {
        if (std::string index_type_string = json_request["indexType"].GetString(); index_type_string == "FLAT")
            return IndexFactory::IndexType::FLAT;
        else if (index_type_string == "HNSW")
            return IndexFactory::IndexType::HNSW;
    }
    return IndexFactory::IndexType::UNKNOWN;
}

void set_json_response(const rapidjson::Document &json_response, httplib::Response &res) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    json_response.Accept(writer);
    res.set_content(buffer.GetString(), "application/json");
}
