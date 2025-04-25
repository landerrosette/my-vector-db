#include "HNSWLibIndex.h"

#include <utility>

HNSWLibIndex::HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M, int ef_construction) {
    if (metric == IndexFactory::MetricType::L2)
        space = std::make_unique<hnswlib::L2Space>(dim);
    else
        space = std::make_unique<hnswlib::InnerProductSpace>(dim);
    index = std::make_unique<hnswlib::HierarchicalNSW<float> >(space.get(), num_data, M, ef_construction);
}

std::pair<std::vector<uint32_t>, std::vector<float> > HNSWLibIndex::search_vectors(
    const std::vector<float> &query, int k, std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap,
    int ef_search) {
    index->setEf(ef_search);

    std::optional<RoaringBitmapIDFilter> selector;
    if (bitmap) selector.emplace(*bitmap);

    std::vector<uint32_t> ids;
    std::vector<float> distances;
    for (auto result = index->searchKnn(query.data(), k, bitmap ? &*selector : nullptr); !result.empty();
         result.pop()) {
        const auto &[distance, id] = result.top();
        ids.push_back(id);
        distances.push_back(distance);
    }

    return {std::move(ids), std::move(distances)};
}
