#include "FilterIndex.h"
#include "logger.h"

void FilterIndex::addIntFieldFilter(const std::string &fieldname, int64_t value, uint64_t id) {
    roaring_bitmap_t *bitmap = roaring_bitmap_create();
    roaring_bitmap_add(bitmap, id);
    intFieldFilter[fieldname][value] = bitmap;
    GlobalLogger->debug("Added int filter for field: {}, value: {}, id: {}", fieldname, value, id);
}

void FilterIndex::getIntFieldFilterBitmap(const std::string &fieldname, Operation op, int64_t value,
    roaring_bitmap_t *result_bitmap) {
    auto it = intFieldFilter.find(fieldname);
    if (it != intFieldFilter.end()) {
        auto &value_map = it->second;
        if (op == Operation::EQUAL) {
            auto bitmap_it = value_map.find(value);
            if (bitmap_it != value_map.end()) {
                roaring_bitmap_or_inplace(result_bitmap, bitmap_it->second);
                GlobalLogger->debug("Retrieved EQUAL bitmap for field: {}, value: {}", fieldname, value);
            }
        } else if (op == Operation::NOT_EQUAL) {
            for (const auto &entry: value_map) {
                if (entry.first != value)
                    roaring_bitmap_or_inplace(result_bitmap, entry.second);
            }
            GlobalLogger->debug("Retrieved NOT_EQUAL bitmap for field: {}, value: {}", fieldname, value);
        }
    }
}
