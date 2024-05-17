#include "logger/logger.hpp"

GlobalLoggerPtr globalLogger = spdlog::basic_logger_mt("mhac", "logs/mhac_logs.txt", true);

void setLogLevel(spdlog::level::level_enum logLevel) {
    globalLogger->set_level(logLevel);
}
