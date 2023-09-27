#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class Swapchain;

	class ImGuiPanel
	{
	public:
		virtual void Begin() const = 0;
		virtual void End() const = 0;

		virtual void Destroy() const = 0;

		static std::shared_ptr<ImGuiPanel> Create(GLFWwindow* win, const std::shared_ptr<Swapchain> swapchain);
	};
}