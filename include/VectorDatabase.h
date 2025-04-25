#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <string>
#include <utility>

#include "IndexFactory.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class VectorDatabase {
public:
    explicit VectorDatabase(const std::string &db_path) : scalar_storage(db_path) {}

    void upsert(uint32_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint32_t id) { return scalar_storage.get_scalar(id); }

    std::pair<std::vector<uint32_t>, std::vector<float> > search(const rapidjson::Document &json_request);

private:
    ScalarStorage scalar_storage;
};


#endif //VECTORDATABASE_H
