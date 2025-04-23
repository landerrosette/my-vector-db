#include "HNSWLibIndex.h"

#include <stdexcept>

HNSWLibIndex::HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M,
                           int ef_construction) : dim(dim) {
    if (metric == IndexFactory::MetricType::L2) space = new hnswlib::L2Space(dim);
    else throw std::invalid_argument("Unsupported metric type");
    index = new hnswlib::HierarchicalNSW(space, num_data, M, ef_construction);
}

std::pair<std::vector<long>, std::vector<float> > HNSWLibIndex::search_vectors(const std::vector<float> &query, int k,
                                                                               const roaring_bitmap_t *bitmap,
                                                                               int ef_search) {
    index->setEf(ef_search);

    RoaringBitmapIDFilter selector(bitmap);

    auto result = index->searchKnn(query.data(), k, bitmap ? &selector : nullptr);
    std::vector<long> indices;
    std::vector<float> distances;
    while (!result.empty()) {
        auto item = result.top();
        indices.push_back(item.second);
        distances.push_back(item.first);
        result.pop();
    }

    return {indices, distances};
}
