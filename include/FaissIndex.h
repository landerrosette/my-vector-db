#ifndef FAISSINDEX_H
#define FAISSINDEX_H


#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <faiss/Index.h>
#include <faiss/impl/IDSelector.h>

#include "roaring/roaring.hh"

class FaissIndex {
public:
    explicit FaissIndex(std::unique_ptr<faiss::Index> index) : index(std::move(index)) {}

    void insert_vectors(const std::vector<float> &data, uint32_t id);

    std::pair<std::vector<uint32_t>, std::vector<float> > search_vectors(
        const std::vector<float> &query, int k,
        std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap = std::nullopt);

    void remove_vectors(const std::vector<uint32_t> &ids);

private:
    std::unique_ptr<faiss::Index> index;
};

struct RoaringBitmapIDSelector : public faiss::IDSelector {
    explicit RoaringBitmapIDSelector(const roaring::Roaring &bitmap) : bitmap(bitmap) {}

    bool is_member(faiss::idx_t id) const final { return bitmap.contains(id); }

private:
    const roaring::Roaring &bitmap;
};


#endif //FAISSINDEX_H
