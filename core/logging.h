//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include "core/typedefs.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// This ignores all warnings raised inside external headers.
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#pragma warning(pop)

namespace Mapo
{
	class Log
	{
	public:
		virtual ~Log() = default;

		static void Init();

		static Ref<spdlog::logger>& GetEngineLogger() { return s_engineLogger; }
		static Ref<spdlog::logger>& GetAppLogger() { return s_appLogger; }

		static String GetLastMessage();

	private:
		static Ref<spdlog::logger> s_engineLogger;
		static Ref<spdlog::logger> s_appLogger;

		static Ref<spdlog::sinks::ringbuffer_sink_mt> s_ringbufferSink;
	};

} // namespace Mapo

// Looks like these functions didn't work.
// Can't fix. https://github.com/gabime/spdlog/issues/1227

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Engine
#define MP_TRACE(...) SPDLOG_LOGGER_TRACE(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)
#define MP_DEBUG(...) SPDLOG_LOGGER_DEBUG(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)
#define MP_INFO(...) SPDLOG_LOGGER_INFO(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)
#define MP_WARN(...) SPDLOG_LOGGER_WARN(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)
#define MP_ERROR(...) SPDLOG_LOGGER_ERROR(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)
#define MP_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::Mapo::Log::GetEngineLogger(), __VA_ARGS__)

// App
#define MP_APP_TRACE(...) SPDLOG_LOGGER_TRACE(::Mapo::Log::GetAppLogger(), __VA_ARGS__)
#define MP_APP_DEBUG(...) SPDLOG_LOGGER_DEBUG(::Mapo::Log::GetAppLogger(), __VA_ARGS__)
#define MP_APP_INFO(...) SPDLOG_LOGGER_INFO(::Mapo::Log::GetAppLogger(), __VA_ARGS__)
#define MP_APP_WARN(...) SPDLOG_LOGGER_WARN(::Mapo::Log::GetAppLogger(), __VA_ARGS__)
#define MP_APP_ERROR(...) SPDLOG_LOGGER_ERROR(::Mapo::Log::GetAppLogger(), __VA_ARGS__)
#define MP_APP_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::Mapo::Log::GetAppLogger(), __VA_ARGS__)

#define MP_NEWLINE(x) printf(x "\n")

// Use printf
// #include <cstdio>
/*
#define MP_PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define MP_INFO(fmt, ...) printf("[INFO] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_WARN(fmt, ...) printf("[WARN] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_ERROR(fmt, ...) printf("[ERROR] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_DEBUG(fmt, ...) printf("[DEBUG] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define MP_WARN_IF(exp, fmt, ...)        \
	do                                   \
	{                                    \
		if (exp)                         \
			MP_WARN(fmt, ##__VA_ARGS__); \
	}                                    \
	while (0)

#define MP_ERROR_IF(exp, fmt, ...)        \
	do                                    \
	{                                     \
		if (exp)                          \
			MP_ERROR(fmt, ##__VA_ARGS__); \
	}                                     \
	while (0)

#define MP_DEBUG_IF(exp, fmt, ...)        \
	do                                    \
	{                                     \
		if (exp)                          \
			MP_DEBUG(fmt, ##__VA_ARGS__); \
	}                                     \
	while (0)

#define MP_NEWLINE(x) printf(x "\n")
*/
