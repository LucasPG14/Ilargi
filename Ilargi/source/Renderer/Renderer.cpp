#include "ilargipch.h"

#include "Renderer.h"
#include "CommandBuffer.h"

namespace Ilargi
{
	GraphicsAPI Renderer::graphicsAPI = GraphicsAPI::VULKAN;
	int Renderer::maxFrames = 0;
	int Renderer::currentFrame = 0;
	std::vector<std::function<void()>> Renderer::queue = {};
	std::vector<std::shared_ptr<CommandBuffer>> Renderer::submittedCommands = {};
	
	void Renderer::Render()
	{
		for (int i = 0; i < queue.size(); ++i)
			queue[i]();
		
		queue.clear();
	}
}