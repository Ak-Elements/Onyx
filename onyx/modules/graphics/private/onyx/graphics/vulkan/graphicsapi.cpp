#include <onyx/graphics/vulkan/graphicsapi.h>

#include <onyx/graphics/vulkan/commandbuffermanager.h>
#include <onyx/graphics/vulkan/debugutilsmessenger.h>
#include <onyx/graphics/vulkan/descriptorpool.h>
#include <onyx/graphics/vulkan/descriptorset.h>
#include <onyx/graphics/vulkan/descriptorsetlayout.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/fence.h>
#include <onyx/graphics/vulkan/framebuffer.h>
#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/vulkan/memoryallocator.h>
#include <onyx/graphics/vulkan/physicaldevice.h>
#include <onyx/graphics/vulkan/pipeline.h>
#include <onyx/graphics/vulkan/renderpass.h>
#include <onyx/graphics/vulkan/swapchain.h>
#include <onyx/graphics/vulkan/surface.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/graphics/vulkan/semaphore.h>
#include <onyx/graphics/vulkan/sampler.h>
#include <onyx/graphics/vulkan/texture.h>
#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/graphics/window/windows/nativewindow.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics::Vulkan
{
    // needed for UniquePtr forward declarations
    VulkanGraphicsApi::VulkanGraphicsApi() = default;
    VulkanGraphicsApi::~VulkanGraphicsApi() = default;

    void VulkanGraphicsApi::Init(const Window& window)
    {
        // TODO: Add time query support for GPU profiling
        m_Window = &window;

        DynamicArray<const char*> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        m_Instance = MakeUnique<Instance>(window, validationLayers);
        m_DebugUtilsMessenger = MakeUnique<DebugUtilsMessenger>(*m_Instance, static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));
        m_Surface = MakeUnique<Surface>(*m_Instance, window);

        m_PhysicalDevice = MakeUnique<PhysicalDevice>(*m_Instance, *m_Surface);

        // Device Extensions
        // TODO: can this be moved somehow into Logiccal Device
        VkPhysicalDeviceFeatures2 physicalFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        DynamicArray<const char*> deviceExtensions;

        VkPhysicalDeviceVulkan11Features vulkan_11_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        void* current_pnext = &vulkan_11_features;

        VkPhysicalDeviceVulkan12Features vulkan_12_features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        vulkan_12_features.pNext = current_pnext;
        current_pnext = &vulkan_12_features;

        VkPhysicalDeviceVulkan13Features vulkan_13_features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        vulkan_13_features.maintenance4 = true;
        vulkan_13_features.pNext = current_pnext;
        current_pnext = &vulkan_13_features;

        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
        {
            m_IsTimelineSemaphoreEnabled = true;
        }

        if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME))
        {
            m_IsSynchronization2Enabled = true;
        }

        if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME))
        {
            m_IsDynamicRenderingEnabled = true;
        }

        if (m_IsDynamicRenderingEnabled == false)
        {
            if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
            {
                deviceExtensions.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
                m_IsRenderPass2ExtensionEnabled = true;
            }
        }

        VkPhysicalDeviceDescriptorIndexingFeatures bindlessExtenstion{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES, nullptr };
        if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
        {
            VkPhysicalDeviceFeatures2 deviceFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &bindlessExtenstion };
            vkGetPhysicalDeviceFeatures2(m_PhysicalDevice->GetHandle(), &deviceFeatures);

            m_IsBindlessEnabled = bindlessExtenstion.descriptorBindingPartiallyBound && bindlessExtenstion.runtimeDescriptorArray;
        }

        VkPhysicalDeviceMeshShaderFeaturesNV meshShaderExtension = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV };
        if (m_PhysicalDevice->IsExtensionSupported(VK_NV_MESH_SHADER_EXTENSION_NAME))
        {
            deviceExtensions.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
            meshShaderExtension.taskShader = true;
            meshShaderExtension.meshShader = true;

            meshShaderExtension.pNext = current_pnext;
            current_pnext = &meshShaderExtension;
        }

        physicalFeatures.pNext = current_pnext;
        // End device extensions

        m_Device = MakeUnique<Device>(*m_PhysicalDevice, deviceExtensions, physicalFeatures, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);

        vkCmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkCmdDrawMeshTasksNV");
        vkCmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkCmdDrawMeshTasksIndirectNV");
        vkCmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkCmdDrawMeshTasksIndirectCountNV");
        vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkSetDebugUtilsObjectNameEXT");
        vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkCmdBeginDebugUtilsLabelEXT");
        vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_Device->GetHandle(), "vkCmdEndDebugUtilsLabelEXT");

        m_Allocator = MakeUnique<MemoryAllocator>("Default Allocator", *this);

        constexpr onyxU32 maxPoolElements = 128;
        DynamicArray<VkDescriptorPoolSize> pools
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, maxPoolElements },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, maxPoolElements }
        };

        m_DescriptorPool = MakeUnique<DescriptorPool>(*m_Device, pools, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, maxPoolElements);
        if (IsBindless())
        {
            pools =
            {
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Graphics::MAX_BINDLESS_RESOURCES },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Graphics::MAX_BINDLESS_RESOURCES },
            };
            m_BindlessDescriptorPool = MakeUnique<DescriptorPool>(*m_Device, pools, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT, Graphics::MAX_BINDLESS_RESOURCES);

            const onyxU32 poolCount = static_cast<uint32_t>(pools.size());

            // change to static array?
            InplaceArray<VkDescriptorSetLayoutBinding, 4> descriptorSetBindings;
            VkDescriptorSetLayoutBinding& imageSamplerBinding = descriptorSetBindings[0];
            imageSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            imageSamplerBinding.descriptorCount = Graphics::MAX_BINDLESS_RESOURCES;
            imageSamplerBinding.binding = Graphics::Vulkan::BINDLESS_TEXTURE_BINDING;
            imageSamplerBinding.stageFlags = VK_SHADER_STAGE_ALL;

            VkDescriptorSetLayoutBinding& storageImageBinding = descriptorSetBindings[1];
            storageImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            storageImageBinding.descriptorCount = Graphics::MAX_BINDLESS_RESOURCES;
            storageImageBinding.binding = Graphics::Vulkan::BINDLESS_TEXTURE_BINDING + 1;
            storageImageBinding.stageFlags = VK_SHADER_STAGE_ALL;

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(poolCount);
            descriptorSetLayoutCreateInfo.pBindings = descriptorSetBindings.data();
            descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

            // TODO: Re-enable VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT
            // Binding flags
            VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | /*VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |*/ VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
            InplaceArray<VkDescriptorBindingFlags, 4> descriptorBindingFlags;

            descriptorBindingFlags[0] = bindlessFlags;
            descriptorBindingFlags[1] = bindlessFlags;

            VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr };
            extended_info.bindingCount = poolCount;
            extended_info.pBindingFlags = descriptorBindingFlags.data();

            descriptorSetLayoutCreateInfo.pNext = &extended_info;

            m_BindlessDescriptorSetLayout = MakeUnique<DescriptorSetLayout>(*m_Device, 0, descriptorSetLayoutCreateInfo);

            VkDescriptorSetAllocateInfo descriptorSetAllocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
            descriptorSetAllocInfo.descriptorPool = m_BindlessDescriptorPool->GetHandle();
            descriptorSetAllocInfo.descriptorSetCount = 1;
            descriptorSetAllocInfo.pSetLayouts = m_BindlessDescriptorSetLayout->GetHandlePtr();

            VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
            onyxU32 max_binding = Graphics::MAX_BINDLESS_RESOURCES - 1;
            count_info.descriptorSetCount = 1;
            // This number is the max allocatable count
            count_info.pDescriptorCounts = &max_binding;
            descriptorSetAllocInfo.pNext = &count_info;
            m_BindlessDescriptorSets = MakeUnique<DescriptorSet>(*m_Device, BINDLESS_SET, descriptorSetAllocInfo);
        }

        m_SwapChain = MakeUnique<SwapChain>(*this);

        // TODO: Add proper thread count
        m_CommandBufferManager = MakeUnique<CommandBufferManager>();
        m_CommandBufferManager->Init(*this, m_PhysicalDevice->GetGraphicsQueueIndex(), 4);

        m_ComputeCommandBufferManager = MakeUnique<CommandBufferManager>();
        m_ComputeCommandBufferManager->Init(*this, m_PhysicalDevice->GetComputeQueueIndex(), 4);

        m_GraphicsSemaphore = MakeUnique<Semaphore>(*m_Device, IsTimelineSemaphoreEnabled());
        m_ComputeSemaphore = MakeUnique<Semaphore>(*m_Device, IsTimelineSemaphoreEnabled());
        m_PresentSemaphore = MakeUnique<Semaphore>(*m_Device, IsTimelineSemaphoreEnabled());

        m_GraphicsSingleSubmitFence = MakeUnique<Fence>(*m_Device, false);
        m_ComputeSingleSubmitFence = MakeUnique<Fence>(*m_Device, false);

        SamplerProperties samplerCreateInfo {};
        samplerCreateInfo.MinFilter = SamplerFilter::Linear;
        samplerCreateInfo.MagFilter = SamplerFilter::Linear;
        samplerCreateInfo.MipFilter = SamplerMipMapMode::Linear;
        samplerCreateInfo.AddressModeU = SamplerAddressMode::Repeat;
        samplerCreateInfo.AddressModeV = SamplerAddressMode::Repeat;
        samplerCreateInfo.AddressModeW = SamplerAddressMode::Repeat;

        onyxU32 defaultSamplerHash = samplerCreateInfo.Hash();
        m_Samplers[defaultSamplerHash] = Reference<Sampler>::Create(*m_Device, samplerCreateInfo);

        samplerCreateInfo.AddressModeU = SamplerAddressMode::ClampToEdge;
        samplerCreateInfo.AddressModeV = SamplerAddressMode::ClampToEdge;
        samplerCreateInfo.MipFilter = SamplerMipMapMode::Nearest;
        onyxU32 samplerHash = samplerCreateInfo.Hash();
        m_Samplers[samplerHash] = Reference<Sampler>::Create(*m_Device, samplerCreateInfo);

        samplerCreateInfo.AddressModeW = SamplerAddressMode::ClampToEdge;
        samplerHash = samplerCreateInfo.Hash();
        m_Samplers[samplerHash] = Reference<Sampler>::Create(*m_Device, samplerCreateInfo);

        samplerCreateInfo.MinFilter = SamplerFilter::Nearest;
        samplerCreateInfo.MagFilter = SamplerFilter::Nearest;
        samplerCreateInfo.MipFilter = SamplerMipMapMode::Nearest;
        samplerCreateInfo.AddressModeU = SamplerAddressMode::ClampToEdge;
        samplerCreateInfo.AddressModeV = SamplerAddressMode::ClampToEdge;
        samplerCreateInfo.AddressModeW = SamplerAddressMode::ClampToEdge;

        onyxU32 pointSamplerHash = samplerCreateInfo.Hash();
        m_Samplers[pointSamplerHash] = Reference<Sampler>::Create(*m_Device, samplerCreateInfo);
    }

    void VulkanGraphicsApi::Shutdown()
    {
        m_CommandBufferManager.reset();
        m_ComputeCommandBufferManager.reset();

        m_GraphicsSemaphore.reset();
        m_ComputeSemaphore.reset();
        m_PresentSemaphore.reset();

        m_ComputeSingleSubmitFence.reset();
        m_GraphicsSingleSubmitFence.reset();

        m_Samplers.clear();

        m_BindlessDescriptorSetLayout.reset();
        m_BindlessDescriptorSets.reset();

        m_BindlessDescriptorPool.reset();
        m_DescriptorPool.reset();

        for (InplaceFunction<bool(), 48>& deletionFunctor : m_DeletionQueue)
        {
            std::ignore = deletionFunctor();
        }
        m_DeletionQueue.clear();

        m_Allocator.reset();

        m_SwapChain.reset();
        m_Device.reset();
        m_PhysicalDevice.reset();
        m_Surface.reset();
        m_DebugUtilsMessenger.reset();
        m_Instance.reset();
    }

    bool VulkanGraphicsApi::BeginFrame(const FrameContext& context)
    {
        if (IsTimelineSemaphoreEnabled() && context.AbsoluteFrame > MAX_FRAMES_IN_FLIGHT)
        {
            ONYX_PROFILE_SECTION(SemaphoreWait);

            // present and graphics values should be aligned
            onyxU64 graphics_timeline_value = context.AbsoluteFrame - (MAX_FRAMES_IN_FLIGHT - 1);
            onyxU64 compute_timeline_value = context.ComputeFrame;

            onyxU64 wait_values[]{ graphics_timeline_value, graphics_timeline_value, compute_timeline_value };
            const bool hasAsyncWork = false;

            VkSemaphore semaphores[]{ m_GraphicsSemaphore->GetHandle(), m_PresentSemaphore->GetHandle(), m_ComputeSemaphore->GetHandle() };
            
            VkSemaphoreWaitInfo semaphore_wait_info{ VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO };
            semaphore_wait_info.semaphoreCount = hasAsyncWork ? 3 : 2;
            semaphore_wait_info.pSemaphores = semaphores;
            semaphore_wait_info.pValues = wait_values;

            vkWaitSemaphores(m_Device->GetHandle(), &semaphore_wait_info, ~0ull);
        }

        bool hasAcquiredImage = m_SwapChain->BeginFrame(context.FrameIndex);

        m_CommandBufferManager->Reset(*m_Device, context.FrameIndex);
        m_ComputeCommandBufferManager->Reset(*m_Device, context.FrameIndex);

        return hasAcquiredImage;
    }

    bool VulkanGraphicsApi::EndFrame(const FrameContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        const onyxU8 commandBufferCount = m_QueuedCommandBuffer.size();
        InplaceArray<VkCommandBuffer, 4> enqueuedCommandBuffers;
        for (onyxU8 i = 0; i < commandBufferCount; ++i)
        {
            VulkanCommandBuffer* cmdBuffer = static_cast<VulkanCommandBuffer*>(m_QueuedCommandBuffer[i]);

            enqueuedCommandBuffers.Add(cmdBuffer->GetHandle());
            cmdBuffer->End();
        }

        const onyxU8 computeCommandBufferCount = m_QueuedComputeCommandBuffer.size();
        InplaceArray<VkCommandBuffer, 4> enqueuedComputeCommandBuffers;
        for (onyxU8 i = 0; i < computeCommandBufferCount; ++i)
        {
            VulkanCommandBuffer* cmdBuffer = static_cast<VulkanCommandBuffer*>(m_QueuedComputeCommandBuffer[i]);

            enqueuedComputeCommandBuffers.Add(cmdBuffer->GetHandle());
            cmdBuffer->End();
        }

        // update bindless textures or should update them at the start of the frame?
        onyxU32 currentIndex = 0;
        if (m_BindlessTexturesToUpdate.empty() == false)
        {
            VkWriteDescriptorSet bindlessDescriptorWrites[MAX_BINDLESS_RESOURCES];

            onyxS32 count = static_cast<onyxS32>(m_BindlessTexturesToUpdate.size());
            for (onyxS32 i = 0; i < count; ++i)
            {
                TextureUpdate& textureUpdate = m_BindlessTexturesToUpdate[i];
                //if (textureUpdate.Texture->GetIndex() == false)
                //    continue;

                VkWriteDescriptorSet& descriptor_write = bindlessDescriptorWrites[currentIndex];
                descriptor_write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                descriptor_write.descriptorCount = 1;
                descriptor_write.dstArrayElement = textureUpdate.Index;
                descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptor_write.dstSet = m_BindlessDescriptorSets->GetHandle();
                descriptor_write.dstBinding = BINDLESS_TEXTURE_BINDING;
                descriptor_write.pImageInfo = &(textureUpdate.Texture->GetDescriptorInfo());

                ++currentIndex;
            }

            if (currentIndex != 0)
                vkUpdateDescriptorSets(m_Device->GetHandle(), currentIndex, bindlessDescriptorWrites, 0, nullptr);

            m_BindlessTexturesToUpdate.clear();
        }

        const UniquePtr<Semaphore>& renderCompleteSemaphore = m_SwapChain->GetRenderCompleteSemaphore(context.FrameIndex);
        const UniquePtr<Semaphore>& backbufferAcquiredSemaphore = m_SwapChain->GetBackbufferAcquiredSemaphore(context.FrameIndex);
        // Submit
        if (IsTimelineSemaphoreEnabled())
        {
            const bool shouldWaitForCompute = (context.ComputeFrame > 0) && (computeCommandBufferCount > 0);
            const bool shouldWaitForGraphics = context.AbsoluteFrame >= MAX_FRAMES_IN_FLIGHT;

            // index of the previous frame in flight to be finished
            const onyxU64 waitForFrameIndex = context.AbsoluteFrame - (MAX_FRAMES_IN_FLIGHT - 1);

            if (IsSynchronization2Enabled())
            {
                VkCommandBufferSubmitInfo commandBufferInfo[4]{};
                for (onyxU8 i = 0; i < commandBufferCount; ++i)
                {
                    commandBufferInfo[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
                    commandBufferInfo[i].commandBuffer = enqueuedCommandBuffers[i];
                }

                InplaceArray<VkSemaphoreSubmitInfo, 4> waitSemaphores;
                waitSemaphores.Add( { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, backbufferAcquiredSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0 } );

                if (shouldWaitForCompute)
                    waitSemaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_ComputeSemaphore->GetHandle(), context.ComputeFrame, VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR, 0);

                if (shouldWaitForGraphics)
                    waitSemaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_GraphicsSemaphore->GetHandle(), waitForFrameIndex, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR , 0);

                VkSemaphoreSubmitInfo signalSemaphores[]
                {
                    { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, renderCompleteSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0 },
                    { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_GraphicsSemaphore->GetHandle(), context.AbsoluteFrame + 1, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR , 0 }
                };

                VkSubmitInfo2 submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
                submitInfo.waitSemaphoreInfoCount = waitSemaphores.size();
                submitInfo.pWaitSemaphoreInfos = waitSemaphores.data();
                submitInfo.commandBufferInfoCount = commandBufferCount;
                submitInfo.pCommandBufferInfos = commandBufferInfo;
                submitInfo.signalSemaphoreInfoCount = 2;
                submitInfo.pSignalSemaphoreInfos = signalSemaphores;

                std::lock_guard lock(m_GraphicsMutex);
                VK_CHECK_RESULT(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
            }
            else
            {
                InplaceArray<VkSemaphore, 4> waitSemaphores;
                InplaceArray<onyxU64, 4> waitValues;
                InplaceArray<VkPipelineStageFlags, 4> waitStages;

                waitSemaphores.Add(backbufferAcquiredSemaphore->GetHandle());
                waitValues.Add(0);
                waitStages.Add(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

                if (shouldWaitForCompute)
                {
                    waitSemaphores.Add(m_ComputeSemaphore->GetHandle());
                    waitValues.Add(context.ComputeFrame);
                    waitStages.Add(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
                }

                if (shouldWaitForGraphics)
                {
                    waitSemaphores.Add(m_GraphicsSemaphore->GetHandle());
                    waitValues.Add(waitForFrameIndex);
                    waitStages.Add(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
                }

                /*if (has_pending_sparse_bindings)
                {
                    waitSemaphores.push(vulkan_bind_semaphore);
                    wait_values.push(0);
                    wait_stages.push(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                }*/

                InplaceArray<VkSemaphore, 2> signalSemaphores { renderCompleteSemaphore->GetHandle(), m_GraphicsSemaphore->GetHandle() };
                InplaceArray<onyxU64, 2> signalValues { 0, context.AbsoluteFrame + 1};

                VkTimelineSemaphoreSubmitInfo semaphoreInfo{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
                semaphoreInfo.signalSemaphoreValueCount = 2;
                semaphoreInfo.pSignalSemaphoreValues = signalValues.data();
                semaphoreInfo.waitSemaphoreValueCount = waitValues.size();
                semaphoreInfo.pWaitSemaphoreValues = waitValues.data();

                VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
                submitInfo.waitSemaphoreCount = waitSemaphores.size();
                submitInfo.pWaitSemaphores = waitSemaphores.data();
                submitInfo.pWaitDstStageMask = waitStages.data();
                submitInfo.commandBufferCount = commandBufferCount;
                submitInfo.pCommandBuffers = enqueuedCommandBuffers.data();
                submitInfo.signalSemaphoreCount = 2;
                submitInfo.pSignalSemaphores = signalSemaphores.data();

                submitInfo.pNext = &semaphoreInfo;

                std::lock_guard lock(m_GraphicsMutex);
                VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
            }
        }
        else
        {
            const UniquePtr<Fence>& renderCompleteFence = m_SwapChain->GetRenderCompleteFence(context.FrameIndex);

            if (IsSynchronization2Enabled())
            {
                InplaceArray<VkCommandBufferSubmitInfoKHR, 4> commandBufferInfo;
                for (onyxU8 i = 0; i < commandBufferCount; ++i)
                {
                    commandBufferInfo[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
                    commandBufferInfo[i].commandBuffer = enqueuedCommandBuffers[i];
                }

                InplaceArray<VkSemaphoreSubmitInfoKHR, 4> wait_semaphores;
                wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, backbufferAcquiredSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0);
                wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_ComputeSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR, 0);

                //if (has_pending_sparse_bindings)
                //  wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, vulkan_bind_semaphore, 0, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR, 0);

                InplaceArray<VkSemaphoreSubmitInfoKHR, 1> signalSemaphores
                {
                    { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, renderCompleteSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0 }
                };

                VkSubmitInfo2 submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
                submitInfo.waitSemaphoreInfoCount = wait_semaphores.size();
                submitInfo.pWaitSemaphoreInfos = wait_semaphores.data();
                submitInfo.commandBufferInfoCount = commandBufferCount;
                submitInfo.pCommandBufferInfos = commandBufferInfo.data();
                submitInfo.signalSemaphoreInfoCount = 1;
                submitInfo.pSignalSemaphoreInfos = signalSemaphores.data();

                std::lock_guard lock(m_GraphicsMutex);
                VK_CHECK_RESULT(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo, renderCompleteFence->GetHandle()));
            }
            else
            {
                InplaceArray<VkSemaphore, 4> waitSemaphores;
                InplaceArray<VkPipelineStageFlags, 4> waitStages;

                waitSemaphores.Add(backbufferAcquiredSemaphore->GetHandle());
                waitStages.Add(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

                waitSemaphores.Add(m_ComputeSemaphore->GetHandle());
                waitStages.Add(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

                /*if (has_pending_sparse_bindings)
                {
                    wait_semaphores.push(vulkan_bind_semaphore);
                    wait_stages.push(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                }*/

                VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
                submitInfo.waitSemaphoreCount = waitSemaphores.size();
                submitInfo.pWaitSemaphores = waitSemaphores.data();
                submitInfo.pWaitDstStageMask = waitStages.data();
                submitInfo.commandBufferCount = commandBufferCount;
                submitInfo.pCommandBuffers = enqueuedCommandBuffers.data();
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = renderCompleteSemaphore->GetHandlePtr();

                std::lock_guard lock(m_GraphicsMutex);
                VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, renderCompleteFence->GetHandle()));
            }
        }

        // submit compute buffers
        //m_HasComputeWork = false;
        // move to function
        if (computeCommandBufferCount != 0)
        {
            bool hasWaitSemaphore = context.ComputeFrame > 0;
            VkSemaphoreSubmitInfoKHR wait_semaphores[]
            {
                { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_ComputeSemaphore->GetHandle(), context.ComputeFrame, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR, 0 }
            };

            
            VkSemaphoreSubmitInfoKHR signal_semaphores[]{
                { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, m_ComputeSemaphore->GetHandle(), context.ComputeFrame + 1, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR, 0 },
            };

            InplaceArray<VkCommandBufferSubmitInfoKHR, 4> commandBufferInfo;
            for (onyxU8 i = 0; i < computeCommandBufferCount; ++i)
            {
                commandBufferInfo[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
                commandBufferInfo[i].commandBuffer = enqueuedComputeCommandBuffers[i];
            }

            VkSubmitInfo2 submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
            submit_info.waitSemaphoreInfoCount = hasWaitSemaphore ? 1 : 0;
            submit_info.pWaitSemaphoreInfos = wait_semaphores;
            submit_info.commandBufferInfoCount = commandBufferInfo.size();
            submit_info.pCommandBufferInfos = commandBufferInfo.data();
            submit_info.signalSemaphoreInfoCount = 1;
            submit_info.pSignalSemaphoreInfos = signal_semaphores;

            vkQueueSubmit2(m_Device->GetComputeQueue(), 1, &submit_info, VK_NULL_HANDLE);
        }

        m_QueuedCommandBuffer.Clear();
        m_QueuedComputeCommandBuffer.Clear();

        // Delete resources
        for (auto it = m_DeletionQueue.begin(); it != m_DeletionQueue.end();)
        {
            if (it->operator()())
            {
                it = m_DeletionQueue.erase(it);
            }
            else
            {
                ++it;
            }
        }
        //m_DeletionQueue.clear();

        return true;
    }

    void VulkanGraphicsApi::SignalPresent(onyxU32 presentIndex)
    {
        VkSemaphoreSignalInfo signalInfo = {};
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
        signalInfo.semaphore = m_PresentSemaphore->GetHandle();
        signalInfo.value = presentIndex;

        //// Signal that the presented image is now free
        vkSignalSemaphore(m_Device->GetHandle(), &signalInfo);

        //m_ConditionVariable.notify_one();
    }

    Reference<Graphics::Sampler> VulkanGraphicsApi::GetSampler(SamplerProperties properties) const
    {
        onyxU32 hash = properties.Hash();
        if (m_Samplers.contains(hash) == false)
        {
            //, "Unknown sampler");
            return {};
        }

        return m_Samplers.at(hash);
    }

    std::lock_guard<std::mutex> VulkanGraphicsApi::LockGraphicsQueue()
    {
        return std::lock_guard(m_GraphicsMutex);
    }

    void VulkanGraphicsApi::ReleaseTexture(const VulkanTexture& texture)
    {
        ONYX_ASSERT(texture.GetRefCount() == 0);
        m_DeletionQueue.emplace_back([textureIndex = texture.GetIndex(), this]() mutable
        {
            m_Textures.Release(textureIndex);
            return true;
        });
    }

    void VulkanGraphicsApi::WaitIdle() const
    {
        vkDeviceWaitIdle(m_Device->GetHandle());
        vkQueueWaitIdle(m_Device->GetGraphicsQueue());
    }

    void VulkanGraphicsApi::OnWindowResize(onyxU32 /*width*/, onyxU32 /*height*/)
    {
    }

    CommandBuffer& VulkanGraphicsApi::GetCommandBuffer(onyxU8 frameIndex)
    {
        return GetCommandBuffer(frameIndex, false);
    }

    CommandBuffer& VulkanGraphicsApi::GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        CommandBuffer& commandBuffer = m_CommandBufferManager->GetCommandBuffer(frameIndex, 0, shouldBegin);
        m_QueuedCommandBuffer.Add(&commandBuffer);
        return commandBuffer;
    }

    CommandBuffer& VulkanGraphicsApi::GetComputeCommandBuffer(onyxU8 frameIndex)
    {
        return GetCommandBuffer(frameIndex, false);
    }

    CommandBuffer& VulkanGraphicsApi::GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        CommandBuffer& commandBuffer = m_ComputeCommandBufferManager->GetCommandBuffer(frameIndex, 0, shouldBegin);
        m_QueuedComputeCommandBuffer.Add(&commandBuffer);
        return commandBuffer;
    }

    void VulkanGraphicsApi::SubmitInstantCommandBuffer(Context context, onyxU8 frameIndex, InplaceFunction<void(CommandBuffer&)>&& functor)
    {
        CommandBufferManager* commandBufferManager = nullptr;
        VkQueue queue = nullptr;
        VkFence fence = nullptr;
        switch (context)
        {
            case Context::Graphics:
                commandBufferManager = m_CommandBufferManager.get();
                queue = m_Device->GetGraphicsQueue();
                fence = m_GraphicsSingleSubmitFence->GetHandle();
                break;
            case Context::Compute:
                commandBufferManager = m_ComputeCommandBufferManager.get();
                queue = m_Device->GetComputeQueue();
                fence = m_ComputeSingleSubmitFence->GetHandle();
                break;
            case Context::CopyTransfer:
                break;
            case Context::Count:
                ONYX_ASSERT(false, "Invalid queue type");
                break;
        }

        if (queue == nullptr)
            return;

        UniquePtr<CommandBuffer> commandBuffer = commandBufferManager->CreateSingleSubmitCommandBuffer(*this, frameIndex, 0);
        VulkanCommandBuffer& vulkanCmdBuffer = static_cast<VulkanCommandBuffer&>(*commandBuffer);

        vulkanCmdBuffer.Begin();
        functor(vulkanCmdBuffer);
        vulkanCmdBuffer.End();

        // Submit command buffer
        if (IsSynchronization2Enabled())
        {
            VkCommandBufferSubmitInfoKHR commandBufferInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR };
            commandBufferInfo.commandBuffer = vulkanCmdBuffer.GetHandle();

            VkSubmitInfo2KHR submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR };
            submitInfo.commandBufferInfoCount = 1;
            submitInfo.pCommandBufferInfos = &commandBufferInfo;

            std::lock_guard lock(m_GraphicsMutex);
            VK_CHECK_RESULT(vkQueueSubmit2(queue, 1, &submitInfo, fence))
        }
        else
        {
            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = vulkanCmdBuffer.GetHandlePtr();

            VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence))
        }

        vkWaitForFences(m_Device->GetHandle(), 1, &fence, true, 9999999);
        vkResetFences(m_Device->GetHandle(), 1, &fence);

        if (context == Context::Graphics)
        {
            std::lock_guard lock(m_GraphicsMutex);
            vkQueueWaitIdle(queue);
        }
    }

    const TextureHandle& VulkanGraphicsApi::GetAcquiredSwapChainImage(onyxU8 i) const
    {
        return m_SwapChain->GetAcquiredBackbuffer(i);
    }

    onyxU32 VulkanGraphicsApi::GetAcquiredBackbufferIndex() const
    {
        return m_SwapChain->GetAcquiredBackbufferIndex();
    }

    TextureFormat VulkanGraphicsApi::GetSwapchainTextureFormat() const
    {
        return m_SwapChain->GetFormat();
    }

    const Vector2s32& VulkanGraphicsApi::GetSwapchainExtent() const
    {
        return m_SwapChain->GetExtent();
    }

    RenderPassHandle VulkanGraphicsApi::CreateRenderPass(const RenderPassSettings& settings)
    {
        // TODO: Test if object pool + index based handle is faster / better
        /*RenderPassHandle handle;
        handle.Handle = m_RenderPasses.AcquireObject();

        if (handle)
        {
            VulkanRenderPass& renderPass = m_RenderPasses[handle];
            renderPass.Init(*m_Device, settings);
        }

        return handle;*/
        Reference<VulkanRenderPass> renderPass = Reference<VulkanRenderPass>::Create();
        renderPass->Init(*this, settings);
        return renderPass;
    }

    FramebufferHandle VulkanGraphicsApi::CreateFramebuffer(const FramebufferSettings& settings)
    {
        /*FramebufferHandle handle;
        handle.Handle = m_Framebuffers.AcquireObject();

        if (handle)
        {
            VulkanFramebuffer& framebuffer = m_Framebuffers[handle.Handle];
            framebuffer.Init(*this, settings);
        }

        return handle;*/

        Reference<VulkanFramebuffer> framebuffer = Reference<VulkanFramebuffer>::Create();
        framebuffer->Init(*this, settings);
        return framebuffer;
    }

    void VulkanGraphicsApi::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties)
    {
        TextureHandle handle;
        Reference<VulkanTextureStorage> storage = Reference<VulkanTextureStorage>::Create(*this, storageProperties);

        handle.Storage = storage;
        CreateTextureView(handle, storage, properties);

        std::swap(outTexture.Storage, handle.Storage);
        std::swap(outTexture.Texture, handle.Texture);
        //std::swap(outTexture., handle.Index);

        /*if (handle)
        {
            m_DeletionQueue.emplace_back([handleToDelete = handle, this]() mutable
                {
                    if (handleToDelete.Texture && handleToDelete.Texture->GetRefCount() == 1 && handleToDelete.Storage->GetRefCount() == 1)
                    {
                        ONYX_ASSERT(handleToDelete.Texture->GetRefCount() == 1);
                        ONYX_ASSERT(handleToDelete.Storage->GetRefCount() == 1);

                        std::ignore = handleToDelete.Texture.Release();
                        m_Textures.Release(handleToDelete.Index);
                        handleToDelete.Storage.Reset();
                        return true;
                    }

                    return false;
                });
        }*/
    }

    void VulkanGraphicsApi::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData)
    {
        TextureHandle handle;
        Reference<VulkanTextureStorage> storage = Reference<VulkanTextureStorage>::Create(*this, storageProperties, initialData);

        handle.Storage = storage;
        CreateTextureView(handle, storage, properties);

        std::swap(outTexture.Storage, handle.Storage);
        std::swap(outTexture.Texture, handle.Texture);
        
        /*if (handle)
        {
            m_DeletionQueue.emplace_back([handleToDelete = handle, this]() mutable
                {
                    if (handleToDelete.Texture && handleToDelete.Texture->GetRefCount() == 1 && handleToDelete.Storage->GetRefCount() == 1)
                    {
                        ONYX_ASSERT(handleToDelete.Texture->GetRefCount() == 1);
                        ONYX_ASSERT(handleToDelete.Storage->GetRefCount() == 1);

                        std::ignore = handleToDelete.Texture.Release();
                        m_Textures.Release(handleToDelete.Index);
                        handleToDelete.Storage.Reset();
                        return true;
                    }

                    return false;
                });
        }*/
    }

    void VulkanGraphicsApi::CreateTextureView(TextureHandle& handle, const Reference<VulkanTextureStorage>& textureStorage, const TextureProperties& properties)
    {
        onyxU32 index;

        VulkanTexture* texture = m_Textures.AcquireAndEmplace(index, *this, properties, textureStorage.Raw());
        texture->SetIndex(index);

        handle.Texture = texture;

        ONYX_LOG_INFO("Allocated texture on index {} with name {}", index, properties.m_DebugName);
        m_BindlessTexturesToUpdate.push_back({ index, texture });
    }

    void VulkanGraphicsApi::CreateAlias(TextureHandle& outTexture, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties)
    {
        ONYX_ASSERT(storageHandle, "Storage handle is invalid");

        VulkanTextureStorage& parentStorage = storageHandle.As<VulkanTextureStorage>();
        ONYX_ASSERT(parentStorage.GetHandle() != nullptr, "Storage for texture is not valid");
        
        outTexture.Storage = storageHandle;
        outTexture.Alias = parentStorage.Alias(aliasStorageProperties);

        onyxU32 index;
        VulkanTexture* texture = m_Textures.AcquireAndEmplace(index, *this, aliasTextureProperties, &parentStorage, outTexture.Alias);
        texture->SetIndex(index);

        outTexture.Texture = texture;
        m_BindlessTexturesToUpdate.push_back({ index, texture });
    }

    void VulkanGraphicsApi::CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties)
    {
        outBuffer = Reference<VulkanBuffer>::Create(*this, properties);
    }

    PipelineHandle VulkanGraphicsApi::CreatePipeline(const PipelineProperties& properties)
    {
        return Reference<Pipeline>::Create(*this, properties);
    }

    ShaderHandle VulkanGraphicsApi::CreateShader(InplaceArray<DynamicArray<onyxU32>, MAX_SHADER_STAGES>& perStageByteCode)
    {
        return Reference<Shader>::Create(*this, perStageByteCode);
    }

    DynamicArray<DescriptorSetHandle> VulkanGraphicsApi::CreateDescriptorSet(const ShaderHandle& shader, const StringView& debugName)
    {
        DynamicArray<DescriptorSetHandle> descriptorSets;

        const Shader& vulkanShader = shader.As<Shader>();
        const InplaceArray<UniquePtr<DescriptorSetLayout>, MAX_DESCRIPTOR_SET_LAYOUTS>& descriptorSetLayouts = vulkanShader.GetDescriptorSetLayouts();
        for (const UniquePtr<DescriptorSetLayout>& layout : descriptorSetLayouts)
        {
            descriptorSets.push_back(Reference<DescriptorSet>::Create(*m_Device, *m_DescriptorPool, *layout, debugName));
        }
        
        return descriptorSets;
        
    }
}
