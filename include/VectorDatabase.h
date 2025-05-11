#ifndef VECTORDATABASE_H
#define VECTORDATABASE_H


#include <utility>

#include "IndexFactory.h"
#include "Persistence.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class VectorDatabase {
public:
    explicit VectorDatabase(IndexFactory &&index_factory) : index_factory(std::move(index_factory)),
                                                            persistence(this->index_factory, scalar_storage) {}

    void upsert(uint32_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type);

    rapidjson::Document query(uint32_t id) const { return scalar_storage.get_scalar(id); }

    std::pair<std::vector<uint32_t>, std::vector<float> > search(const rapidjson::Document &json_request) const;

    void reload_database();

    void take_snapshot() { persistence.take_snapshot(); }

private:
    IndexFactory index_factory;
    ScalarStorage scalar_storage;
    Persistence persistence;

    void apply_upsert(uint32_t id, const rapidjson::Document &data, const rapidjson::Document &old_data, IndexFactory::IndexType index_type);
};


#endif //VECTORDATABASE_H
