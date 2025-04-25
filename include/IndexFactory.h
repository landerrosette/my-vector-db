#ifndef INDEXFACTORY_H
#define INDEXFACTORY_H


#include <map>
#include <memory>
#include <variant>

class FaissIndex;
class HNSWLibIndex;
class FilterIndex;

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

    using IndexVariant = std::variant<
        std::unique_ptr<FaissIndex>,
        std::unique_ptr<HNSWLibIndex>,
        std::unique_ptr<FilterIndex>
    >;

    void init(IndexType type, int dim = 1, int num_data = 0, MetricType metric = MetricType::L2);

    template<typename T>
    T *get_index(IndexType type) const;

private:
    std::map<IndexType, IndexVariant> index_map;
};

template<typename T>
T *IndexFactory::get_index(IndexType type) const {
    if (auto it = index_map.find(type); it != index_map.end())
        return std::get_if<std::unique_ptr<T> >(&it->second)->get();
    return nullptr;
}

IndexFactory &get_global_index_factory();


#endif //INDEXFACTORY_H
