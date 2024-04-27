//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cstdio>

// #define LOG(...)                             \
// 	printf("[%s:%d] ", __FUNCTION__, __LINE__); \
// 	printf(__VA_ARGS__);                        \
// 	printf("\n")

#define MP_PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define MP_INFO(fmt, ...) printf("[INFO] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_WARN(fmt, ...) printf("[WARN] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_ERROR(fmt, ...) printf("[ERROR] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MP_DEBUG(fmt, ...) printf("[DEBUG] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define WARN_IF(exp, fmt, ...)           \
	do                                   \
	{                                    \
		if (exp)                         \
			MP_WARN(fmt, ##__VA_ARGS__); \
	}                                    \
	while (0)

#define ERROR_IF(exp, fmt, ...)           \
	do                                    \
	{                                     \
		if (exp)                          \
			MP_ERROR(fmt, ##__VA_ARGS__); \
	}                                     \
	while (0)

#define DEBUG_IF(exp, fmt, ...)           \
	do                                    \
	{                                     \
		if (exp)                          \
			MP_DEBUG(fmt, ##__VA_ARGS__); \
	}                                     \
	while (0)

#define NEWLINE(x) printf(x "\n")
