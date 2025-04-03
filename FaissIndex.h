#ifndef FAISSINDEX_H
#define FAISSINDEX_H


#include <faiss/Index.h>
#include <vector>
#include <cstdint>

class FaissIndex {
public:
    explicit FaissIndex(faiss::Index *index) {}

    void insert_vectors(const std::vector<float> &data, uint64_t label);

private:
    faiss::Index *index;
};


#endif //FAISSINDEX_H
