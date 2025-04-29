#ifndef INDEXFACTORY_H
#define INDEXFACTORY_H


#include <map>
#include <memory>
#include <string>

#include "IndexBase.h"

class IndexFactory {
public:
    enum class IndexType {
        FLAT,
        HNSW,
        FILTER,
        UNKNOWN = -1
    };

    enum class MetricType {
        L2,
        IP
    };

    void make_index(IndexType type, int dim = 1, int num_data = 0, MetricType metric = MetricType::L2);

    IndexBase *get_index(IndexType type) const;

    void save_index(const std::string &prefix) const;

    void load_index(const std::string &prefix);

private:
    std::map<IndexType, std::unique_ptr<IndexBase> > index_map;
};


#endif //INDEXFACTORY_H
