#ifndef SCALARSTORAGE_H
#define SCALARSTORAGE_H


#include <memory>
#include <rocksdb/db.h>

#include "rapidjson/document.h"

constexpr const char *SCALAR_STORAGE_NAME = "scalar_storage";

class ScalarStorage {
public:
    ScalarStorage();

    void insert_scalar(uint32_t id, const rapidjson::Document &data);

    rapidjson::Document get_scalar(uint32_t id) const;

private:
    std::unique_ptr<rocksdb::DB> db;
};


#endif //SCALARSTORAGE_H
