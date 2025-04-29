#include "IndexFactory.h"

#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>

#include "FaissIndex.h"
#include "FilterIndex.h"
#include "HNSWLibIndex.h"

void IndexFactory::make_index(IndexType type, int dim, int num_data, MetricType metric) {
    switch (type) {
        case IndexType::FLAT: {
            faiss::MetricType faiss_metric = metric == MetricType::L2 ? faiss::METRIC_L2 : faiss::METRIC_INNER_PRODUCT;
            index_map[type] = std::make_unique<FaissIndex>(std::move(
                std::make_unique<faiss::IndexIDMap>(std::make_unique<faiss::IndexFlat>(dim, faiss_metric).release())));
            break;
        }
        case IndexType::HNSW:
            index_map[type] = std::make_unique<HNSWLibIndex>(dim, num_data, metric, 16, 200);
            break;
        case IndexType::FILTER:
            index_map[type] = std::make_unique<FilterIndex>();
            break;
        default:
            break;
    }
}

IndexBase *IndexFactory::get_index(IndexType type) const {
    if (auto it = index_map.find(type); it != index_map.end())
        return it->second.get();
    return nullptr;
}

void IndexFactory::save_index(const std::string &prefix) const {
    for (const auto &[type, index]: index_map)
        index->save_index(prefix + std::to_string(static_cast<int>(type)) + ".index");
}

void IndexFactory::load_index(const std::string &prefix) {
    for (const auto &[type, index]: index_map)
        index->load_index(prefix + std::to_string(static_cast<int>(type)) + ".index");
}
