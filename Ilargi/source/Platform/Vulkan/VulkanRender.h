#pragma once

#include "Renderer/IRender.h"

struct GLFWwindow;

namespace Ilargi
{
	class VulkanGraphicsContext;

	class VulkanRender : public IRender
	{
	public:
		/*
		* @brief Constructor.
		*/
		VulkanRender(GLFWwindow* aWindow, std::string_view aAppName);

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

	private:
		std::unique_ptr<VulkanGraphicsContext> mGraphicsContext;
	};
}