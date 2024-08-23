#pragma once

#include "Utils/Math/Math.h"

namespace Ilargi
{
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		const mat4& GetViewMatrix() const { return mViewMatrix; }
		const mat4& GetProjectionMatrix() const { return mProjectionMatrix; }

		const mat4& GetViewProjectionMatrix() const { return mProjectionMatrix * mViewMatrix; }

		void Update();

		void Resize(float aWidth, float aHeight);

	private:
		void ComputeViewMatrix();

	private:
		mat4 mViewMatrix;
		mat4 mProjectionMatrix;

		vec3 mPosition;
		vec3 mUp;
		vec3 mFront;

		float mHorizontalFov;
		float mNearPlane;
		float mFarPlane;

		float mYaw;
		float mPitch;

		vec2 mMousePosition;
	};
}