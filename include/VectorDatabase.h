#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <string>
#include <utility>

#include "IndexFactory.h"
#include "Persistence.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class VectorDatabase {
public:
    VectorDatabase(const std::string &db_path, const std::string &wal_path) : scalar_storage(db_path) {
        persistence.init(wal_path);
    }

    void upsert(uint32_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint32_t id) { return scalar_storage.get_scalar(id); }

    std::pair<std::vector<uint32_t>, std::vector<float> > search(const rapidjson::Document &json_request);

    void reload_database();

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data);

private:
    ScalarStorage scalar_storage;
    Persistence persistence;
};


#endif //VECTORDATABASE_H
