#include "HNSWLibIndex.h"
#include <stdexcept>

HNSWLibIndex::HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M,
                           int ef_construction) : dim(dim) {
    if (metric == IndexFactory::MetricType::L2) space = new hnswlib::L2Space(dim);
    else throw std::invalid_argument("Unsupported metric type");
    index = new hnswlib::HierarchicalNSW(space, num_data, M, ef_construction);
}

std::pair<std::vector<long>, std::vector<float> > HNSWLibIndex::search_vectors(const std::vector<float> &query, int k,
                                                                               int ef_search) {
    index->setEf(ef_search);
    auto result = index->searchKnn(query.data(), k);
    std::vector<long> indices(k);
    std::vector<float> distances(k);
    for (int i = 0; i < k; ++i) {
        auto item = result.top();
        indices[i] = item.second;
        distances[i] = item.first;
        result.pop();
    }
    return {indices, distances};
}
