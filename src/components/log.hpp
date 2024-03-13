#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

extern spdlog::logger* logger;

void init_logger(void);

void deinit_logger(void);
