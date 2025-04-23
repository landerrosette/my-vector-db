#include "VectorDatabase.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "FaissIndex.h"
#include "FilterIndex.h"
#include "HNSWLibIndex.h"
#include "IndexFactory.h"
#include "logger.h"
#include "utils.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void VectorDatabase::upsert(uint64_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    GlobalLogger->debug("Upserting data: {}", buffer.GetString());

    rapidjson::Document existingData = scalar_storage_.get_scalar(id);
    GlobalLogger->debug("No existing data found for ID: {}, creating new entry", id);

    if (existingData.IsObject()) {
        // If the data already exists, we need to remove it from the index
        std::vector<float> existingVector(existingData["vectors"].Size());
        for (rapidjson::SizeType i = 0; i < existingData["vectors"].Size(); ++i)
            existingVector[i] = existingData["vectors"][i].GetFloat();
        void *index = getGlobalIndexFactory()->getIndex(index_type);
        switch (index_type) {
            case IndexFactory::IndexType::FLAT: {
                auto *faiss_index = static_cast<FaissIndex *>(index);
                faiss_index->remove_vectors({static_cast<long>(id)});
                break;
            }
            default:
                break;
        }
    }

    std::vector<float> newVector(data["vectors"].Size());
    for (rapidjson::SizeType i = 0; i < data["vectors"].Size(); ++i)
        newVector[i] = data["vectors"][i].GetFloat();
    void *index = getGlobalIndexFactory()->getIndex(index_type);
    switch (index_type) {
        case IndexFactory::IndexType::FLAT: {
            auto *faiss_index = static_cast<FaissIndex *>(index);
            faiss_index->insert_vectors(newVector, id);
            break;
        }
        case IndexFactory::IndexType::HNSW: {
            auto *hnsw_index = static_cast<HNSWLibIndex *>(index);
            hnsw_index->insert_vectors(newVector, id);
            break;
        }
        default:
            break;
    }

    scalar_storage_.insert_scalar(id, data);
}
