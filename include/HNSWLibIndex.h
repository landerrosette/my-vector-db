#ifndef HNSWLIBINDEX_H
#define HNSWLIBINDEX_H


#include <cstdint>
#include <utility>
#include <vector>

#include "IndexFactory.h"
#include "hnswlib/hnswlib.h"
#include "roaring/roaring.h"

class HNSWLibIndex {
public:
    HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M = 16, int ef_construction = 200);

    void insert_vectors(const std::vector<float> &data, uint64_t label) { index->addPoint(data.data(), label); }

    std::pair<std::vector<long>, std::vector<float> > search_vectors(const std::vector<float> &query, int k,
                                                                     const roaring_bitmap_t *bitmap = nullptr,
                                                                     int ef_search = 50);

    class RoaringBitmapIDFilter : public hnswlib::BaseFilterFunctor {
    public:
        explicit RoaringBitmapIDFilter(const roaring_bitmap_t *bitmap) : bitmap_(bitmap) {}

        bool operator()(hnswlib::labeltype label) override {
            return roaring_bitmap_contains(bitmap_, static_cast<uint32_t>(label));
        }

    private:
        const roaring_bitmap_t *bitmap_;
    };

private:
    int dim;
    hnswlib::SpaceInterface<float> *space;
    hnswlib::HierarchicalNSW<float> *index;
};


#endif //HNSWLIBINDEX_H
