#pragma once

namespace Ilargi
{
	class IFramebuffer;
	class Material;
	class IGraphicsPipeline;
	class IRenderPass;
	class IUniformBuffer;

	class ICommandBuffer : public std::enable_shared_from_this<ICommandBuffer>
	{
	public:
		/*
		* @brief Destroys the command buffer data.
		*/
		virtual void Destroy() const = 0;

		/*
		* @brief Starts a new command.
		*/
		virtual void BeginCommand() const = 0;

		/*
		* @brief Ends the current command.
		*/
		virtual void EndCommand() const = 0;

		/*
		* @brief Begins the specified render pass.
		* @param aRenderPass The render pass.
		* @param aFramebuffer The framebuffer to draw.
		*/
		virtual void BeginRenderPass(const IRenderPass& aRenderPass, const std::shared_ptr<IFramebuffer>& aFramebuffer) const = 0;
		
		/*
		* @brief Ends the current render pass.
		*/
		virtual void EndRenderPass() const = 0;

		/*
		* @brief Binds the specified pipeline.
		* @param aPipeline The pipeline.
		*/
		virtual void BindPipeline(const std::shared_ptr<IGraphicsPipeline>& aPipeline) const = 0;

		/*
		* @brief Push a constant value to the shader.
		* @param aShaderName The name of the shader.
		* @param aOffset The offset of the data.
		* @param aSize The size of the data.
		* @param aData A pointer to the data pushed to the shader.
		*/
		virtual void PushConstants(std::string_view aShaderName, uint32_t aOffset, uint32_t aSize, const void* aData) const = 0;

		/*
		* @brief Binds the material.
		* @param aShaderName The name of the shader.
		* @param aMaterial The material to bind.
		* @param aSetIndex The index set to bind the material.
		*/
		virtual void BindMaterial(std::string_view aShaderName, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const = 0;

		/*
		* @brief Binds the uniform buffer.
		* @param aShaderName The name of the shader.
		* @param aUniformBuffer The uniform buffer to bind.
		* @param aSetIndex The index set to bind the uniform buffer.
		*/
		virtual void BindUniformBuffer(std::string_view aShaderName, const std::shared_ptr<IUniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const = 0;

		/*
		* @brief Submits the actual command to the graphics queue.
		*/
		virtual void Submit() const = 0;

		/*
		* @brief Casts the command buffer to the specified template class.
		* @tparam The destination type to which the command buffer will be cast.
		* @return An instance of type 'T' created from the command buffer.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<ICommandBuffer, T>::value, "T must be a derived class of CommandBuffer");

			return std::static_pointer_cast<T>(shared_from_this());
		}
		
		/*
		* @brief Creates the command buffer.
		* @param aFramesInFlight The number of command buffers that will be created.
		* @return An instance of the command buffer created.
		*/
		static std::shared_ptr<ICommandBuffer> Create(uint32_t aFramesInFlight);
	};
}