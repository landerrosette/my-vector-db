#ifndef PERSISTENCE_H
#define PERSISTENCE_H


#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include "IndexFactory.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

class Persistence {
public:
    Persistence(const std::filesystem::path &wal_file_path, IndexFactory &index_factory, ScalarStorage &scalar_storage);

    ~Persistence() { if (wal_stream.is_open()) wal_stream.close(); }

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data);

    std::pair<std::string, rapidjson::Document> read_next_wal_log();

    void take_snapshot();

    void load_snapshot();

private:
    uint32_t id = 0;
    std::fstream wal_stream;

    uint32_t last_snapshot_id = 0;

    IndexFactory &index_factory;
    ScalarStorage &scalar_storage;

    uint32_t increase_id() { return ++id; }

    void save_last_snapshot_id();

    void load_last_snapshot_id();
};


#endif //PERSISTENCE_H
