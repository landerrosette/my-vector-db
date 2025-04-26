#ifndef HNSWLIBINDEX_H
#define HNSWLIBINDEX_H


#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "IndexFactory.h"
#include "hnswlib/hnswlib.h"
#include "roaring/roaring.hh"

class HNSWLibIndex {
public:
    HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M = 16, int ef_construction = 200);

    void insert_vectors(const std::vector<float> &data, uint32_t id) { index->addPoint(data.data(), id); }

    std::pair<std::vector<uint32_t>, std::vector<float> > search_vectors(
        const std::vector<float> &query, int k,
        std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap = std::nullopt, int ef_search = 50);

private:
    class RoaringBitmapIDFilter : public hnswlib::BaseFilterFunctor {
    public:
        explicit RoaringBitmapIDFilter(const roaring::Roaring &bitmap) : bitmap(bitmap) {}

        bool operator()(hnswlib::labeltype label) final { return bitmap.contains(label); }

    private:
        const roaring::Roaring &bitmap;
    };

    std::unique_ptr<hnswlib::SpaceInterface<float> > space;
    std::unique_ptr<hnswlib::HierarchicalNSW<float> > index;
};


#endif //HNSWLIBINDEX_H
