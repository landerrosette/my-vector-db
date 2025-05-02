#ifndef PERSISTENCE_H
#define PERSISTENCE_H


#include <fstream>
#include <string>
#include <utility>

#include "IndexFactory.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

constexpr const char *WAL_FILE_NAME = "index_wal.log";
constexpr const char *SNAPSHOT_FOLDER_NAME = "snapshot";

class Persistence {
public:
    Persistence(IndexFactory &index_factory, ScalarStorage &scalar_storage);

    ~Persistence() { if (wal_stream.is_open()) wal_stream.close(); }

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data);

    std::pair<std::string, rapidjson::Document> read_next_wal_log();

    void take_snapshot();

    void load_snapshot();

private:
    uint32_t id = 0;
    std::fstream wal_stream = std::fstream(WAL_FILE_NAME, std::ios::in | std::ios::out | std::ios::app);

    uint32_t last_snapshot_id = 0;

    IndexFactory &index_factory;
    ScalarStorage &scalar_storage;

    uint32_t increase_id() { return ++id; }

    void save_last_snapshot_id();

    void load_last_snapshot_id();
};


#endif //PERSISTENCE_H
