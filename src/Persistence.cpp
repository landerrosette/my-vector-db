#include "Persistence.h"

#include <sstream>
#include <stdexcept>

#include "logger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void Persistence::init(const std::string &local_path) {
    wal_log_file.open(local_path, std::ios::in | std::ios::out | std::ios::app);
    if (!wal_log_file.is_open()) {
        global_logger->error("Error opening WAL log file: {}", std::strerror(errno));
        throw std::runtime_error("Error opening WAL log file: " + std::string(std::strerror(errno)));
    }
}

void Persistence::write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data,
                                const std::string &version) {
    auto log_id = increase_id();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    json_data.Accept(writer);
    wal_log_file << log_id << "|" << version << "|" << operation_type << "|" << buffer.GetString() << "\n";
    if (wal_log_file.fail()) {
        global_logger->error("Error writing to WAL log file: {}", std::strerror(errno));
    } else {
        global_logger->debug("WAL log entry written: {}|{}|{}|{}", log_id, version, operation_type, buffer.GetString());
        wal_log_file.flush();
    }
}

std::pair<std::string, rapidjson::Document> Persistence::read_next_wal_log() {
    global_logger->debug("Reading next WAL log entry");
    if (std::string line; std::getline(wal_log_file, line)) {
        std::istringstream iss(line);
        std::string log_id_str, version, operation_type, json_data_str;
        std::getline(iss, log_id_str, '|');
        std::getline(iss, version, '|');
        std::getline(iss, operation_type, '|');
        std::getline(iss, json_data_str);

        uint32_t log_id = std::stoul(log_id_str);
        if (log_id > id) id = log_id;

        rapidjson::Document json_data;
        json_data.Parse(json_data_str.c_str());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        json_data.Accept(writer);
        global_logger->debug("Read WAL log entry: {}|{}|{}|{}", log_id_str, version, operation_type,
                             buffer.GetString());
        return {std::move(operation_type), std::move(json_data)};
    } else {
        wal_log_file.clear();
        global_logger->debug("No more WAL log entries to read");
        return {};
    }
}
