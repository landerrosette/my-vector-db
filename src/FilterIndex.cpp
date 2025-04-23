#include "FilterIndex.h"

#include "logger.h"

void FilterIndex::addIntFieldFilter(const std::string &fieldName, int64_t value, uint64_t id) {
    roaring_bitmap_t *bitmap = roaring_bitmap_create();
    roaring_bitmap_add(bitmap, id);
    intFieldFilter[fieldName][value] = bitmap;
    GlobalLogger->debug("Added int filter for field: {}, value: {}, id: {}", fieldName, value, id);
}

void FilterIndex::updateIntFieldFilter(const std::string &fieldName, const int64_t *old_value, int64_t new_value,
                                       uint64_t id) {
    auto it = intFieldFilter.find(fieldName);
    if (it != intFieldFilter.end()) {
        auto &value_map = it->second;

        // Remove the old value bitmap if it exists
        auto old_bitmap_it = old_value ? value_map.find(*old_value) : value_map.end();
        if (old_bitmap_it != value_map.end()) roaring_bitmap_remove(old_bitmap_it->second, id);

        // Look for the new value bitmap
        auto new_bitmap_it = value_map.find(new_value);
        if (new_bitmap_it == value_map.end()) {
            // If it doesn't exist, create a new bitmap for the new value
            roaring_bitmap_t *new_bitmap = roaring_bitmap_create();
            value_map[new_value] = new_bitmap;
            new_bitmap_it = value_map.find(new_value);
        }
        roaring_bitmap_add(new_bitmap_it->second, id);

        if (old_value)
            GlobalLogger->debug("Updated int filter for field: {}, old value: {}, new value: {}, id: {}", fieldName,
                                *old_value, new_value, id);
        else
            GlobalLogger->debug("Updated int filter for field: {}, old value: null, new value: {}, id: {}", fieldName,
                                new_value, id);
    } else {
        addIntFieldFilter(fieldName, new_value, id);
    }
}

void FilterIndex::getIntFieldFilterBitmap(const std::string &fieldName, Operation op, int64_t value,
                                          roaring_bitmap_t *result_bitmap) {
    auto it = intFieldFilter.find(fieldName);
    if (it != intFieldFilter.end()) {
        auto &value_map = it->second;
        if (op == Operation::EQUAL) {
            auto bitmap_it = value_map.find(value);
            if (bitmap_it != value_map.end()) {
                roaring_bitmap_or_inplace(result_bitmap, bitmap_it->second);
                GlobalLogger->debug("Retrieved EQUAL bitmap for field: {}, value: {}", fieldName, value);
            }
        } else if (op == Operation::NOT_EQUAL) {
            for (const auto &[first, second]: value_map) {
                if (first != value) roaring_bitmap_or_inplace(result_bitmap, second);
            }
            GlobalLogger->debug("Retrieved NOT_EQUAL bitmap for field: {}, value: {}", fieldName, value);
        }
    }
}
