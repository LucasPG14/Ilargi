#include "ilargipch.h"

#include "Vec4.h"

namespace Ilargi
{
	constexpr vec4::vec4(float value) : x(value), y(value), z(value), w(value)
	{
	}
	
	constexpr vec4::vec4(float vX, float vY, float vZ, float vW) : x(vX), y(vY), z(vZ), w(vW)
	{
	}

	constexpr vec4& vec4::operator*=(const float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

		return *this;
	}
	
	constexpr vec4& vec4::operator*=(const vec4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;
	}
	
	constexpr vec4& vec4::operator/=(const float scale)
	{
		float s = 1.0f / scale;
		x *= s;
		y *= s;
		z *= s;
		w *= s;

		return *this;
	}
	
	constexpr vec4& vec4::operator/=(const vec4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;

		return *this;
	}
	
	constexpr vec4& vec4::operator+=(const float scale)
	{
		x += scale;
		y += scale;
		z += scale;
		w += scale;

		return *this;
	}
	
	constexpr vec4& vec4::operator+=(const vec4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}
	
	constexpr vec4& vec4::operator-=(const float scale)
	{
		x -= scale;
		y -= scale;
		z -= scale;
		w -= scale;

		return *this;
	}
	
	constexpr vec4& vec4::operator-=(const vec4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}
}