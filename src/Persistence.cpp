#include "Persistence.h"

#include <sstream>
#include <utility>

#include "IndexFactory.h"
#include "logger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

Persistence::Persistence(IndexFactory &index_factory, ScalarStorage &scalar_storage) : index_factory(index_factory),
    scalar_storage(scalar_storage) {
    wal_write_stream.exceptions(std::ios::failbit | std::ios::badbit);
    wal_read_stream.exceptions(std::ios::failbit | std::ios::badbit);
    wal_write_stream.open(WAL_FILE_NAME, std::ios::app);
    wal_read_stream.open(WAL_FILE_NAME);
    load_last_snapshot_id();
}

Persistence::~Persistence() {
    wal_write_stream.close();
    wal_read_stream.close();
}

void Persistence::write_wal_log(const std::string &operation_type, const rapidjson::Document &data,
                                const rapidjson::Document &old_data) {
    auto id = increase_wal_id();
    rapidjson::StringBuffer data_buffer, old_data_buffer;
    rapidjson::Writer data_writer(data_buffer), old_data_writer(old_data_buffer);
    data.Accept(data_writer);
    old_data.Accept(old_data_writer);
    wal_write_stream << id << "|" << operation_type << "|" << data_buffer.GetString() << "|" << old_data_buffer.
            GetString() << "\n";
    wal_write_stream.flush();
    get_global_logger()->debug("WAL log entry written: {}|{}|{}|{}", id, operation_type, data_buffer.GetString(),
                               old_data_buffer.GetString());
}

std::tuple<std::string, rapidjson::Document, rapidjson::Document> Persistence::read_next_wal_log() {
    get_global_logger()->debug("Reading next WAL log entry");
    try {
        for (std::string line; std::getline(wal_read_stream, line);) {
            std::istringstream iss(line);
            std::string log_id_str, operation_type, data_str, old_data_str;
            std::getline(iss, log_id_str, '|');
            std::getline(iss, operation_type, '|');
            std::getline(iss, data_str, '|');
            std::getline(iss, old_data_str);

            uint32_t log_id = std::stoul(log_id_str);
            if (log_id > wal_id) wal_id = log_id;

            if (log_id > last_snapshot_id) {
                rapidjson::Document data, old_data;
                data.Parse(data_str.c_str());
                old_data.Parse(old_data_str.c_str());
                rapidjson::StringBuffer data_buffer, old_data_buffer;
                rapidjson::Writer data_writer(data_buffer), old_data_writer(old_data_buffer);
                data.Accept(data_writer);
                old_data.Accept(old_data_writer);
                get_global_logger()->debug("Read WAL log entry: {}|{}|{}|{}", log_id_str, operation_type,
                                           data_buffer.GetString(), old_data_buffer.GetString());
                return {std::move(operation_type), std::move(data), std::move(old_data)};
            } else
                get_global_logger()->debug("Skipped WAL log entry: {}|{}|{}|{}", log_id_str, operation_type, data_str,
                                           old_data_str);
        }
    } catch (const std::ios_base::failure &) {
        if (!wal_read_stream.eof()) throw;
    }
    get_global_logger()->debug("No more WAL log entries to read");
    return {};
}

void Persistence::take_snapshot() {
    get_global_logger()->debug("Taking snapshot");
    last_snapshot_id = wal_id;
    index_factory.save_index(SNAPSHOT_FOLDER_NAME);
    save_last_snapshot_id();
}

void Persistence::load_snapshot() {
    get_global_logger()->debug("Loading snapshot");
    index_factory.load_index(SNAPSHOT_FOLDER_NAME);
}

void Persistence::save_last_snapshot_id() {
    std::filesystem::create_directories(SNAPSHOT_FOLDER_NAME);
    std::ofstream ofs;
    ofs.exceptions(std::ios::failbit | std::ios::badbit);
    ofs.open(std::filesystem::path(SNAPSHOT_FOLDER_NAME) / "max_log_id.txt");
    ofs << last_snapshot_id;
    get_global_logger()->debug("Saved last snapshot ID: {}", last_snapshot_id);
}

void Persistence::load_last_snapshot_id() {
    std::filesystem::path last_snapshot_id_file_path = std::filesystem::path(SNAPSHOT_FOLDER_NAME) / "max_log_id.txt";
    if (std::filesystem::exists(last_snapshot_id_file_path)) {
        std::ifstream ifs;
        ifs.exceptions(std::ios::failbit | std::ios::badbit);
        ifs.open(last_snapshot_id_file_path);
        ifs >> last_snapshot_id;
        get_global_logger()->debug("Loaded last snapshot ID: {}", last_snapshot_id);
    }
}
