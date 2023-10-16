#pragma once

#include "ilargipch.h"

namespace Ilargi::imath
{
	struct vec2
	{
		float x, y;

		constexpr vec2() : x(1.0), y(1.0f) {}
		constexpr vec2(float value) : x(value), y(value) {}
		constexpr vec2(float vX, float vY) : x(vX), y(vY) {}

		constexpr float& operator[](int i) { return (&x)[i]; }

		constexpr const float& operator[](int i) const { return (&x)[i]; }

		constexpr vec2& operator*=(const float scale)
		{
			x *= scale;
			y *= scale;

			return *this;
		}

		constexpr vec2& operator*=(const vec2& v)
		{
			x *= v.x;
			y *= v.y;

			return *this;
		}

		constexpr vec2& operator/=(const float scale)
		{
			float s = 1.0f / scale;
			x *= s;
			y *= s;

			return *this;
		}

		constexpr vec2& operator/=(const vec2& v)
		{
			x /= v.x;
			y /= v.y;

			return *this;
		}

		constexpr vec2& operator+=(const float scale)
		{
			x += scale;
			y += scale;

			return *this;
		}

		constexpr vec2& operator+=(const vec2& v)
		{
			x += v.x;
			y += v.y;

			return *this;
		}

		constexpr vec2& operator-=(const float scale)
		{
			x -= scale;
			y -= scale;

			return *this;
		}

		constexpr vec2& operator-=(const vec2& v)
		{
			x -= v.x;
			y -= v.y;

			return *this;
		}
	};

	constexpr vec2 operator*(const vec2& v, const float scale)
	{
		return vec2(v.x * scale, v.y * scale);
	}

	constexpr vec2 operator*(const vec2& v1, const vec2& v2)
	{
		return vec2(v1.x * v2.x, v1.y * v2.y);
	}

	constexpr vec2 operator/(const vec2& v, const float scale)
	{
		float s = 1.0f / scale;
		return vec2(v.x * s, v.y * s);
	}

	constexpr vec2 operator/(const vec2& v1, const vec2& v2)
	{
		return vec2(v1.x / v2.x, v1.y / v2.y);
	}

	constexpr vec2 operator+(const vec2& v, const float scale)
	{
		return vec2(v.x + scale, v.y + scale);
	}

	constexpr vec2 operator+(const vec2& v1, const vec2& v2)
	{
		return vec2(v1.x + v2.x, v1.y + v2.y);
	}

	constexpr vec2 operator-(const vec2& v1, const vec2& v2)
	{
		return vec2(v1.x - v2.x, v1.y - v2.y);
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