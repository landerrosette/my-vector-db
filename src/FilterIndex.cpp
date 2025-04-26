#include "FilterIndex.h"

#include <cstddef>
#include <sstream>
#include <vector>

#include "logger.h"

void FilterIndex::add_int_field_filter(const std::string &field_name, int value, uint32_t id) {
    int_field_filter[field_name][value].add(id);
    global_logger->debug("Added int filter for field: {}, value: {}, id: {}", field_name, value, id);
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

        global_logger->debug("Updated int filter for field: {}, old value: {}, new value: {}, id: {}", field_name,
                             old_value ? std::to_string(*old_value) : "null", new_value, id);
    } else {
        // If the field doesn't exist, create a new entry
        add_int_field_filter(field_name, new_value, id);
    }
}

roaring::Roaring FilterIndex::get_int_field_filter_bitmap(const std::string &field_name, Operation op, int value) {
    roaring::Roaring result_bitmap;
    if (auto it = int_field_filter.find(field_name); it != int_field_filter.end()) {
        auto &value_map = it->second;
        switch (op) {
            case Operation::EQUAL: {
                auto bitmap_it = value_map.find(value);
                if (bitmap_it != value_map.end()) {
                    result_bitmap = bitmap_it->second;
                    global_logger->debug("Retrieved EQUAL bitmap for field: {}, value: {}", field_name, value);
                }
            }
            case Operation::NOT_EQUAL:
            default:
                for (const auto &[v, bitmap]: value_map)
                    if (v != value) result_bitmap |= bitmap;
                global_logger->debug("Retrieved NOT_EQUAL bitmap for field: {}, value: {}", field_name, value);
        }
    }
    return result_bitmap;
}

std::string FilterIndex::serialize_int_field_filter() {
    std::ostringstream oss;
    for (const auto &[field_name, value_map]: int_field_filter) {
        for (const auto &[value, bitmap]: value_map) {
            uint32_t size = bitmap.getSizeInBytes();
            std::vector<std::byte> serialized_bitmap(size);
            bitmap.write(reinterpret_cast<char *>(serialized_bitmap.data()));
            oss << field_name << "|" << value << "|";
            oss.write(reinterpret_cast<char *>(serialized_bitmap.data()), size);
            oss << "\n";
        }
    }
    return oss.str();
}

void FilterIndex::deserialize_int_field_filter(const std::string &serialized_data) {
    std::istringstream iss(serialized_data);
    for (std::string line; std::getline(iss, line);) {
        std::istringstream iss_line(line);
        std::string field_name, value_str;
        std::getline(iss_line, field_name, '|');
        std::getline(iss_line, value_str, '|');
        int value = std::stoi(value_str);
        std::vector<std::byte> serialized_bitmap;
        while (iss_line) {
            std::byte byte;
            iss_line.read(reinterpret_cast<char *>(&byte), sizeof(std::byte));
            serialized_bitmap.push_back(byte);
        }
        roaring::Roaring bitmap = roaring::Roaring::readSafe(reinterpret_cast<char *>(serialized_bitmap.data()),
                                                             serialized_bitmap.size());
        int_field_filter[field_name][value] = bitmap;
    }
}
