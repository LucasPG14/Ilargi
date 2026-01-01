#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class Swapchain;

	class ImGuiPanel
	{
	public:
		/*
		* @brief Starts a new frame.
		*/
		virtual void Begin() const = 0;

		/*
		* @brief Ends the current frame.
		*/
		virtual void End() const = 0;

		/*
		* @brief Destroys the ImGui panel data.
		*/
		virtual void Destroy() const = 0;

		/*
		* @brief Creates an ImGuiPanel depending on the GraphicsAPI.
		* @param aWindow A pointer to the actual GLFW window.
		* @param aSwapchain A pointer to the swapchain of the window.
		* @return The ImGui panel created.
		*/
		static std::unique_ptr<ImGuiPanel> Create(GLFWwindow* aWindow, const std::shared_ptr<Swapchain>& aSwapchain);
	};
}