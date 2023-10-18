#pragma once

#include "Utils/Math/Vec3.h"
#include "Utils/Math/Vec4.h"
#include "Utils/Math/Quat.h"

namespace Ilargi
{
	struct mat4
	{
		mat4() = default;
		constexpr mat4(float value)
		{
			matrix[0][0] = value;	matrix[0][1] = 0.0f;	matrix[0][2] = 0.0f;	matrix[0][3] = 0.0f;
			matrix[1][0] = 0.0f;	matrix[1][1] = value;	matrix[1][2] = 0.0f;	matrix[1][3] = 0.0f;
			matrix[2][0] = 0.0f;	matrix[2][1] = 0.0f;	matrix[2][2] = value;	matrix[2][3] = 0.0f;
			matrix[3][0] = 0.0f;	matrix[3][1] = 0.0f;	matrix[3][2] = 0.0f;	matrix[3][3] = value;
		}

		constexpr mat4(const vec4& x, const vec4& y, const vec4& z, const vec4& w)
		{
			matrix[0] = x;
			matrix[1] = y;
			matrix[2] = z;
			matrix[3] = w;
		}

		constexpr mat4(const quat& q)
		{
			float qxqx = q.x * q.x;
			float qyqy = q.y * q.y;
			float qzqz = q.z * q.z;
			float qxqz = q.x * q.z;
			float qxqy = q.x * q.y;
			float qyqz = q.y * q.z;
			float qwqx = q.w * q.x;
			float qwqy = q.w * q.y;
			float qwqz = q.w * q.z;

			matrix[0][0] = 1.0f - 2.0f * (qyqy + qzqz);
			matrix[0][1] = 2.0f * (qxqy + qwqz);
			matrix[0][2] = 2.0f * (qxqz - qwqy);
			matrix[0][3] = 0.0f;

			matrix[1][0] = 2.0f * (qxqy - qwqz);
			matrix[1][1] = 1.0f - 2.0f * (qxqx + qzqz);
			matrix[1][2] = 2.0f * (qyqz + qwqx);
			matrix[1][3] = 0.0f;
							
			matrix[2][0] = 2.0f * (qxqz + qwqy);
			matrix[2][1] = 2.0f * (qyqz - qwqx);
			matrix[2][2] = 1.0f - 2.0f * (qxqx + qyqy);
			matrix[2][3] = 0.0f;

			matrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		mat4& operator=(const mat4& m)
		{
			matrix[0] = m[0];
			matrix[1] = m[1];
			matrix[2] = m[2];
			matrix[3] = m[3];

			return *this;
		}

		constexpr operator float*() { return &matrix[0].x; }
		constexpr operator const float*() const { return &matrix[0].x; }

		constexpr vec4& operator[](int i) { return matrix[i]; }
		constexpr const vec4& operator[](int i) const { return matrix[i]; }

	private:
		vec4 matrix[4];
	};

	constexpr mat4 operator*(const mat4& m1, const mat4& m2)
	{
		const vec4 a0 = m1[0];
		const vec4 a1 = m1[1];
		const vec4 a2 = m1[2];
		const vec4 a3 = m1[3];

		const vec4 b0 = m2[0];
		const vec4 b1 = m2[1];
		const vec4 b2 = m2[2];
		const vec4 b3 = m2[3];

		return mat4(a0 * b0[0] + a1 * b0[1] + a2 * b0[2] + a3 * b0[3],
					a0 * b1[0] + a1 * b1[1] + a2 * b1[2] + a3 * b1[3],
					a0 * b2[0] + a1 * b2[1] + a2 * b2[2] + a3 * b2[3],
					a0 * b3[0] + a1 * b3[1] + a2 * b3[2] + a3 * b3[3]);
	}

	constexpr mat4 operator*(const mat4& m1, const vec4& v)
	{
		return mat4(m1[0] * v, m1[1] * v, m1[2] * v, m1[3] * v);
	}

	inline mat4 perspective(float fovY, float aspectRatio, float nPlane, float fPlane)
	{
		mat4 matrix = mat4(0.0f);

		const float tanHalfFovy = tan(fovY * 0.5f);
		
		// Right handed
		matrix[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
		matrix[1][1] = 1.0f / (tanHalfFovy);
		matrix[2][2] = -(fPlane + nPlane) / (fPlane - nPlane);
		matrix[2][3] = -1.0f;
		matrix[3][2] = -(2.0f * fPlane * nPlane) / (fPlane - nPlane);

		// Left handed
		//matrix[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
		//matrix[1][1] = 1.0f / (tanHalfFovy);
		//matrix[2][2] = (fPlane + nPlane) / (fPlane - nPlane);
		//matrix[2][3] = 1.0f;
		//matrix[3][2] = -(2.0f * fPlane * nPlane) / (fPlane - nPlane);

		return matrix;
	}
	
	inline constexpr mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up)
	{
		const vec3 f(normalize(target - eye));
		const vec3 s(normalize(cross(f, up)));
		const vec3 u(cross(s, f));

		//const vec3 f(normalize(target - eye));
		//const vec3 s(normalize(cross(up, f)));
		//const vec3 u(cross(f, s));

		mat4 matrix(1.0f);

		// Right handed
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

		// Left handed
		//matrix[0][0] = s.x;
		//matrix[1][0] = s.y;
		//matrix[2][0] = s.z;
		//matrix[0][1] = u.x;
		//matrix[1][1] = u.y;
		//matrix[2][1] = u.z;
		//matrix[0][2] = f.x;
		//matrix[1][2] = f.y;
		//matrix[2][2] = f.z;
		//matrix[3][0] = -dot(s, eye);
		//matrix[3][1] = -dot(u, eye);
		//matrix[3][2] = -dot(f, eye);

		return matrix;
	}

	namespace math
	{
		inline mat4 translate(const vec3& v)
		{
			mat4 matrix(1.0f);

			matrix[3] = matrix[0] * v[0] + matrix[1] * v[1] + matrix[2] * v[2] + matrix[3];

			return matrix;
		}

		constexpr inline mat4 scale(const vec3& v)
		{
			mat4 matrix(1.0f);

			matrix[0] = matrix[0] * v[0];
			matrix[1] = matrix[1] * v[1];
			matrix[2] = matrix[2] * v[2];

			return matrix;
		}

		constexpr inline mat4 transpose(const mat4& m)
		{
			mat4 matrix;
			matrix[0][0] = m[0][0];
			matrix[0][1] = m[1][0];
			matrix[0][2] = m[2][0];
			matrix[0][3] = m[3][0];

			matrix[1][0] = m[0][1];
			matrix[1][1] = m[1][1];
			matrix[1][2] = m[2][1];
			matrix[1][3] = m[3][1];

			matrix[2][0] = m[0][2];
			matrix[2][1] = m[1][2];
			matrix[2][2] = m[2][2];
			matrix[2][3] = m[3][2];

			matrix[3][0] = m[0][3];
			matrix[3][1] = m[1][3];
			matrix[3][2] = m[2][3];
			matrix[3][3] = m[3][3];

			return matrix;
		}
	}
}