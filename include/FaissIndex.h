#ifndef FAISSINDEX_H
#define FAISSINDEX_H


#include <memory>
#include <faiss/Index.h>
#include <faiss/index_io.h>
#include <faiss/impl/IDSelector.h>

#include "VectorIndex.h"

class FaissIndex : public VectorIndex {
public:
    explicit FaissIndex(std::unique_ptr<faiss::Index> index) : index(std::move(index)) {}

    void insert_vectors(const std::vector<float> &data, uint32_t id) override;

    std::pair<std::vector<uint32_t>, std::vector<float> > search_vectors(
        const std::vector<float> &query, int k,
        std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap = std::nullopt) const override;

    void remove_vectors(const std::vector<uint32_t> &ids) override;

    void save_index(const std::filesystem::path &file_path) const override {
        faiss::write_index(index.get(), file_path.c_str());
    }

    void load_index(const std::filesystem::path &file_path) override;

private:
    class RoaringBitmapIDSelector : public faiss::IDSelector {
    public:
        explicit RoaringBitmapIDSelector(const roaring::Roaring &bitmap) : bitmap(bitmap) {}

        bool is_member(faiss::idx_t id) const final { return bitmap.contains(id); }

    private:
        const roaring::Roaring &bitmap;
    };

    std::unique_ptr<faiss::Index> index;
};


#endif //FAISSINDEX_H
