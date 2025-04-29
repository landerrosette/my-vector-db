#ifndef UTILS_H
#define UTILS_H


#include "httplib.h"
#include "IndexFactory.h"
#include "rapidjson/document.h"

IndexFactory::IndexType get_index_type_from_request(const rapidjson::Document &json_request);

void set_json_response(const rapidjson::Document &json_response, httplib::Response &res);


#endif //UTILS_H
