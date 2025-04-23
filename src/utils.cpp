#include "utils.h"

IndexFactory::IndexType getIndexTypeFromRequest(const rapidjson::Document &json_request) {
    if (json_request.HasMember("indexType") && json_request["indexType"].IsString()) {
        std::string index_type_string = json_request["indexType"].GetString();
        if (index_type_string == "FLAT")
            return IndexFactory::IndexType::FLAT;
        else if (index_type_string == "HNSW")
            return IndexFactory::IndexType::HNSW;
    }
    return IndexFactory::IndexType::UNKNOWN;
}
