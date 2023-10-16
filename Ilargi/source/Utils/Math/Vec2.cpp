#include "ilargipch.h"

#include "Vec2.h"

namespace Ilargi
{
	constexpr vec2::vec2(float value) : x(value), y(value)
	{
	}

	constexpr vec2::vec2(float vX, float vY) : x(vX), y(vY)
	{
	}

	constexpr vec2& vec2::operator*=(const float scale)
	{
		x *= scale;
		y *= scale;

		return *this;
	}

	constexpr vec2& vec2::operator*=(const vec2& v)
	{
		x *= v.x;
		y *= v.y;

		return *this;
	}

	constexpr vec2& vec2::operator/=(const float scale)
	{
		float s = 1.0f / scale;
		x *= s;
		y *= s;

		return *this;
	}

	constexpr vec2& vec2::operator/=(const vec2& v)
	{
		x /= v.x;
		y /= v.y;

		return *this;
	}

	constexpr vec2& vec2::operator+=(const float scale)
	{
		x += scale;
		y += scale;

		return *this;
	}

	constexpr vec2& vec2::operator+=(const vec2& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	constexpr vec2& vec2::operator-=(const float scale)
	{
		x -= scale;
		y -= scale;

		return *this;
	}

	constexpr vec2& vec2::operator-=(const vec2& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	constexpr vec2 normalize(const vec2& v)
	{
		return v / std::sqrt(v.x * v.x + v.y * v.y);
	}

	constexpr vec2 cross(const vec2& v1, const vec2& v2)
	{
		return vec2(v1.x * v2.y - v1.y * v2.x);
	}

	constexpr float dot(const vec2& v1, const vec2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
}