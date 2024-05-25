//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mapo
{
	Ref<spdlog::logger>					   Log::s_engineLogger;
	Ref<spdlog::logger>					   Log::s_appLogger;
	Ref<spdlog::sinks::ringbuffer_sink_mt> Log::s_ringbufferSink;

	void Log::Init()
	{
		s_ringbufferSink = MakeRef<spdlog::sinks::ringbuffer_sink_mt>(5);

		spdlog::set_pattern("%^%n [%l] (%s:%#) - %v%$");
		s_engineLogger = spdlog::stdout_color_mt("MAPO");
		s_engineLogger->set_level(spdlog::level::trace);

		s_appLogger = spdlog::stdout_color_mt("APP^");
		s_appLogger->set_level(spdlog::level::trace);

		s_engineLogger->sinks().push_back(s_ringbufferSink);
		s_appLogger->sinks().push_back(s_ringbufferSink);
	}

	String Log::GetLastMessage()
	{
		return s_ringbufferSink->last_formatted(1)[0];
	}

} // namespace Mapo
