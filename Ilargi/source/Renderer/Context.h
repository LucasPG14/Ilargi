#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class GraphicsContext
	{
	public:
		virtual void Destroy() const = 0;

		static std::shared_ptr<GraphicsContext> Create(GLFWwindow* win, std::string_view appName);
	};
}