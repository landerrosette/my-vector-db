#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <string>
#include "ScalarStorage.h"
#include "rapidjson/document.h"
#include <cstdint>
#include "IndexFactory.h"

class VectorDatabase {
public:
    explicit VectorDatabase(const std::string &db_path) : scalar_storage_(db_path) {}

    void upsert(uint64_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint64_t id) { return scalar_storage_.get_scalar(id); }

private:
    ScalarStorage scalar_storage_;
};


#endif //VECTORDATABASE_H
