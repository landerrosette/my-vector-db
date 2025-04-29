#include "FaissIndex.h"

#include <utility>
#include <faiss/IndexIDMap.h>

#include "logger.h"

void FaissIndex::insert_vectors(const std::vector<float> &data, uint32_t id) {
    faiss::idx_t faiss_idx = id;
    index->add_with_ids(1, data.data(), &faiss_idx);
}

std::pair<std::vector<uint32_t>, std::vector<float> > FaissIndex::search_vectors(
    const std::vector<float> &query, int k,
    std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap) const {
    int dim = index->d;
    int num_queries = query.size() / dim;
    std::vector<faiss::idx_t> indices(num_queries * k);
    std::vector<float> distances(num_queries * k);

    faiss::SearchParameters search_params;
    std::optional<RoaringBitmapIDSelector> selector;
    if (bitmap) {
        selector.emplace(*bitmap);
        search_params.sel = &*selector;
    }

    index->search(num_queries, query.data(), k, distances.data(), indices.data(), &search_params);
    std::vector<uint32_t> ids;
    for (const auto &idx: indices) {
        if (idx == -1) continue;
        ids.push_back(static_cast<uint32_t>(idx));
    }

    return {std::move(ids), std::move(distances)};
}

void FaissIndex::remove_vectors(const std::vector<uint32_t> &ids) {
    std::vector<faiss::idx_t> indices(ids.begin(), ids.end());
    faiss::IDSelectorBatch selector(ids.size(), indices.data());
    index->remove_ids(selector);
}

void FaissIndex::load_index(const std::filesystem::path &file_path) {
    if (std::filesystem::exists(file_path))
        index.reset(faiss::read_index(file_path.c_str()));
    else
        get_global_logger()->info("File {} does not exist, skipping load", file_path);
}
