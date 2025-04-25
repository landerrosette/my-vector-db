#include "utils.h"
#include <string>

IndexFactory::IndexType get_index_type_from_request(const rapidjson::Document &json_request) {
    if (json_request.HasMember("indexType") && json_request["indexType"].IsString()) {
        if (std::string index_type_string = json_request["indexType"].GetString(); index_type_string == "FLAT")
            return IndexFactory::IndexType::FLAT;
        else if (index_type_string == "HNSW")
            return IndexFactory::IndexType::HNSW;
    }
    return IndexFactory::IndexType::UNKNOWN;
}
