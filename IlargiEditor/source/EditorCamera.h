#pragma once

#include <glm.hpp>

namespace Ilargi
{
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		const glm::mat4& GetViewMatrix() const { return viewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }

		const glm::mat4 GetViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }

		void Update();

		void Resize(float width, float height);

	private:
		void ComputeViewMatrix();

	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;

		glm::vec3 position;
		glm::vec3 up;
		glm::vec3 front;

		float hFov;
		float nearPlane;
		float farPlane;

		float yaw;
		float pitch;

		glm::vec2 mousePosition;
	};
}