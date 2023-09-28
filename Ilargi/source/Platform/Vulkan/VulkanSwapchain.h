#pragma once

#include "Renderer/Swapchain.h"

#include <vulkan/vulkan.h>
#include <glm.hpp>

namespace Ilargi
{
	struct SwapchainSupportDetails;

	// TODO: This needs to be removed
	class VertexBuffer;
	class IndexBuffer;

	class Framebuffer;
	class RenderPass;
	class Pipeline;
	class CommandBuffer;

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		virtual ~VulkanSwapchain();

		void Destroy() const;
		
		void StartFrame() override;
		void EndFrame() override;

		void Present() override;

		VkRenderPass GetRenderPass() { return renderPass; }

		VkCommandBuffer GetCurrentCommand() { return commandBuffers[currentFrame]; }

		VkFramebuffer GetFramebuffer(uint32_t index) const { return framebuffers[index]; }

		uint32_t GetWidth() const { return extent.width; }
		uint32_t GetHeight() const { return extent.height; }

	private:
		void RecreateSwapchain();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CleanUpSwapchain() const;

		void CreateRenderPass(VkDevice device);
		void CreatingPipeline(VkDevice device);

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& data);

		SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device) const;
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	private:
		VkSwapchainKHR swapchain;

		VkExtent2D extent;
		VkSurfaceFormatKHR surfaceFormat;

		VkRenderPass renderPass;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> renderFinished;
		std::vector<VkFence> fences;

		uint32_t currentFrame;
		uint32_t currentImageIndex;
		VkQueue presentQueue;

		// TODO: This must not be here
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;

		//std::shared_ptr<VertexBuffer> vertexBuffer;
		//std::shared_ptr<IndexBuffer> indexBuffer;
		//
		//std::shared_ptr<Framebuffer> fb;
		//std::shared_ptr<RenderPass> rp;
		//std::shared_ptr<Pipeline> pip;
		//
		//std::shared_ptr<CommandBuffer> commandBuffer;
	};
}