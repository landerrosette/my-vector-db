#ifndef HNSWLIBINDEX_H
#define HNSWLIBINDEX_H


#include "IndexFactory.h"
#include <vector>
#include <cstdint>
#include <utility>
#include "hnswlib/hnswlib.h"

class HNSWLibIndex {
public:
    HNSWLibIndex(int dim, int num_data, IndexFactory::MetricType metric, int M = 16, int ef_construction = 200);

    void insert_vectors(const std::vector<float> &data, uint64_t label) { index->addPoint(data.data(), label); }

    std::pair<std::vector<long>, std::vector<float> > search_vectors(const std::vector<float> &query, int k,
                                                                     int ef_search = 50);

private:
    int dim;
    hnswlib::SpaceInterface<float> *space;
    hnswlib::HierarchicalNSW<float> *index;
};


#endif //HNSWLIBINDEX_H
