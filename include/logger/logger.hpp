#ifndef MHAC_GLOBAL_LOGGER_HPP
#define MHAC_GLOBAL_LOGGER_HPP

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>

using GlobalLoggerPtr = std::shared_ptr<spdlog::logger>;
 
extern GlobalLoggerPtr globalLogger;

#endif // MHAC_GLOBAL_LOGGER_HPP