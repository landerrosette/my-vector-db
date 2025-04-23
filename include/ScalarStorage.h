#ifndef SCALARSTORAGE_H
#define SCALARSTORAGE_H


#include <cstdint>
#include <string>
#include <vector>
#include <rocksdb/db.h>

#include "rapidjson/document.h"

class ScalarStorage {
public:
    explicit ScalarStorage(const std::string &db_path);

    ~ScalarStorage() { delete db_; }

    void insert_scalar(uint64_t id, const rapidjson::Document &data);

    rapidjson::Document get_scalar(uint64_t id);

private:
    rocksdb::DB *db_;
};


#endif //SCALARSTORAGE_H
