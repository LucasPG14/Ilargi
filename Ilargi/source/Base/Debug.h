#pragma once

#if ILG_DEBUG
#define ILG_ASSERT(x, ...)		{ if (!x) { __debugbreak(); }}
#else
#define ILG_ASSERT(x, ...)
#endif

#define ILG_BIND_FN(x) std::bind(&x, this, std::placeholders::_1)