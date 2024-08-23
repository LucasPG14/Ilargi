#include "ilargipch.h"

#include "EditorCamera.h"
#include "Base/Input.h"


namespace Ilargi
{
	EditorCamera::EditorCamera() : mViewMatrix(), mProjectionMatrix(), mPosition(0.0f, 1.5f, -5.0f), mUp(0.0f, 1.0f, 0.0f), 
		mFront(0.0f, 0.0f, -1.0f), mHorizontalFov(radians(70.0f)), mNearPlane(0.1f), mFarPlane(1000.0f), mYaw(90.0f), mPitch(0.0f),
		mMousePosition(0.0f)
	{
		float aspectRatio = 1080.0f / 720.0f;
		mProjectionMatrix = perspective(2.0f * atan(tan(mHorizontalFov * 0.5f) * aspectRatio), aspectRatio, mNearPlane, mFarPlane);

		vec3 direction;
		direction.x = cos(radians(mYaw)) * cos(radians(mPitch));
		direction.y = sin(radians(mPitch));
		direction.z = sin(radians(mYaw)) * cos(radians(mPitch));
		mFront = normalize(direction);

		ComputeViewMatrix();
	}
	
	EditorCamera::~EditorCamera()
	{
	}
	
	void EditorCamera::Update()
	{
		constexpr float sensitivity = 0.1f;
		float cameraSpeed = 0.05f;

		const vec2& mousePos = Input::GetMousePos();

		if (Input::IsMouseButtonPressed(MouseCode::RIGHT))
		{
			if (Input::IsKeyPressed(KeyCode::W))
				mPosition += mFront * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::S))
				mPosition -= mFront * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::A))
				mPosition -= normalize(cross(mFront, mUp)) * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::D))
				mPosition += normalize(cross(mFront, mUp)) * cameraSpeed;

			if (Input::IsKeyPressed(KeyCode::Q))
				mPosition += mUp * cameraSpeed;
			if (Input::IsKeyPressed(KeyCode::E))
				mPosition -= mUp * cameraSpeed;

			const vec2& delta = (mMousePosition - mousePos) * sensitivity;

			mYaw += delta.x;
			mPitch += delta.y;

			if (mPitch > 89.0f)
				mPitch = 89.0f;
			if (mPitch < -89.0f)
				mPitch = -89.0f;

			vec3 direction;
			direction.x = cos(radians(mYaw)) * cos(radians(mPitch));
			direction.y = sin(radians(mPitch));
			direction.z = sin(radians(mYaw)) * cos(radians(mPitch));
			mFront = normalize(direction);
			
			ComputeViewMatrix();
		}

		mMousePosition = mousePos;
	}

	void EditorCamera::Resize(float aWidth, float aHeight)
	{
		float aspectRatio = aWidth / aHeight;
		mProjectionMatrix = perspective(2.0f * atan(tan(mHorizontalFov * 0.5f) * aspectRatio), aspectRatio, mNearPlane, mFarPlane);
	}
	
	void EditorCamera::ComputeViewMatrix()
	{
		mViewMatrix = lookAt(mPosition, mPosition + mFront, mUp);
	}
}