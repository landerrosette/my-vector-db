#ifndef SCALARSTORAGE_H
#define SCALARSTORAGE_H


#include <filesystem>
#include <memory>
#include <string>
#include <rocksdb/db.h>

#include "rapidjson/document.h"

class ScalarStorage {
public:
    explicit ScalarStorage(const std::filesystem::path &db_path);

    void insert_scalar(uint32_t id, const rapidjson::Document &data);

    rapidjson::Document get_scalar(uint32_t id) const;

    void put(const std::string &key, const std::string &value);

    std::string get(const std::string &key) const;

private:
    std::unique_ptr<rocksdb::DB> db;
};


#endif //SCALARSTORAGE_H
