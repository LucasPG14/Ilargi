#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

namespace Ilargi
{
	class Input
	{
	public:
		static void SetWindow(GLFWwindow* aWindow) { sWindow = aWindow; }

		static bool IsKeyPressed(KeyCode::KeyCode aKey);
		static bool IsMouseButtonPressed(MouseCode::MouseCode aKey);

		static const glm::vec2 GetMousePos();

	private:
		static GLFWwindow* sWindow;
	};
}