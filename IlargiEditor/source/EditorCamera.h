#pragma once

#include <mat4x4.hpp>

namespace Ilargi
{
	class EditorCamera
	{
	public:
		/*
		* @brief Constructor.
		*/
		EditorCamera();

		/*
		* @brief Destructor.
		*/
		~EditorCamera();

		/*
		* @brief Returns the view matrix of the camera.
		* @return The view matrix of the camera.
		*/
		[[nodiscard]] const glm::mat4& GetViewMatrix() const { return mViewMatrix; }

		/*
		* @brief Returns the projection matrix of the camera.
		* @return The projection matrix of the camera.
		*/
		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }

		/*
		* @brief Returns the view projection matrix of the camera.
		* @return The view projection matrix of the camera.
		*/
		[[nodiscard]] const glm::mat4 GetViewProjectionMatrix() const { return mProjectionMatrix * mViewMatrix; }

		/*
		* @brief Returns the camera position.
		* @return The camera position.
		*/
		[[nodiscard]] glm::vec3 GetPosition() { return mPosition; }

		/*
		* @brief Updates the camera by input.
		* @return The camera position.
		*/
		void Update(float aDeltaTime);

		/*
		* @brief Resizes the camera to the new width and height of the window and recreates the projection matrix.
		* @param aWidth The new width.
		* @param aHeight The new height.
		*/
		void Resize(float aWidth, float aHeight);

	private:
		/*
		* @brief Generates the view matrix.
		* @return The camera position.
		*/
		void ComputeViewMatrix();

	private:
		glm::mat4 mViewMatrix; // View matrix of the camera
		glm::mat4 mProjectionMatrix; // Projection matrix of the camera.

		glm::vec3 mPosition; // Position of the camera.
		glm::vec3 mUp; // Up vector of the camera.
		glm::vec3 mFront; // Front vector of the camera.

		float mHorizontalFov; // Horizontal pov of the camera.
		float mNearPlane; // The minimum distance of the camera.
		float mFarPlane; // The maximum distance of the camera.

		glm::vec2 mMousePosition; // The position of the mouse.
	};
}