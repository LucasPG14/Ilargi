#pragma once

#include "Renderer/IRender.h"

namespace Ilargi
{
	class VulkanRender : public IRender
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
		void SubmitGeometry(const std::shared_ptr<ICommandBuffer>& aCommandBuffer, const std::shared_ptr<IVertexBuffer>& aVertexBuffer, const std::shared_ptr<IIndexBuffer>& aIndexBuffer) const override;
		
		/*
		* @copydoc VulkanRender::DrawDefault()
		*/
		void DrawDefault(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const override;
	};
}