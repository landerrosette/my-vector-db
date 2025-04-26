#ifndef FILTERINDEX_H
#define FILTERINDEX_H


#include <map>
#include <optional>
#include <string>

#include "ScalarStorage.h"
#include "roaring/roaring.hh"

class FilterIndex {
public:
    enum class Operation {
        EQUAL,
        NOT_EQUAL
    };

    FilterIndex() = default;

    void add_int_field_filter(const std::string &field_name, int value, uint32_t id);

    void update_int_field_filter(const std::string &field_name, std::optional<int> old_value, int new_value,
                                 uint32_t id);

    roaring::Roaring get_int_field_filter_bitmap(const std::string &field_name, Operation op, int value);

    std::string serialize_int_field_filter();

    void deserialize_int_field_filter(const std::string &serialized_data);

    void save_index(ScalarStorage &scalar_storage, const std::string &key) { scalar_storage.put(key, serialize_int_field_filter()); }

    void load_index(ScalarStorage &scalar_storage, const std::string &key) { deserialize_int_field_filter(scalar_storage.get(key)); }

private:
    std::map<std::string, std::map<int, roaring::Roaring> > int_field_filter;
};


#endif //FILTERINDEX_H
