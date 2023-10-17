#pragma once

namespace Ilargi
{
	struct vec4
	{
		float x, y, z, w;

		constexpr vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		constexpr vec4(float value) : x(value), y(value), z(value), w(value) {}
		constexpr vec4(float vX, float vY, float vZ, float vW) : x(vX), y(vY), z(vZ), w(vW) {}

		constexpr operator float*() { return &x; }

		constexpr float& operator[](int i) { return (&x)[i]; }
		constexpr const float& operator[](int i) const { return (&x)[i]; }

		constexpr vec4& operator*=(const float scale)
		{
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;

			return *this;
		}

		constexpr vec4& operator*=(const vec4& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			w *= v.w;

			return *this;
		}

		constexpr vec4& operator/=(const float scale)
		{
			float s = 1.0f / scale;
			x *= s;
			y *= s;
			z *= s;
			w *= s;

			return *this;
		}

		constexpr vec4& operator/=(const vec4& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			w /= v.w;

			return *this;
		}
								    
		constexpr vec4& operator+=(const float scale)
		{
			x += scale;
			y += scale;
			z += scale;
			w += scale;

			return *this;
		}

		constexpr vec4& operator+=(const vec4& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;

			return *this;
		}
								    
		constexpr vec4& operator-=(const float scale)
		{
			x -= scale;
			y -= scale;
			z -= scale;
			w -= scale;

			return *this;
		}

		constexpr vec4& operator-=(const vec4& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;

			return *this;
		}
	};

	constexpr vec4 operator*(const vec4& v, float scale)
	{
		return vec4(v.x * scale, v.y * scale, v.z * scale, v.w * scale);
	}

	constexpr vec4 operator*(const vec4& v1, vec4 v2)
	{
		return vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
	}

	constexpr vec4 operator/(const vec4& v, float scale)
	{
		float s = 1.0f / scale;
		return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
	}

	constexpr vec4 operator/(const vec4& v1, vec4 v2)
	{
		return vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
	}

	constexpr vec4 operator+(const vec4& v1, float value)
	{
		return vec4(v1.x + value, v1.y + value, v1.z + value, v1.w + value);
	}

	constexpr vec4 operator+(const vec4& v1, vec4 v2)
	{
		return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
	}
}