#include "logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

void init_global_logger() {
    spdlog::stdout_color_mt(GLOBAL_LOGGER_NAME);
    spdlog::get(GLOBAL_LOGGER_NAME)->info("Global logger initialized");
}

void set_global_log_level(spdlog::level::level_enum level) {
    spdlog::get(GLOBAL_LOGGER_NAME)->set_level(level);
}

std::shared_ptr<spdlog::logger> get_global_logger() {
    return spdlog::get(GLOBAL_LOGGER_NAME);
}
