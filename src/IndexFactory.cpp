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

void IndexFactory::save_index(const std::string &prefix, ScalarStorage &scalar_storage) {
    for (const auto &[type, index]: index_map) {
        std::string file_path = prefix + std::to_string(static_cast<int>(type)) + ".index";
        switch (type) {
            case IndexType::FLAT:
                std::get<std::unique_ptr<FaissIndex> >(index)->save_index(file_path);
                break;
            case IndexType::HNSW:
                std::get<std::unique_ptr<HNSWLibIndex> >(index)->save_index(file_path);
                break;
            case IndexType::FILTER:
                std::get<std::unique_ptr<FilterIndex> >(index)->save_index(scalar_storage, file_path);
                break;
            default:
                break;
        }
    }
}

void IndexFactory::load_index(const std::string &prefix, ScalarStorage &scalar_storage) {
    for (const auto &[type, index]: index_map) {
        std::string file_path = prefix + std::to_string(static_cast<int>(type)) + ".index";
        switch (type) {
            case IndexType::FLAT:
                std::get<std::unique_ptr<FaissIndex> >(index)->load_index(file_path);
                break;
            case IndexType::HNSW:
                std::get<std::unique_ptr<HNSWLibIndex> >(index)->load_index(file_path);
                break;
            case IndexType::FILTER:
                std::get<std::unique_ptr<FilterIndex> >(index)->load_index(scalar_storage, file_path);
                break;
            default:
                break;
        }
    }
}

IndexFactory &get_global_index_factory() {
    static IndexFactory global_index_factory;
    return global_index_factory;
}
