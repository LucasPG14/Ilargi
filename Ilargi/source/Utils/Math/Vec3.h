#pragma once

namespace Ilargi
{
	struct vec3
	{
		float x, y, z;

		constexpr vec3() = default;
		constexpr vec3(float value);
		constexpr vec3(float valueX, float valueY, float valueZ);

		constexpr float& operator[](int i) { return (&x)[i]; }

		constexpr const float& operator[](int i) const { return (&x)[i]; }

		constexpr vec3& operator*=(const float scale);
		constexpr vec3& operator*=(const vec3& v);

		constexpr vec3& operator/=(const float scale);
		constexpr vec3& operator/=(const vec3& v);

		constexpr vec3& operator+=(const float scale);
		constexpr vec3& operator+=(const vec3& v);

		constexpr vec3& operator-=(const float scale);
		constexpr vec3& operator-=(const vec3& v);
	};

	constexpr vec3 operator*(const vec3& v, const float scale)
	{
		return vec3(v.x * scale, v.y * scale, v.z * scale);
	}

	constexpr vec3 operator*(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	constexpr vec3 operator/(const vec3& v, const float scale)
	{
		float s = 1.0f / scale;
		return vec3(v.x * s, v.y * s, v.z * s);
	}

	constexpr vec3 operator/(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	}

	constexpr vec3 operator+(const vec3& v, const float scale)
	{
		return vec3(v.x + scale, v.y + scale, v.z + scale);
	}

	constexpr vec3 operator+(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	constexpr vec3 operator-(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	constexpr vec3 normalize(const vec3& v);
	constexpr vec3 cross(const vec3& v1, const vec3& v2);
	constexpr float dot(const vec3& v1, const vec3& v2);
}