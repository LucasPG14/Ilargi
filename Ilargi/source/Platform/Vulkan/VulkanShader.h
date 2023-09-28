#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::string_view vert, std::string_view frag);
		virtual ~VulkanShader();

		void Destroy();

		const VkShaderModule GetVertexShader() const { return vertexShader; }
		const VkShaderModule GetFragmentShader() const { return fragmentShader; }

	private:
		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

	private:
		VkShaderModule vertexShader;
		VkShaderModule fragmentShader;
	};
}