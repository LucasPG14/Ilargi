#include "ilargipch.h"

#include "EditorCamera.h"
#include "Base/Input.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/orthonormalize.hpp>
#include <gtc/matrix_transform.hpp>

namespace Ilargi
{
	EditorCamera::EditorCamera() : mViewMatrix(), mProjectionMatrix(), mPosition(0.0f, 1.0f, 8.0f), mUp(0.0f, 1.0f, 0.0f),
		mFront(0.0f, 0.0f, -1.0f), mHorizontalFov(glm::radians(60.0f)), mNearPlane(0.1f), mFarPlane(1000.0f), mMousePosition(0.0f)
	{
		float aspectRatio{ 1080.0f / 720.0f };
		mProjectionMatrix = glm::perspective(mHorizontalFov, aspectRatio, mNearPlane, mFarPlane);
		//mProjectionMatrix[1][1] *= -1;

		ComputeViewMatrix();
	}
	
	EditorCamera::~EditorCamera()
	{
	}
	
	void EditorCamera::Update(float aDeltaTime)
	{
		constexpr float speed{ 5.0f };

		glm::vec3 newPos{ mPosition };
		glm::vec3 newFront{ mFront };
		glm::vec3 newUp{ mUp };

		const glm::vec2& mouse{ Input::GetMousePos() };
		glm::vec2 delta{ (mouse - mMousePosition) * 0.0001f };
		mMousePosition = mouse;

		delta.x = -delta.x * (0.016f * 1000.0f);
		delta.y = -delta.y * (0.016f * 1000.0f);
		if (Input::IsMouseButtonPressed(MouseCode::RIGHT))
		{
			if (Input::IsKeyPressed(KeyCode::W))
			{
				newPos += mFront * speed * aDeltaTime;
			}
			if (Input::IsKeyPressed(KeyCode::S))
			{
				newPos -= mFront * speed * aDeltaTime;
			}
			if (Input::IsKeyPressed(KeyCode::A))
			{
				newPos -= glm::normalize(glm::cross(mFront, mUp)) * speed * aDeltaTime;
			}
			if (Input::IsKeyPressed(KeyCode::D))
			{
				newPos += glm::normalize(glm::cross(mFront, mUp)) * speed * aDeltaTime;
			}
			if (Input::IsKeyPressed(KeyCode::Q))
			{
				newPos += mUp * speed * aDeltaTime;
			}
			if (Input::IsKeyPressed(KeyCode::E))
			{
				newPos -= mUp * speed * aDeltaTime;
			}

			if (Input::IsKeyPressed(KeyCode::LEFT_ALT))
			{
				glm::quat orbit{ glm::quat(glm::vec3(mPosition.z >= 0.0f ? delta.y * 2.0f : -delta.y * 2.0f, delta.x * 2.0f, 0.0)) };
				
				newPos = glm::normalize(orbit) * newPos;
				newFront = glm::normalize(glm::vec3(0.0) - newPos);
			}
			else
			{
				if (delta.y != 0)
				{
					const glm::quat& quaternion{ glm::quat(delta.y, glm::normalize(glm::cross(mFront, mUp))) };
					const glm::quat& conjQuat{ glm::conjugate(quaternion) };

					newFront = glm::normalize(quaternion * newFront * conjQuat);
					newUp = glm::normalize(quaternion * newUp * conjQuat);
					newFront = glm::orthonormalize(newFront, newUp);
				}
				if (delta.x != 0)
				{
					const glm::quat& quaternion{ glm::quat(delta.x, glm::vec3(0.0f, 1.0f, 0.0f)) };
					const glm::quat& conjQuat{ glm::conjugate(quaternion) };

					newFront = glm::normalize(quaternion * newFront * conjQuat);
					newUp = glm::normalize(quaternion * newUp * conjQuat);
					glm::orthonormalize(newFront, newUp);
				}
			}

			mPosition = newPos;
			mUp = newUp;
			mFront = newFront;
		}

		mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void EditorCamera::Resize(float aWidth, float aHeight)
	{
		float aspectRatio{ aWidth / aHeight };
		mProjectionMatrix = glm::perspective(mHorizontalFov, aspectRatio, mNearPlane, mFarPlane);
		//mProjectionMatrix[1][1] *= -1;
	}
	
	void EditorCamera::ComputeViewMatrix()
	{
		mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);
	}
}