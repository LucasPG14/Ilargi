#pragma once

#include "Renderer/IFence.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanFence : public IFence
	{
	public:
		VulkanFence();

		virtual ~VulkanFence();

	private:
		VkFence mFence;
	};
}