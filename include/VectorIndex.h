#ifndef VECTORINDEX_H
#define VECTORINDEX_H


#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include "IndexBase.h"
#include "roaring/roaring.hh"

class VectorIndex : public IndexBase {
public:
    virtual void insert_vectors(const std::vector<float> &data, uint32_t id) = 0;

    virtual std::pair<std::vector<uint32_t>, std::vector<float> > search_vectors(
        const std::vector<float> &query, int k,
        std::optional<std::reference_wrapper<const roaring::Roaring> > bitmap = std::nullopt) const = 0;

    virtual void remove_vectors(const std::vector<uint32_t> &ids) = 0;
};


#endif //VECTORINDEX_H
