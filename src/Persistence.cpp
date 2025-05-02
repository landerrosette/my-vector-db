#include "Persistence.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "IndexFactory.h"
#include "logger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

Persistence::Persistence(IndexFactory &index_factory, ScalarStorage &scalar_storage) : index_factory(index_factory),
    scalar_storage(scalar_storage) {
    if (!wal_stream.is_open())
        throw std::runtime_error("Error opening WAL log file: " + std::string(std::strerror(errno)));
    load_last_snapshot_id();
}

void Persistence::write_wal_log(const std::string &operation_type, const rapidjson::Document &json_data) {
    auto log_id = increase_id();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    json_data.Accept(writer);
    wal_stream << log_id << "|" << operation_type << "|" << buffer.GetString() << "\n";
    wal_stream.flush();
    if (!wal_stream)
        throw std::runtime_error("Error writing WAL log: " + std::string(std::strerror(errno)));
    get_global_logger()->debug("WAL log entry written: {}|{}|{}", log_id, operation_type, buffer.GetString());
}

std::pair<std::string, rapidjson::Document> Persistence::read_next_wal_log() {
    get_global_logger()->debug("Reading next WAL log entry");
    for (std::string line; std::getline(wal_stream, line);) {
        std::istringstream iss(line);
        std::string log_id_str, operation_type, json_data_str;
        std::getline(iss, log_id_str, '|');
        std::getline(iss, operation_type, '|');
        std::getline(iss, json_data_str);

        uint32_t log_id = std::stoul(log_id_str);
        if (log_id > id) id = log_id;

        if (log_id > last_snapshot_id) {
            rapidjson::Document json_data;
            json_data.Parse(json_data_str.c_str());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer writer(buffer);
            json_data.Accept(writer);
            get_global_logger()->debug("Read WAL log entry: {}|{}|{}", log_id_str, operation_type, buffer.GetString());
            return {std::move(operation_type), std::move(json_data)};
        } else
            get_global_logger()->debug("Skipped WAL log entry: {}|{}|{}", log_id_str, operation_type, json_data_str);
    }
    if (!wal_stream && !wal_stream.eof())
        throw std::runtime_error("Error reading WAL log: " + std::string(std::strerror(errno)));
    wal_stream.clear();
    get_global_logger()->debug("No more WAL log entries to read");
    return {};
}

void Persistence::take_snapshot() {
    get_global_logger()->debug("Taking snapshot");
    last_snapshot_id = id;
    index_factory.save_index(SNAPSHOT_FOLDER_NAME);
    save_last_snapshot_id();
}

void Persistence::load_snapshot() {
    get_global_logger()->debug("Loading snapshot");
    index_factory.load_index(SNAPSHOT_FOLDER_NAME);
}

void Persistence::save_last_snapshot_id() {
    std::filesystem::create_directories(SNAPSHOT_FOLDER_NAME);
    std::ofstream ofs(std::filesystem::path(SNAPSHOT_FOLDER_NAME) / "max_log_id.txt");
    ofs << last_snapshot_id;
    ofs.close();
    if (!ofs)
        throw std::runtime_error("Error saving last snapshot ID: " + std::string(std::strerror(errno)));
    get_global_logger()->debug("Saved last snapshot ID: {}", last_snapshot_id);
}

void Persistence::load_last_snapshot_id() {
    std::filesystem::path last_snapshot_id_file_path = std::filesystem::path(SNAPSHOT_FOLDER_NAME) / "max_log_id.txt";
    if (std::filesystem::exists(last_snapshot_id_file_path)) {
        std::ifstream ifs(last_snapshot_id_file_path);
        ifs >> last_snapshot_id;
        if (!ifs && !ifs.eof())
            throw std::runtime_error("Error loading last snapshot ID: " + std::string(std::strerror(errno)));
        get_global_logger()->debug("Loaded last snapshot ID: {}", last_snapshot_id);
    }
}
