#ifndef FILTERINDEX_H
#define FILTERINDEX_H


#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "IndexBase.h"
#include "roaring/roaring.hh"

class FilterIndex : public IndexBase {
public:
    enum class Operation {
        EQUAL,
        NOT_EQUAL
    };

    FilterIndex() = default;

    void add_int_field_filter(const std::string &field_name, int value, uint32_t id);

    void update_int_field_filter(const std::string &field_name, std::optional<int> old_value, int new_value,
                                 uint32_t id);

    roaring::Roaring get_int_field_filter_bitmap(const std::string &field_name, Operation op, int value) const;

    void serialize_int_field_filter(std::ostream &os) const;

    void deserialize_int_field_filter(std::istream &is);

    void save_index(const std::filesystem::path &file_path) const override;

    void load_index(const std::filesystem::path &file_path) override;

private:
    std::unordered_map<std::string, std::unordered_map<int, roaring::Roaring> > int_field_filter;
};


#endif //FILTERINDEX_H
