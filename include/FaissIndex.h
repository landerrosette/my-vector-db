#ifndef FAISSINDEX_H
#define FAISSINDEX_H


#include <cstdint>
#include <utility>
#include <vector>
#include <faiss/Index.h>
#include <faiss/impl/IDSelector.h>
#include "roaring/roaring.h"

class FaissIndex {
public:
    explicit FaissIndex(faiss::Index *index) : index(index) {}

    void insert_vectors(const std::vector<float> &data, uint64_t label);

    std::pair<std::vector<long>, std::vector<float> > search_vectors(const std::vector<float> &query, int k,
                                                                     const roaring_bitmap_t *bitmap = nullptr);

    void remove_vectors(const std::vector<long> &ids);

private:
    faiss::Index *index;
};

struct RoaringBitmapIDSelector : faiss::IDSelector {
    explicit RoaringBitmapIDSelector(const roaring_bitmap_t *bitmap) : bitmap_(bitmap) {}

    bool is_member(int64_t id) const final { return roaring_bitmap_contains(bitmap_, static_cast<uint32_t>(id)); }

    ~RoaringBitmapIDSelector() override = default;

    const roaring_bitmap_t *bitmap_;
};


#endif //FAISSINDEX_H
