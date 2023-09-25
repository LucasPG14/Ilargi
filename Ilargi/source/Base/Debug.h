#pragma once

#include "Log.h"

#if ILG_DEBUG
#define ILG_ASSERT(x, ...)		{ if (!(x)) { ILG_CORE_ERROR(__VA_ARGS__); __debugbreak(); }}
#else
#define ILG_ASSERT(x, ...)
#endif

#define VK_CHECK_RESULT(x, ...)		{ ILG_ASSERT(x, __VA_ARGS__); }

#define ILG_BIND_FN(x) std::bind(&x, this, std::placeholders::_1)