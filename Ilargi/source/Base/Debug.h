#pragma once

#include "Timer.h"

#if ILG_DEBUG
#define ILG_ASSERT(x, ...)		{ if (!(x)) { ILG_CORE_ERROR(__VA_ARGS__); __debugbreak(); }}
#else
#define ILG_ASSERT(x, ...)
#endif

#define ILG_BIND_FN(x) std::bind(&x, this, std::placeholders::_1)

#define ILG_PROFILE_FUNC Timer timer;