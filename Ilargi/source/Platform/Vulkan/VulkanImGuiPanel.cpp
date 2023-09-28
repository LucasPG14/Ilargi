#include "ilargipch.h"

#include "VulkanImGuiPanel.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandBuffer.h"

// 3rd Party headers
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Ilargi
{
	static void CheckResult(VkResult error)
	{
		VK_CHECK_RESULT(error == VK_SUCCESS, "Vulkan Error");
	}

	VulkanImGuiPanel::VulkanImGuiPanel(GLFWwindow* win, const std::shared_ptr<Swapchain> swapchain)
	{
		vkSwapchain = std::static_pointer_cast<VulkanSwapchain>(swapchain);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(win, true);

		//VkDescriptorPoolSize poolSizes[] =
		//{
		//	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		//	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		//};


		//VkDescriptorPoolCreateInfo poolInfo = {};
		//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		//poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		//poolInfo.maxSets = 1000;
		//poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
		//poolInfo.pPoolSizes = poolSizes;

		//VK_CHECK_RESULT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) == VK_SUCCESS, "Unable to create descriptor pool");
		
		auto device = VulkanContext::GetLogicalDevice();

		ImGui_ImplVulkan_InitInfo imguiInfo = {};
		imguiInfo.Instance = VulkanContext::GetInstance();
		imguiInfo.PhysicalDevice = VulkanContext::GetPhysicalDevice();
		imguiInfo.Device = device;
		imguiInfo.QueueFamily = VulkanContext::GetQueueIndices().graphicsFamily;
		imguiInfo.Queue = VulkanContext::GetGraphicsQueue();
		imguiInfo.DescriptorPool = VulkanContext::GetDescriptorPool();
		imguiInfo.Subpass = 0;
		imguiInfo.MinImageCount = Renderer::GetMaxFrames();
		imguiInfo.ImageCount = Renderer::GetMaxFrames();
		imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		imguiInfo.Allocator = nullptr;
		imguiInfo.CheckVkResultFn = CheckResult;
		ImGui_ImplVulkan_Init(&imguiInfo, vkSwapchain->GetRenderPass());

		//io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("fonts/arial.ttf", 16.0f);
		{
			VkCommandBuffer commandBuffer = VulkanContext::BeginSingleCommandBuffer();
			
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			
			VulkanContext::EndSingleCommandBuffer(commandBuffer);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}

		commandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(CommandBuffer::Create(Renderer::GetMaxFrames()));
	}
	
	VulkanImGuiPanel::~VulkanImGuiPanel()
	{
	}

	void VulkanImGuiPanel::Destroy() const
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDeviceWaitIdle(device);

		//vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void VulkanImGuiPanel::Begin() const
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiPanel::End() const
	{
		ImGui::EndFrame();
		
		// TODO: This must be done in another way
		commandBuffer->BeginCommand();
		uint32_t index = Renderer::GetCurrentFrame();
		VkCommandBuffer cmdBuffer = commandBuffer->GetCurrentCommand(index);
		{
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vkSwapchain->GetRenderPass();
			renderPassInfo.framebuffer = vkSwapchain->GetFramebuffer(index);

			uint32_t width = vkSwapchain->GetWidth();
			uint32_t height = vkSwapchain->GetHeight();

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = { width, height};

			VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)width;
			viewport.height = (float)height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = { width, height };
			vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

			vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		vkCmdEndRenderPass(cmdBuffer);
		commandBuffer->EndCommand();

		Renderer::AddCommand(commandBuffer);
	}
}