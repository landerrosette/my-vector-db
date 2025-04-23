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

    // Check if the ID already exists
    rapidjson::Document existingData = scalar_storage_.get_scalar(id);
    GlobalLogger->debug("No existing data found for ID: {}, creating new entry", id);

    // Remove the existing vector from the index if it exists
    if (existingData.IsObject()) {
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

    // Insert the new vector into the index
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

    // Update the filter index
    FilterIndex *filter_index = static_cast<FilterIndex *>(getGlobalIndexFactory()->getIndex(
        IndexFactory::IndexType::FILTER));
    for (auto it = data.MemberBegin(); it != data.MemberEnd(); ++it) {
        std::string field_name = it->name.GetString();
        if (field_name != "id" && it->value.IsInt()) {
            int64_t field_value = it->value.GetInt64();
            int64_t *old_field_value_p = nullptr;
            if (existingData.IsObject()) {
                old_field_value_p = static_cast<int64_t *>(malloc(sizeof(int64_t)));
                *old_field_value_p = existingData[field_name.c_str()].GetInt64();
            }
            filter_index->updateIntFieldFilter(field_name, old_field_value_p, field_value, id);
            delete old_field_value_p;
        }
    }

    // Update the scalar storage with the new data
    scalar_storage_.insert_scalar(id, data);
}

std::pair<std::vector<long>, std::vector<float> > VectorDatabase::search(const rapidjson::Document &json_request) {
    std::vector<float> query;
    for (const auto &q: json_request["vectors"].GetArray())
        query.push_back(q.GetFloat());
    int k = json_request["k"].GetInt();

    auto indexType = getIndexTypeFromRequest(json_request);
    void *index = getGlobalIndexFactory()->getIndex(indexType);

    // Create a bitmap for filtering
    roaring_bitmap_t *filter_bitmap = nullptr;
    if (json_request.HasMember("filter") && json_request["filter"].IsObject()) {
        const auto &filter = json_request["filter"];
        std::string fieldName = filter["fieldName"].GetString();
        std::string op_str = filter["op"].GetString();
        int64_t value = filter["value"].GetInt64();
        auto op = op_str == "=" ? FilterIndex::Operation::EQUAL : FilterIndex::Operation::NOT_EQUAL;
        auto *filter_index = static_cast<FilterIndex *>(getGlobalIndexFactory()->getIndex(
            IndexFactory::IndexType::FILTER));
        filter_bitmap = roaring_bitmap_create();
        filter_index->getIntFieldFilterBitmap(fieldName, op, value, filter_bitmap);
    }

    std::pair<std::vector<long>, std::vector<float> > results;
    switch (indexType) {
        case IndexFactory::IndexType::FLAT: {
            auto *faiss_index = static_cast<FaissIndex *>(index);
            results = faiss_index->search_vectors(query, k, filter_bitmap);
            break;
        }
        case IndexFactory::IndexType::HNSW: {
            auto *hnsw_index = static_cast<HNSWLibIndex *>(index);
            results = hnsw_index->search_vectors(query, k, filter_bitmap);
            break;
        }
        default:
            break;
    }

    delete filter_bitmap;

    return results;
}
