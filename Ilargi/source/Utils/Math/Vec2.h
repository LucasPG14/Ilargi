#pragma once

namespace Ilargi
{
	struct vec2
	{
		float x, y;

		constexpr vec2() = default;
		constexpr vec2(float value);
		constexpr vec2(float valueX, float valueY);

		constexpr float& operator[](int i) { return (&x)[i]; }

		constexpr const float& operator[](int i) const { return (&x)[i]; }

		constexpr vec2& operator*=(const float scale);
		constexpr vec2& operator*=(const vec2& v);

		constexpr vec2& operator/=(const float scale);
		constexpr vec2& operator/=(const vec2& v);

		constexpr vec2& operator+=(const float scale);
		constexpr vec2& operator+=(const vec2& v);

		constexpr vec2& operator-=(const float scale);
		constexpr vec2& operator-=(const vec2& v);
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

	constexpr vec2 normalize(const vec2& v);
	constexpr vec2 cross(const vec2& v1, const vec2& v2);
	constexpr float dot(const vec2& v1, const vec2& v2);
}