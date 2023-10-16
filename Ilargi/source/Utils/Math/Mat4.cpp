#include "ilargipch.h"

#include "Mat4.h"
#include "Vec3.h"

namespace Ilargi
{
	constexpr mat4::mat4(float value)
	{
		matrix[0][0] = value;	matrix[0][1] = 0.0f;	matrix[0][2] = 0.0f;	matrix[0][3] = 0.0f;
		matrix[1][0] = 0.0f;	matrix[1][1] = value;	matrix[1][2] = 0.0f;	matrix[1][3] = 0.0f;
		matrix[2][0] = 0.0f;	matrix[2][1] = 0.0f;	matrix[2][2] = value;	matrix[2][3] = 0.0f;
		matrix[3][0] = 0.0f;	matrix[3][1] = 0.0f;	matrix[3][2] = 0.0f;	matrix[3][3] = value;
	}

	constexpr mat4::mat4(const vec4& x, const vec4& y, const vec4& z, const vec4& w)
	{
		matrix[0] = x;
		matrix[1] = y;
		matrix[2] = z;
		matrix[3] = w;
	}
	
	constexpr mat4 operator*(const mat4& m1, const mat4& m2)
	{
		const vec4 a0 = m1[0];
		const vec4 a1 = m1[1];
		const vec4 a2 = m1[2];
		const vec4 a3 = m1[3];
		 
		const vec4 b0 = m1[0];
		const vec4 b1 = m1[1];
		const vec4 b2 = m1[2];
		const vec4 b3 = m1[3];

		return mat4(a0 * b0 + a1 * b0 + a2 * b0 + a3 * b0, 
				a0 * b1 + a1 * b1 + a2 * b1 + a3 * b1,
				a0 * b2 + a1 * b2 + a2 * b2 + a3 * b2,
				a0 * b3 + a1 * b3 + a2 * b3 + a3 * b3);
	}
	
	constexpr mat4 operator*(const mat4& m1, const vec4& v)
	{
		return mat4(m1[0] * v, m1[1] * v, m1[2] * v, m1[3] * v);
	}
	
	// Right handed perspective
	mat4 perspective(const float fovY, const float aspectRatio, const float nPlane, const float fPlane)
	{
		mat4 matrix = mat4(0.0f);

		const float tanHalfFovy = tan(fovY * 0.5f);

		matrix[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
		matrix[1][1] = 1.0f / (tanHalfFovy);
		matrix[2][2] = -(fPlane + nPlane) / (fPlane - nPlane);
		matrix[2][3] = -1.0f;
		matrix[3][2] = -(2.0f * fPlane * nPlane) / (fPlane - nPlane);

		return matrix;
	}

	constexpr mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up)
	{
		const vec3 f(normalize(target - eye));
		const vec3 s(normalize(cross(f, up)));
		const vec3 u(cross(s, f));

		mat4 matrix(1.0f);

		matrix[0][0] = s.x;
		matrix[1][0] = s.y;
		matrix[2][0] = s.z;
		matrix[0][1] = u.x;
		matrix[1][1] = u.y;
		matrix[2][1] = u.z;
		matrix[0][2] = -f.x;
		matrix[1][2] = -f.y;
		matrix[2][2] = -f.z;
		matrix[3][0] = -dot(s, eye);
		matrix[3][1] = -dot(u, eye);
		matrix[3][2] = dot(f, eye);

		return matrix;
	}
}