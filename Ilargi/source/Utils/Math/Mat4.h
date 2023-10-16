#pragma once

#include "Vec4.h"

namespace Ilargi
{
	struct vec3;

	struct mat4
	{
		mat4() = default;
		constexpr mat4(float value);
		constexpr mat4(const vec4& x, const vec4& y, const vec4& z, const vec4& w);

		constexpr vec4& operator[](int i) { return matrix[i]; }

		constexpr const vec4& operator[](int i) const { return matrix[i]; }

	private:
		vec4 matrix[4];
	};

	constexpr mat4 operator*(const mat4& m1, const mat4& m2);
	constexpr mat4 operator*(const mat4& m1, const vec4& v);

	inline mat4 perspective(const float fovY, const float aspectRatio, const float nPlane, const float fPlane);
	inline constexpr mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up);
}