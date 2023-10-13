#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

#include <vec2.hpp>

namespace Ilargi
{
	class Input
	{
	public:
		static void SetWindow(GLFWwindow* win) { window = win; }

		static bool IsKeyPressed(KeyCode::KeyCode key);
		static bool IsMouseButtonPressed(MouseCode::MouseCode key);

		static const glm::vec2 GetMousePos();

	private:
		static GLFWwindow* window;
	};
}