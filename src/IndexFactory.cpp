#include "IndexFactory.h"

#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>

#include "FaissIndex.h"
#include "FilterIndex.h"
#include "HNSWLibIndex.h"

void IndexFactory::init(IndexType type, int dim, int num_data, MetricType metric) {
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

IndexFactory &get_global_index_factory() {
    static IndexFactory global_index_factory;
    return global_index_factory;
}
