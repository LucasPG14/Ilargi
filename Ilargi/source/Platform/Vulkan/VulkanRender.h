#pragma once

#include "Renderer/Render.h"

namespace Ilargi
{
	class VulkanRender : public Render
	{
	public:
		/*
		* @brief Constructor.
		*/
		VulkanRender();

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanRender();

		/*
		* @copydoc VulkanRender::SubmitGeometry() 
		*/
		void SubmitGeometry(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<VertexBuffer>& aVertexBuffer, const std::shared_ptr<IndexBuffer>& aIndexBuffer) const override;
		
		/*
		* @copydoc VulkanRender::DrawDefault()
		*/
		void DrawDefault(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const override;
	};
}