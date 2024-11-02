#pragma once

#include <mat4x4.hpp>

namespace Ilargi
{
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }

		const glm::mat4& GetViewProjectionMatrix() const { return mProjectionMatrix * mViewMatrix; }

		glm::vec3 GetPosition() { return mPosition; }

		void Update();

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