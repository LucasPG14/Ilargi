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
	private:
		static GraphicsAPI graphicsAPI;

		static int maxFrames;
	};
}