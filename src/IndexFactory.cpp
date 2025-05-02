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

void IndexFactory::save_index(const std::filesystem::path &directory_path) const {
    std::filesystem::create_directories(directory_path);
    for (const auto &[type, index]: index_map) {
        std::string file_name = std::to_string(static_cast<int>(type)) + ".index";
        index->save_index(directory_path / file_name);
    }
}

void IndexFactory::load_index(const std::filesystem::path &directory_path) {
    for (const auto &[type, index]: index_map) {
        std::string file_name = std::to_string(static_cast<int>(type)) + ".index";
        index->load_index(directory_path / file_name);
    }
}
