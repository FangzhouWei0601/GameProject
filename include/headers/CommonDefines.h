#pragma once
#include <iostream>

#ifdef _DEBUG
#define DEBUG_LOG(x) std::cout << "[DEBUG] " << x << std::endl
#define DEBUG_LOG_WARN(x) std::cout << "[WARNING] " << x << std::endl
#define DEBUG_LOG_ERROR(x) std::cout << "[ERROR] " << x << std::endl
#else
#define DEBUG_LOG(x)
#define DEBUG_LOG_WARN(x)
#define DEBUG_LOG_ERROR(x)
#endif