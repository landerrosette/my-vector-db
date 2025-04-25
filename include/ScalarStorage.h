#ifndef SCALARSTORAGE_H
#define SCALARSTORAGE_H


#include <memory>
#include <string>
#include <rocksdb/db.h>

#include "rapidjson/document.h"

class ScalarStorage {
public:
    explicit ScalarStorage(const std::string &db_path);

    void insert_scalar(uint32_t id, const rapidjson::Document &data);

    rapidjson::Document get_scalar(uint32_t id);

private:
    std::unique_ptr<rocksdb::DB> db;
};


#endif //SCALARSTORAGE_H
