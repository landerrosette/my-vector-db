#ifndef FILTERINDEX_H
#define FILTERINDEX_H


#include <cstdint>
#include <map>
#include <string>

#include "roaring/roaring.h"

class FilterIndex {
public:
    enum class Operation {
        EQUAL,
        NOT_EQUAL
    };

    FilterIndex() = default;

    void addIntFieldFilter(const std::string &fieldname, int64_t value, uint64_t id);

    void getIntFieldFilterBitmap(const std::string &fieldname, Operation op, int64_t value,
                                 roaring_bitmap_t *result_bitmap);

private:
    std::map<std::string, std::map<long, roaring_bitmap_t *> > intFieldFilter;
};


#endif //FILTERINDEX_H
