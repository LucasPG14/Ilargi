#pragma once

struct GLFWwindow;

#include "Utils/InputCodes.h"

#include "Utils/Math/Vec2.h"

namespace Ilargi
{
	class Input
	{
	public:
		static void SetWindow(GLFWwindow* aWindow) { sWindow = aWindow; }

		static bool IsKeyPressed(KeyCode::KeyCode aKey);
		static bool IsMouseButtonPressed(MouseCode::MouseCode aKey);

		static const vec2 GetMousePos();

	private:
		static GLFWwindow* sWindow;
	};
}