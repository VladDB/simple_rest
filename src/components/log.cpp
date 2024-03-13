#include "log.hpp"

spdlog::logger *logger;

void init_logger(void)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("simple_rest.log", 1024 * 1024, 3);
    logger = new spdlog::logger("logger", {console_sink, file_sink});

#ifdef DEBUG
    // logger->set_level(spdlog::level::debug);
    logger->set_level(spdlog::level::trace);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->set_pattern("[%Y-%m-%d %T.%e] [%05t] [%L] %v");
}

void deinit_logger(void)
{
    delete logger;
    logger = NULL;
}