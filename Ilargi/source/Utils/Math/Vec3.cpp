#include "ilargipch.h"

#include "Vec3.h"

namespace Ilargi
{
	constexpr vec3::vec3(float value) : x(value), y(value), z(value)
	{
	}

	constexpr vec3::vec3(float valueX, float valueY, float valueZ) : x(valueX), y(valueY), z(valueZ)
	{
	}

	constexpr vec3& vec3::operator*=(const float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	constexpr vec3& vec3::operator*=(const vec3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	constexpr vec3& vec3::operator/=(const float scale)
	{
		float s = 1.0f / scale;
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}
	
	constexpr vec3& vec3::operator/=(const vec3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}
	
	constexpr vec3& vec3::operator+=(const float scale)
	{
		x += scale;
		y += scale;
		z += scale;

		return *this;
	}
	
	constexpr vec3& vec3::operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}
	
	constexpr vec3& vec3::operator-=(const float scale)
	{
		x -= scale;
		y -= scale;
		z -= scale;

		return *this;
	}
	
	constexpr vec3& vec3::operator-=(const vec3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}
	
	constexpr vec3 normalize(const vec3& v)
	{
		return v / std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	
	constexpr vec3 cross(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.y * v2.z - v1.z * v2.y,
					v1.z * v2.x - v1.x * v2.z,
					v1.x * v2.y - v1.y * v2.x);
	}
	
	constexpr float dot(const vec3& v1, const vec3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
}