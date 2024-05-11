//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mapo
{
	Ref<spdlog::logger> Log::s_engineLogger;
	Ref<spdlog::logger> Log::s_appLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^%n [%l] (%s:%#) - %v%$");
		s_engineLogger = spdlog::stdout_color_mt("MAPO");
		s_engineLogger->set_level(spdlog::level::trace);

		s_appLogger = spdlog::stdout_color_mt("APP");
		s_appLogger->set_level(spdlog::level::trace);
	}

} // namespace Mapo
