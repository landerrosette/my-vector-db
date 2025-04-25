#ifndef LOGGER_H
#define LOGGER_H


#include <memory>

#include "spdlog/spdlog.h"

extern std::shared_ptr<spdlog::logger> global_logger;

void init_global_logger();

void set_log_level(spdlog::level::level_enum level);


#endif //LOGGER_H
