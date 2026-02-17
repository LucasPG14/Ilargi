#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class IGraphicsContext
	{
	public:
		/*
		* @brief Destroys the graphics context data.
		*/
		virtual void Destroy() const = 0;

		/*
		* @brief Creates the graphic context.
		* @param aWindow The current GLFW window pointer.
		* @param aAppName The application name.
		* @return An instance of the graphics context created.
		*/
		static std::unique_ptr<IGraphicsContext> Create(GLFWwindow* aWindow, std::string_view aAppName);
	};
}