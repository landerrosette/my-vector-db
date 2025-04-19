#include "ScalarStorage.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <string>
#include "logger.h"

ScalarStorage::ScalarStorage(const std::string &db_path) {
    rocksdb::Options options;
    options.create_if_missing = true;
    auto status = rocksdb::DB::Open(options, db_path, &db_);
}

void ScalarStorage::insert_scalar(uint64_t id, const rapidjson::Document &data) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    std::string value = buffer.GetString();
    auto status = db_->Put(rocksdb::WriteOptions(), std::to_string(id), value);
}

rapidjson::Document ScalarStorage::get_scalar(uint64_t id) {
    std::string value;
    auto status = db_->Get(rocksdb::ReadOptions(), std::to_string(id), &value);
    rapidjson::Document data;
    data.Parse(value.c_str());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    GlobalLogger->debug("Data retrieved: {}, RocksDB status: {}", buffer.GetString(), status.ToString());
    return data;
}
