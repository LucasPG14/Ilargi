#pragma once

#include <mat4x4.hpp>

namespace Ilargi
{
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		[[nodiscard]] const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }

		[[nodiscard]] const glm::mat4& GetViewProjectionMatrix() const { return mProjectionMatrix * mViewMatrix; }

		[[nodiscard]] glm::vec3 GetPosition() { return mPosition; }

		void Update(float aDeltaTime);

		void Resize(float aWidth, float aHeight);

	private:
		void ComputeViewMatrix();

	private:
		glm::mat4 mViewMatrix;
		glm::mat4 mProjectionMatrix;

		glm::vec3 mPosition;
		glm::vec3 mUp;
		glm::vec3 mFront;

		float mHorizontalFov;
		float mNearPlane;
		float mFarPlane;

		glm::vec2 mMousePosition;
	};
}