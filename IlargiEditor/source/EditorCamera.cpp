#include "ilargipch.h"

#include "EditorCamera.h"
#include "Base/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

namespace Ilargi
{
	EditorCamera::EditorCamera() : viewMatrix(), projectionMatrix(), position(0.0f, 0.5f, 3.0f), up(0.0f, 1.0f, 0.0f), 
		front(0.0f, 0.0f, -1.0f), hFov(glm::radians(45.0f)), nearPlane(0.1f), farPlane(1000.0f), yaw(-90.0f), pitch(0.0f), 
		mousePosition(0.0f)
	{
		float aspectRatio = 1080.0f / 720.0f;
		projectionMatrix = glm::perspective(2 * glm::atan(glm::tan(hFov * 0.5f) * aspectRatio), aspectRatio, nearPlane, farPlane);

		ComputeViewMatrix();
	}
	
	EditorCamera::~EditorCamera()
	{
	}
	
	void EditorCamera::Update()
	{
		constexpr float sensitivity = 0.1f;
		float cameraSpeed = 0.05f;

		const glm::vec2& mousePos = Input::GetMousePos();

		if (Input::IsMouseButtonPressed(MouseCode::RIGHT))
		{
			if (Input::IsKeyPressed(KeyCode::W))
				position += front * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::S))
				position -= front * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::A))
				position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::D))
				position += glm::normalize(glm::cross(front, up)) * cameraSpeed;

			if (Input::IsKeyPressed(KeyCode::Q))
				position += up * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::E))
				position -= up * cameraSpeed;

			const glm::vec2& delta = (mousePosition - mousePos) * sensitivity;

			yaw += delta.x;
			pitch += delta.y;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			front = glm::normalize(direction);
			ComputeViewMatrix();
		}

		mousePosition = mousePos;
	}

	void EditorCamera::Resize(float width, float height)
	{
		float aspectRatio = width / height;
		projectionMatrix = glm::perspective(2 * glm::atan(glm::tan(hFov * 0.5f) * aspectRatio), aspectRatio, nearPlane, farPlane);
	}
	
	void EditorCamera::ComputeViewMatrix()
	{
		viewMatrix = glm::lookAt(position, position + front, up);
	}
}