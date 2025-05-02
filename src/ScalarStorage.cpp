#include "ScalarStorage.h"

#include <stdexcept>
#include <string>

#include "logger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

ScalarStorage::ScalarStorage() {
    rocksdb::Options options;
    options.create_if_missing = true;
    if (auto status = rocksdb::DB::Open(options, SCALAR_STORAGE_NAME, &db); !status.ok())
        throw std::runtime_error("Error opening RocksDB: " + status.ToString());
}

void ScalarStorage::insert_scalar(uint32_t id, const rapidjson::Document &data) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    std::string value = buffer.GetString();
    if (auto status = db->Put(rocksdb::WriteOptions(), std::to_string(id), value); !status.ok())
        get_global_logger()->error("Error inserting scalar: {}", status.ToString());
}

rapidjson::Document ScalarStorage::get_scalar(uint32_t id) const {
    std::string value;
    auto status = db->Get(rocksdb::ReadOptions(), std::to_string(id), &value);
    if (!status.ok()) return {};

    rapidjson::Document data;
    data.Parse(value.c_str());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    get_global_logger()->debug("Data retrieved: {}, RocksDB status: {}", buffer.GetString(), status.ToString());
    return data;
}
