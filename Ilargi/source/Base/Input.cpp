#include "ilargipch.h"

// Main headers
#include "Input.h"

// 3rd Party headers
#include <GLFW/glfw3.h>

namespace Ilargi
{
	GLFWwindow* Input::sWindow = nullptr;

	bool Input::IsKeyPressed(KeyCode::KeyCode aKey)
	{
		int k = glfwGetKey(sWindow, aKey);

		return k == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonPressed(MouseCode::MouseCode aKey)
	{
		int k = glfwGetMouseButton(sWindow, aKey);

		return k == GLFW_PRESS;
	}
	
	const glm::vec2 Input::GetMousePos()
	{
		double x, y;
		glfwGetCursorPos(sWindow, &x, &y);
		return { (float)x, (float)y };
	}
}