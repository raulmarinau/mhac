#ifndef GLOBAL_LOGGER_HPP
#define GLOBAL_LOGGER_HPP

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/logger.h"

using GlobalLoggerPtr = std::shared_ptr<spdlog::logger>;
 
extern GlobalLoggerPtr globalLogger;

#endif // GLOBAL_LOGGER_HPP