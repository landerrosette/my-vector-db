#include "FilterIndex.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "logger.h"

void FilterIndex::add_int_field_filter(const std::string &field_name, int value, uint32_t id) {
    int_field_filter[field_name][value].add(id);
    get_global_logger()->debug("Added int filter for field: {}, value: {}, id: {}", field_name, value, id);
}

void FilterIndex::update_int_field_filter(const std::string &field_name, std::optional<int> old_value, int new_value,
                                          uint32_t id) {
    if (auto it = int_field_filter.find(field_name); it != int_field_filter.end()) {
        auto &value_map = it->second;

        // Look for the bitmap for the old value and remove the id from it
        if (old_value) {
            auto old_bitmap_it = value_map.find(*old_value);
            if (old_bitmap_it != value_map.end()) old_bitmap_it->second.remove(id);
        }

        // Add the id to the new value
        value_map[new_value].add(id);

        get_global_logger()->debug("Updated int filter for field: {}, old value: {}, new value: {}, id: {}", field_name,
                                   old_value ? std::to_string(*old_value) : "null", new_value, id);
    } else {
        // If the field doesn't exist, create a new entry
        add_int_field_filter(field_name, new_value, id);
    }
}

roaring::Roaring
FilterIndex::get_int_field_filter_bitmap(const std::string &field_name, Operation op, int value) const {
    roaring::Roaring result_bitmap;
    if (auto it = int_field_filter.find(field_name); it != int_field_filter.end()) {
        auto &value_map = it->second;
        switch (op) {
            case Operation::EQUAL: {
                auto bitmap_it = value_map.find(value);
                if (bitmap_it != value_map.end()) {
                    result_bitmap = bitmap_it->second;
                    get_global_logger()->debug("Retrieved EQUAL bitmap for field: {}, value: {}", field_name, value);
                }
                break;
            }
            case Operation::NOT_EQUAL:
            default:
                for (const auto &[v, bitmap]: value_map)
                    if (v != value) result_bitmap |= bitmap;
                get_global_logger()->debug("Retrieved NOT_EQUAL bitmap for field: {}, value: {}", field_name, value);
                break;
        }
    }
    return result_bitmap;
}

void FilterIndex::serialize_int_field_filter(std::ostream &os) const {
    for (const auto &[field_name, value_map]: int_field_filter) {
        for (const auto &[value, bitmap]: value_map) {
            uint32_t size = bitmap.getSizeInBytes();
            std::vector<char> serialized_bitmap(size);
            bitmap.write(serialized_bitmap.data());
            os << field_name << "|" << value << "|";
            os.write(serialized_bitmap.data(), size);
            os << "\n";
        }
    }
}

void FilterIndex::deserialize_int_field_filter(std::istream &is) {
    for (std::string line; std::getline(is, line);) {
        std::istringstream iss_line(line);
        std::string field_name, value_str;
        std::getline(iss_line, field_name, '|');
        std::getline(iss_line, value_str, '|');
        int value = std::stoi(value_str);
        std::vector<char> serialized_bitmap(std::istreambuf_iterator<char>(iss_line), {});
        roaring::Roaring bitmap = roaring::Roaring::readSafe(serialized_bitmap.data(), serialized_bitmap.size());
        int_field_filter[field_name][value] = bitmap;
    }
}

void FilterIndex::save_index(const std::filesystem::path &file_path) const {
    std::ofstream ofs(file_path, std::ios::binary);
    if (!ofs.is_open())
        throw std::runtime_error("Error opening " + file_path.string() + ": " + std::strerror(errno));
    serialize_int_field_filter(ofs);
    if (!ofs)
        throw std::runtime_error("Error saving index to file: " + std::string(std::strerror(errno)));
}

void FilterIndex::load_index(const std::filesystem::path &file_path) {
    if (std::filesystem::exists(file_path)) {
        std::ifstream ifs(file_path, std::ios::binary);
        if (!ifs.is_open())
            throw std::runtime_error("Error opening " + file_path.string() + ": " + std::strerror(errno));
        deserialize_int_field_filter(ifs);
        if (!ifs && !ifs.eof())
            throw std::runtime_error("Error loading index from file: " + std::string(std::strerror(errno)));
    } else
        get_global_logger()->info("File {} does not exist, skipping load", file_path.string());
}
