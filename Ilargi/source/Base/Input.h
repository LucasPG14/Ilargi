#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

namespace Ilargi
{
	class Input
	{
	public:
		/*
		* @brief Stores the window pointer to a local variable.
		* @param aWindow The window pointer.
		*/
		static void SetWindow(GLFWwindow* aWindow) { sWindow = aWindow; }

		/*
		* @brief Checks if the key is pressed.
		* @param aKey The key pressed.
		* @return True if the key is pressed, false otherwise.
		*/
		static bool IsKeyPressed(KeyCode::KeyCode aKey);

		/*
		* @brief Checks if the mouse button is pressed.
		* @param aButton The mouse button pressed.
		* @return True if the key is pressed, false otherwise.
		*/
		static bool IsMouseButtonPressed(MouseCode::MouseCode aButton);

		/*
		* @brief Gets the mouse position on the screen.
		* @return The position x and y of the mouse.
		*/
		static const glm::vec2 GetMousePos();

	private:
		static GLFWwindow* sWindow; // Pointer to the window context.
	};
}