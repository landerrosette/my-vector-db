#ifndef UTILS_H
#define UTILS_H


#include "IndexFactory.h"
#include "rapidjson/document.h"

IndexFactory::IndexType getIndexTypeFromRequest(const rapidjson::Document &json_request);


#endif //UTILS_H
