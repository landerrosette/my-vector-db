#ifndef LOGGER_H
#define LOGGER_H


#include <memory>

#include "spdlog/spdlog.h"

constexpr const char *GLOBAL_LOGGER_NAME = "vector-db";

void init_global_logger();

void set_global_log_level(spdlog::level::level_enum level);

std::shared_ptr<spdlog::logger> get_global_logger();


#endif //LOGGER_H
