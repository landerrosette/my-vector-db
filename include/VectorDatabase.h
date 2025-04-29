#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "IndexFactory.h"
#include "Persistence.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class VectorDatabase {
public:
    VectorDatabase(std::unique_ptr<IndexFactory> index_factory, const std::filesystem::path &db_path,
                   const std::filesystem::path &wal_path) : index_factory(std::move(index_factory)),
                                                            scalar_storage(db_path),
                                                            persistence(wal_path, *this->index_factory.get(),
                                                                        scalar_storage) {}

    void upsert(uint32_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint32_t id) const { return scalar_storage.get_scalar(id); }

    std::pair<std::vector<uint32_t>, std::vector<float> > search(const rapidjson::Document &json_request) const;

    void reload_database();

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data) {
        persistence.write_wal_log(operation_type, json_data);
    }

    void take_snapshot() { persistence.take_snapshot(); }

private:
    std::unique_ptr<IndexFactory> index_factory;
    ScalarStorage scalar_storage;
    Persistence persistence;
};


#endif //VECTORDATABASE_H
