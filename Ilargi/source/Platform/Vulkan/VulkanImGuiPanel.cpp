#include "ilargipch.h"

#include "VulkanImGuiPanel.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"

#include "Utils/UI/IlargiUI.h"

// 3rd Party headers
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <ImGuizmo.h>

namespace Ilargi
{
	static void CheckResult(VkResult error)
	{
		VK_CHECK_RESULT(error);
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
		
		UI::IlargiStyle2();

		ImGui_ImplGlfw_InitForVulkan(win, true);
		
		auto device = VulkanContext::GetLogicalDevice();
		uint32_t maxFrames = Renderer::GetConfig().maxFrames;

		ImGui_ImplVulkan_InitInfo imguiInfo = {};
		imguiInfo.Instance = VulkanContext::GetInstance();
		imguiInfo.PhysicalDevice = VulkanContext::GetPhysicalDevice();
		imguiInfo.Device = device;
		imguiInfo.QueueFamily = VulkanContext::GetQueueIndices().graphicsFamily;
		imguiInfo.Queue = VulkanContext::GetGraphicsQueue();
		imguiInfo.DescriptorPool = VulkanContext::GetDescriptorPool();
		imguiInfo.Subpass = 0;
		imguiInfo.MinImageCount = maxFrames;
		imguiInfo.ImageCount = maxFrames;
		imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		imguiInfo.Allocator = nullptr;
		imguiInfo.CheckVkResultFn = CheckResult;
		ImGui_ImplVulkan_Init(&imguiInfo, vkSwapchain->GetRenderPass());

		io.Fonts->AddFontFromFileTTF("fonts/arial.ttf", 16.0f);
		{
			VkCommandBuffer commandBuffer = VulkanContext::BeginSingleCommandBuffer();
			
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			
			VulkanContext::EndSingleCommandBuffer(commandBuffer);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	
	VulkanImGuiPanel::~VulkanImGuiPanel()
	{
	}

	void VulkanImGuiPanel::Destroy() const
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDeviceWaitIdle(device);

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void VulkanImGuiPanel::Begin() const
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void VulkanImGuiPanel::End() const
	{
		ImGui::EndFrame();
		
		// TODO: This must be done in another way
		VkCommandBuffer cmdBuffer = vkSwapchain->GetCurrentCommand();
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			vkBeginCommandBuffer(cmdBuffer, &beginInfo);

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vkSwapchain->GetRenderPass();
			renderPassInfo.framebuffer = vkSwapchain->GetFramebuffer();

			uint32_t width = vkSwapchain->GetWidth();
			uint32_t height = vkSwapchain->GetHeight();

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = { width, height};

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

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
		vkEndCommandBuffer(cmdBuffer);
	}
}