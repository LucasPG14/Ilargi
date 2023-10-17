#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

#include "Utils/Math/Vec2.h"

namespace Ilargi
{
	class Input
	{
	public:
		static void SetWindow(GLFWwindow* win) { window = win; }

		static bool IsKeyPressed(KeyCode::KeyCode key);
		static bool IsMouseButtonPressed(MouseCode::MouseCode key);

		static const vec2 GetMousePos();

	private:
		static GLFWwindow* window;
	};
}