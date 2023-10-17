#pragma once

#include "Utils/Math/Math.h"

namespace Ilargi
{
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		const mat4& GetViewMatrix() const { return viewMatrix; }
		const mat4& GetProjectionMatrix() const { return projectionMatrix; }

		const mat4& GetViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }

		void Update();

		void Resize(float width, float height);

	private:
		void ComputeViewMatrix();

	private:
		mat4 viewMatrix;
		mat4 projectionMatrix;

		vec3 position;
		vec3 up;
		vec3 front;

		float hFov;
		float nearPlane;
		float farPlane;

		float yaw;
		float pitch;

		vec2 mousePosition;
	};
}