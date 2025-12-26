#pragma once

#include "Renderer/Render.h"

namespace Ilargi
{
	class VulkanRender : public Render
	{
	public:
		VulkanRender();
		virtual ~VulkanRender();

		void SubmitGeometry(std::shared_ptr<CommandBuffer> aCommandBuffer, std::shared_ptr<VertexBuffer> aVertexBuffer, std::shared_ptr<IndexBuffer> aIndexBuffer) const;
		void DrawDefault(std::shared_ptr<CommandBuffer> aCommandBuffer) const;
	
	};
}