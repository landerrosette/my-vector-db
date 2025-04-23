#include "FaissIndex.h"

#include <faiss/IndexIDMap.h>

#include "logger.h"

void FaissIndex::insert_vectors(const std::vector<float> &data, uint64_t label) {
    long id = static_cast<long>(label);
    index->add_with_ids(1, data.data(), &id);
}

std::pair<std::vector<long>, std::vector<float> > FaissIndex::search_vectors(
    const std::vector<float> &query, int k, const roaring_bitmap_t *bitmap) {
    int dim = index->d;
    int num_queries = query.size() / dim;
    std::vector<long> indices(num_queries * k);
    std::vector<float> distances(num_queries * k);

    faiss::SearchParameters search_params;
    RoaringBitmapIDSelector selector(bitmap);
    if (bitmap) search_params.sel = &selector;

    index->search(num_queries, query.data(), k, distances.data(), indices.data(), &search_params);
    // GlobalLogger->debug("Retrieved values:");
    for (int i = 0; i < indices.size(); ++i) {
        if (indices[i] == -1) continue;
        // GlobalLogger->debug("ID: {}, Distance: {}", indices[i], distances[i]);
    }

    return {indices, distances};
}

void FaissIndex::remove_vectors(const std::vector<long> &ids) {
    auto *id_map = dynamic_cast<faiss::IndexIDMap *>(index);
    faiss::IDSelectorBatch selector(ids.size(), ids.data());
    id_map->remove_ids(selector);
}
