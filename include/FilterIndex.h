#ifndef FILTERINDEX_H
#define FILTERINDEX_H


#include <map>
#include <optional>
#include <string>

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

private:
    std::map<std::string, std::map<int, roaring::Roaring> > int_field_filter;
};


#endif //FILTERINDEX_H
