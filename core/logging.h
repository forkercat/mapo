//
// Created by Junhao Wang (@forkercat) on 3/2/24.
//

#pragma once

#include <cstdio>

// #define LOG(...)                             \
// 	printf("[%s:%d] ", __FUNCTION__, __LINE__); \
// 	printf(__VA_ARGS__);                        \
// 	printf("\n")

#define PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define INFO(fmt, ...) printf("[INFO] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WARN(fmt, ...) printf("[WARN] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ERROR(fmt, ...) printf("[ERROR] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("[DEBUG] (%s:%d) - " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define WARN_IF(exp, fmt, ...)        \
	do                                \
	{                                 \
		if (exp)                      \
			WARN(fmt, ##__VA_ARGS__); \
	}                                 \
	while (0)

#define ERROR_IF(exp, fmt, ...)        \
	do                                 \
	{                                  \
		if (exp)                       \
			ERROR(fmt, ##__VA_ARGS__); \
	}                                  \
	while (0)

#define DEBUG_IF(exp, fmt, ...)        \
	do                                 \
	{                                  \
		if (exp)                       \
			DEBUG(fmt, ##__VA_ARGS__); \
	}                                  \
	while (0)

#define NEWLINE(x) printf(x "\n")
