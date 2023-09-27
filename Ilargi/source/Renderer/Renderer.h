#pragma once

namespace Ilargi
{
	enum class GraphicsAPI
	{
		VULKAN = 0,
	};

	class Renderer
	{
	public:

		static int GetMaxFrames() { return maxFrames; }

		static void SetMaxFrames(int frms) { maxFrames = frms; }
		static GraphicsAPI GetGraphicsAPI() { return graphicsAPI; }

		static void Submit(std::function<void()> func) { queue.push_back(func); }

		static void Render();
	private:
		static GraphicsAPI graphicsAPI;

		static int maxFrames;

		static std::vector<std::function<void()>> queue;
	};
}