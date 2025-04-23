#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <cstdint>
#include <string>
#include <utility>

#include "IndexFactory.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class VectorDatabase {
public:
    explicit VectorDatabase(const std::string &db_path) : scalar_storage_(db_path) {}

    void upsert(uint64_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint64_t id) { return scalar_storage_.get_scalar(id); }

    std::pair<std::vector<long>, std::vector<float>> search(const rapidjson::Document &json_request);

private:
    ScalarStorage scalar_storage_;
};


#endif //VECTORDATABASE_H
