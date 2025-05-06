#ifndef PERSISTENCE_H
#define PERSISTENCE_H


#include <fstream>
#include <string>
#include <tuple>

#include "IndexFactory.h"
#include "ScalarStorage.h"
#include "rapidjson/document.h"

constexpr const char *WAL_FILE_NAME = "index_wal.log";
constexpr const char *SNAPSHOT_FOLDER_NAME = "snapshot";

class Persistence {
public:
    Persistence(IndexFactory &index_factory, ScalarStorage &scalar_storage);

    ~Persistence();

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &data,
                       const rapidjson::Document &old_data);

    std::tuple<std::string, rapidjson::Document, rapidjson::Document> read_next_wal_log();

    void take_snapshot();

    void load_snapshot();

private:
    uint32_t wal_id = 0;
    std::ofstream wal_write_stream;
    std::ifstream wal_read_stream;

    uint32_t last_snapshot_id = 0;

    IndexFactory &index_factory;
    ScalarStorage &scalar_storage;

    uint32_t increase_wal_id() { return ++wal_id; }

    void save_last_snapshot_id();

    void load_last_snapshot_id();
};


#endif //PERSISTENCE_H
