#include "ilargipch.h"

// Main headers
#include "Input.h"

// 3rd Party headers
#include <GLFW/glfw3.h>

namespace Ilargi
{
	GLFWwindow* Input::window = nullptr;

	bool Input::IsKeyPressed(KeyCode::KeyCode key)
	{
		int k = glfwGetKey(window, key);

		return k == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonPressed(MouseCode::MouseCode key)
	{
		int k = glfwGetMouseButton(window, key);

		return k == GLFW_PRESS;
	}
	
	glm::vec2 Input::GetMousePos()
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return glm::vec2(x, y);
	}
}