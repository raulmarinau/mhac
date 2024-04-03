#include "logger/logger.hpp"

GlobalLoggerPtr globalLogger = spdlog::basic_logger_mt("spdlogger", "logs/mhac_logs.txt", true);