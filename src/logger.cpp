#include "logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> global_logger;

void init_global_logger() {
    global_logger = spdlog::stdout_color_mt("global_logger");
}

void set_log_level(spdlog::level::level_enum level) {
    global_logger->set_level(level);
}
