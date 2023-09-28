#pragma once

#include "Renderer/Render.h"

namespace Ilargi
{
	class VulkanRender : public Render
	{
	public:
		VulkanRender();
		virtual ~VulkanRender();

		void SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);
	};
}