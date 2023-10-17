#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"

namespace Ilargi
{
	constexpr float radians(const float degrees)
	{
		return degrees * 0.01745329251994329576923690768489f;
	}

	constexpr vec3 radians(const vec3& degrees)
	{
		return degrees * 0.01745329251994329576923690768489f;
	}
}