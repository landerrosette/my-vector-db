#ifndef PERSISTENCE_H
#define PERSISTENCE_H


#include <fstream>
#include <string>
#include <utility>

#include "rapidjson/document.h"

class Persistence {
public:
    Persistence() = default;

    ~Persistence() { if (wal_log_file.is_open()) wal_log_file.close(); }

    void init(const std::string &local_path);

    uint32_t increase_id() { return ++id; }

    uint32_t get_id() const { return id; }

    void write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data,
                       const std::string &version);

    std::pair<std::string, rapidjson::Document> read_next_wal_log();

private:
    uint32_t id = 0;
    std::fstream wal_log_file;
};


#endif //PERSISTENCE_H
