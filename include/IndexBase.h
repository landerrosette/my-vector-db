#ifndef INDEXBASE_H
#define INDEXBASE_H


#include <filesystem>

class IndexBase {
public:
    virtual ~IndexBase() = default;

    virtual void save_index(const std::filesystem::path &file_path) const = 0;

    virtual void load_index(const std::filesystem::path &file_path) = 0;
};


#endif //INDEXBASE_H
