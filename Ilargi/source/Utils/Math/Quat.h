#pragma once

#include "Vec3.h"

namespace Ilargi
{
	struct quat
	{
		float x, y, z, w;

		constexpr quat() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		constexpr quat(const vec3& v)
		{
			vec3 cs = cos(v * 0.5f);
			vec3 sn = sin(v * 0.5f);

			x = sn.x * cs.y * cs.z - cs.x * sn.y * sn.z;
			y = cs.x * sn.y * cs.z + sn.x * cs.y * sn.z;
			z = cs.x * cs.y * sn.z - sn.x * sn.y * cs.z;
			w = cs.x * cs.y * cs.z + sn.x * sn.y * sn.z;
		}

	private:
		constexpr vec3 cos(const vec3& v)
		{
			return vec3(std::cos(v.x), std::cos(v.y), std::cos(v.z));
		}

		constexpr vec3 sin(const vec3& v)
		{
			return vec3(std::sin(v.x), std::sin(v.y), std::sin(v.z));
		}
	};
}