#include "ilargipch.h"

#include "Renderer.h"

namespace Ilargi
{
	GraphicsAPI Renderer::graphicsAPI = GraphicsAPI::VULKAN;
	int Renderer::maxFrames = 0;
	std::vector<std::function<void()>> Renderer::queue = {};
	
	void Renderer::Render()
	{
		for (int i = 0; i < queue.size(); ++i)
			queue[i]();
		
		queue.clear();
	}
}