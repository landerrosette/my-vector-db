#ifndef FAISSINDEX_H
#define FAISSINDEX_H


#include <cstdint>
#include <utility>
#include <vector>
#include <faiss/Index.h>

class FaissIndex {
public:
    explicit FaissIndex(faiss::Index *index) : index(index) {}

    void insert_vectors(const std::vector<float> &data, uint64_t label);

    std::pair<std::vector<long>, std::vector<float> > search_vectors(const std::vector<float> &query, int k);

    void remove_vectors(const std::vector<long> &ids);

private:
    faiss::Index *index;
};


#endif //FAISSINDEX_H
