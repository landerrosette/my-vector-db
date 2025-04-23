#include "IndexFactory.h"

#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>

#include "FaissIndex.h"
#include "FilterIndex.h"
#include "HNSWLibIndex.h"

namespace {
    IndexFactory globalIndexFactory;
}

void IndexFactory::init(IndexType type, int dim, int num_data, MetricType metric) {
    faiss::MetricType faiss_metric = metric == MetricType::L2 ? faiss::METRIC_L2 : faiss::METRIC_INNER_PRODUCT;
    switch (type) {
        case IndexType::FLAT:
            index_map[type] = new FaissIndex(new faiss::IndexIDMap(new faiss::IndexFlat(dim, faiss_metric)));
            break;
        case IndexType::HNSW:
            index_map[type] = new HNSWLibIndex(dim, num_data, metric, 16, 200);
            break;
        case IndexType::FILTER:
            index_map[type] = new FilterIndex();
            break;
        default:
            break;
    }
}

void *IndexFactory::getIndex(IndexType type) const {
    auto it = index_map.find(type);
    if (it != index_map.end()) return it->second;
    return nullptr;
}

IndexFactory *getGlobalIndexFactory() {
    return &globalIndexFactory;
}
