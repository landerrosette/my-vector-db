#include "VectorDatabase.h"

#include <optional>
#include <string>
#include <vector>

#include "FilterIndex.h"
#include "logger.h"
#include "utils.h"
#include "VectorIndex.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

void VectorDatabase::upsert(uint32_t id, const rapidjson::Document &data, IndexFactory::IndexType index_type) {
    auto *index = dynamic_cast<VectorIndex *>(index_factory->get_index(index_type));
    if (!index) return;

    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    data.Accept(writer);
    get_global_logger()->debug("Upserting data: {}", buffer.GetString());

    // Remove the existing vector from the index if it exists
    rapidjson::Document existing_data = scalar_storage.get_scalar(id);
    if (existing_data.IsObject()) {
        std::vector<float> existing_vector(existing_data["vectors"].Size());
        for (rapidjson::SizeType i = 0; i < existing_data["vectors"].Size(); ++i)
            existing_vector[i] = existing_data["vectors"][i].GetFloat();
        index->remove_vectors({id});
    } else
        get_global_logger()->debug("No existing data found for ID: {}, creating new entry", id);

    // Insert the new vector into the index
    std::vector<float> new_vector(data["vectors"].Size());
    for (rapidjson::SizeType i = 0; i < data["vectors"].Size(); ++i)
        new_vector[i] = data["vectors"][i].GetFloat();
    index->insert_vectors(new_vector, id);

    // Update the filter index
    auto *filter_index = dynamic_cast<FilterIndex *>(index_factory->get_index(IndexFactory::IndexType::FILTER));
    for (const auto &member: data.GetObject()) {
        if (std::string field_name = member.name.GetString(); field_name != "id" && member.value.IsInt()) {
            int field_value = member.value.GetInt();
            std::optional<int> old_field_value;
            if (existing_data.IsObject() && existing_data.HasMember(field_name.c_str()) && existing_data[field_name.
                    c_str()].IsInt())
                old_field_value = existing_data[field_name.c_str()].GetInt();
            filter_index->update_int_field_filter(field_name, old_field_value, field_value, id);
        }
    }

    // Update the scalar storage with the new data
    scalar_storage.insert_scalar(id, data);
}

std::pair<std::vector<uint32_t>, std::vector<float> > VectorDatabase::search(const rapidjson::Document &json_request) const {
    std::vector<float> query(json_request["vectors"].Size());
    for (rapidjson::SizeType i = 0; i < json_request["vectors"].Size(); ++i)
        query[i] = json_request["vectors"][i].GetFloat();
    int k = json_request["k"].GetInt();

    std::optional<roaring::Roaring> filter_bitmap;
    if (json_request.HasMember("filter") && json_request["filter"].IsObject()) {
        const auto &filter = json_request["filter"];
        std::string field_name = filter["fieldName"].GetString();
        std::string op_str = filter["op"].GetString();
        int value = filter["value"].GetInt();
        auto op = op_str == "=" ? FilterIndex::Operation::EQUAL : FilterIndex::Operation::NOT_EQUAL;
        auto *filter_index = dynamic_cast<FilterIndex *>(index_factory->get_index(IndexFactory::IndexType::FILTER));
        filter_bitmap = filter_index->get_int_field_filter_bitmap(field_name, op, value);
    }

    return index->search_vectors(query, k, filter_bitmap);
}

void VectorDatabase::reload_database() {
    get_global_logger()->info("Reloading vector database");
    persistence.load_snapshot();
    while (true) {
        auto [operation_type, json_data] = persistence.read_next_wal_log();
        if (operation_type.empty()) break;

        get_global_logger()->info("Operation type: {}", operation_type);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        json_data.Accept(writer);
        get_global_logger()->info("Data: {}", buffer.GetString());

        if (operation_type == "upsert") {
            uint32_t id = json_data["id"].GetUint();
            auto index_type = get_index_type_from_request(json_data);
            upsert(id, json_data, index_type);
        }
    }
}
