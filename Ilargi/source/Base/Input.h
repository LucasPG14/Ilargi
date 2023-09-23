#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

namespace Ilargi
{
	class Input
	{
	public:
		static void SetWindow(GLFWwindow* win) { window = win; }

		static bool IsKeyPressed(KeyCode::KeyCode key);
		static bool IsMouseButtonPressed(MouseCode::MouseCode key);

		//static glm::vec2 GetMousePos();

	private:
		static GLFWwindow* window;
	};
}