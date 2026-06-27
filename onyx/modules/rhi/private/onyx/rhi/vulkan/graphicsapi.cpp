#include <onyx/rhi/vulkan/graphicsapi.h>

#include <onyx/platform/window.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphiclimits.h>
#include <onyx/rhi/graphicsettings.h>
#include <onyx/rhi/vulkan/commandbuffermanager.h>
#include <onyx/rhi/vulkan/debugutilsmessenger.h>
#include <onyx/rhi/vulkan/descriptorpool.h>
#include <onyx/rhi/vulkan/descriptorset.h>
#include <onyx/rhi/vulkan/descriptorsetlayout.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/fence.h>
#include <onyx/rhi/vulkan/framebuffer.h>
#include <onyx/rhi/vulkan/instance.h>
#include <onyx/rhi/vulkan/memoryallocator.h>
#include <onyx/rhi/vulkan/physicaldevice.h>
#include <onyx/rhi/vulkan/pipeline.h>
#include <onyx/rhi/vulkan/renderpass.h>
#include <onyx/rhi/vulkan/sampler.h>
#include <onyx/rhi/vulkan/semaphore.h>
#include <onyx/rhi/vulkan/shader.h>
#include <onyx/rhi/vulkan/surface.h>
#include <onyx/rhi/vulkan/swapchain.h>
#include <onyx/rhi/vulkan/texture.h>
#include <onyx/rhi/vulkan/texturestorage.h>

namespace onyx::rhi::vulkan {
// needed for UniquePtr forward declarations
VulkanGraphicsApi::VulkanGraphicsApi() = default;
VulkanGraphicsApi::~VulkanGraphicsApi() = default;

void VulkanGraphicsApi::init( GraphicLimits& limits, const GraphicSettings& settings ) {
    DynamicArray< const char* > validationLayers = { "VK_LAYER_KHRONOS_validation" };

    m_instance = makeUnique< Instance >( settings, validationLayers );

    if( settings.IsDebugEnabled ) {
        uint32_t severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        m_debugUtilsMessenger = makeUnique< DebugUtilsMessenger >(
            *m_instance,
            static_cast< VkDebugUtilsMessageSeverityFlagBitsEXT >( severity ) );
    }

    m_physicalDevice = makeUnique< PhysicalDevice >( *m_instance );
    limits.MaxBufferSize = m_physicalDevice->getMaxBufferSize();

    // Device Extensions
    // TODO: can this be moved somehow into Logiccal Device=
    DynamicArray< const char* > deviceExtensions;

    VkPhysicalDeviceVulkan11Features vulkan11Features;
    vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    vulkan11Features.pNext = nullptr;

    void* currentPnext = &vulkan11Features;

    VkPhysicalDeviceVulkan12Features vulkan12Features;
    vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vulkan12Features.pNext = currentPnext;
    currentPnext = &vulkan12Features;

    VkPhysicalDeviceVulkan13Features vulkan13Features;
    vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vulkan13Features.maintenance4 = true;
    vulkan13Features.pNext = currentPnext;

    currentPnext = &vulkan13Features;

    deviceExtensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
    deviceExtensions.push_back( VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME );
    deviceExtensions.push_back( VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME );

    // Tracy gpu query extensions
    deviceExtensions.push_back( VK_KHR_CALIBRATED_TIMESTAMPS_EXTENSION_NAME );
    deviceExtensions.push_back( VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME );
    //

    if( m_physicalDevice->isExtensionSupported( VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME ) ) {
        m_isTimelineSemaphoreEnabled = true;
    }

    if( m_physicalDevice->isExtensionSupported( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME ) ) {
        m_isSynchronization2Enabled = true;
    }

    if( m_physicalDevice->isExtensionSupported( VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME ) ) {
        m_isDynamicRenderingEnabled = true;
    }

#if VK_KHR_unified_image_layouts
    if( m_physicalDevice->isExtensionSupported( VK_KHR_UNIFIED_IMAGE_LAYOUTS_EXTENSION_NAME ) ) {
        deviceExtensions.push_back( VK_KHR_UNIFIED_IMAGE_LAYOUTS_EXTENSION_NAME );
        m_isUnifiedImageLayoutSupported = true;
    }
#endif

    if( m_isDynamicRenderingEnabled == false ) {
        if( m_physicalDevice->isExtensionSupported( VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME ) ) {
            deviceExtensions.push_back( VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME );
            m_isRenderPass2ExtensionEnabled = true;
        }
    }

    VkPhysicalDeviceDescriptorIndexingFeatures bindlessExtenstion;
    if( m_physicalDevice->isExtensionSupported( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME ) ) {
        bindlessExtenstion.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        bindlessExtenstion.pNext = nullptr;

        VkPhysicalDeviceFeatures2 deviceFeatures{};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures.pNext = &bindlessExtenstion;
        vkGetPhysicalDeviceFeatures2( m_physicalDevice->GetHandle(), &deviceFeatures );

        m_isBindlessEnabled = bindlessExtenstion.descriptorBindingPartiallyBound &&
                              bindlessExtenstion.runtimeDescriptorArray;
    }

    VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderExtension{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT };
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingExtension{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR };

    if( m_physicalDevice->isExtensionSupported( VK_EXT_MESH_SHADER_EXTENSION_NAME ) ) {
        deviceExtensions.push_back( VK_EXT_MESH_SHADER_EXTENSION_NAME );

        meshShaderExtension.taskShader = true;
        meshShaderExtension.meshShader = true;
        meshShaderExtension.primitiveFragmentShadingRateMeshShader = true;

        fragmentShadingExtension.primitiveFragmentShadingRate = true;

        fragmentShadingExtension.pNext = currentPnext;
        currentPnext = &fragmentShadingExtension;

        meshShaderExtension.pNext = currentPnext;
        currentPnext = &meshShaderExtension;
    }

    VkPhysicalDeviceFeatures2 physicalFeatures;
    physicalFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalFeatures.pNext = currentPnext;
    // End device extensions

    m_device = makeUnique< Device >( *m_physicalDevice,
                                     deviceExtensions,
                                     physicalFeatures,
                                     VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT );

    g_vkCmdDrawMeshTasks = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr( m_device->GetHandle(),
                                                                           "vkCmdDrawMeshTasksEXT" );
    g_vkCmdDrawMeshTasksIndirect = (PFN_vkCmdDrawMeshTasksIndirectEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkCmdDrawMeshTasksIndirectEXT" );
    g_vkCmdDrawMeshTasksIndirectCount = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkCmdDrawMeshTasksIndirectCountEXT" );
    g_vkSetDebugUtilsObjectNameExt = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkSetDebugUtilsObjectNameEXT" );
    g_vkCmdBeginDebugUtilsLabelExt = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkCmdBeginDebugUtilsLabelEXT" );
    g_vkCmdEndDebugUtilsLabelExt = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr( m_device->GetHandle(),
                                                                                        "vkCmdEndDebugUtilsLabelEXT" );

    g_vkResetQueryPoolExt = (PFN_vkResetQueryPoolEXT)vkGetDeviceProcAddr( m_device->GetHandle(), "vkResetQueryPool" );
    g_vkGetPhysicalDeviceCalibrateableTimeDomainsExt = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT)
        vkGetInstanceProcAddr( m_instance->GetHandle(), "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT" );
    g_vkGetCalibratedTimestampsExt = (PFN_vkGetCalibratedTimestampsEXT)vkGetInstanceProcAddr(
        m_instance->GetHandle(),
        "vkGetCalibratedTimestampsEXT" );

    g_vkCmdBeginConditionalRenderingExt = (PFN_vkCmdBeginConditionalRenderingEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkCmdBeginConditionalRenderingEXT" );
    g_vkCmdEndConditionalRenderingExt = (PFN_vkCmdEndConditionalRenderingEXT)vkGetDeviceProcAddr(
        m_device->GetHandle(),
        "vkCmdEndConditionalRenderingEXT" );

    m_allocator = makeUnique< MemoryAllocator >( "Default Allocator", *this );

    constexpr uint32_t MaxPoolElements = 128;
    DynamicArray< VkDescriptorPoolSize > pools{ { VK_DESCRIPTOR_TYPE_SAMPLER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MaxPoolElements },
                                                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, MaxPoolElements } };

    m_descriptorPool = makeUnique< DescriptorPool >( *m_device,
                                                     pools,
                                                     VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                                     MaxPoolElements );
    if( isBindless() ) {
        pools = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, rhi::MAX_BINDLESS_RESOURCES },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, rhi::MAX_BINDLESS_RESOURCES },
        };
        m_bindlessDescriptorPool = makeUnique< DescriptorPool >( *m_device,
                                                                 pools,
                                                                 VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT,
                                                                 rhi::MAX_BINDLESS_RESOURCES );

        const uint32_t poolCount = static_cast< uint32_t >( pools.size() );

        // change to static array?
        InplaceArray< VkDescriptorSetLayoutBinding, 4 > descriptorSetBindings;
        VkDescriptorSetLayoutBinding& imageSamplerBinding = descriptorSetBindings[ 0 ];
        imageSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageSamplerBinding.descriptorCount = rhi::MAX_BINDLESS_RESOURCES;
        imageSamplerBinding.binding = rhi::vulkan::BindlessTextureBinding;
        imageSamplerBinding.stageFlags = VK_SHADER_STAGE_ALL;

        VkDescriptorSetLayoutBinding& storageImageBinding = descriptorSetBindings[ 1 ];
        storageImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        storageImageBinding.descriptorCount = rhi::MAX_BINDLESS_RESOURCES;
        storageImageBinding.binding = rhi::vulkan::BindlessTextureBinding + 1;
        storageImageBinding.stageFlags = VK_SHADER_STAGE_ALL;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( poolCount );
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetBindings.data();
        descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

        // TODO: Re-enable VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT
        // Binding flags
        VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
                                                 /*VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |*/
                                                 VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
        InplaceArray< VkDescriptorBindingFlags, 4 > descriptorBindingFlags;

        descriptorBindingFlags[ 0 ] = bindlessFlags;
        descriptorBindingFlags[ 1 ] = bindlessFlags;

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo;
        extendedInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        extendedInfo.bindingCount = poolCount;
        extendedInfo.pBindingFlags = descriptorBindingFlags.data();
        extendedInfo.pNext = nullptr;

        descriptorSetLayoutCreateInfo.pNext = &extendedInfo;

        m_bindlessDescriptorSetLayout = makeUnique< DescriptorSetLayout >( *m_device,
                                                                           0,
                                                                           descriptorSetLayoutCreateInfo );

        VkDescriptorSetAllocateInfo descriptorSetAllocInfo;
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = m_bindlessDescriptorPool->GetHandle();
        descriptorSetAllocInfo.descriptorSetCount = 1;
        descriptorSetAllocInfo.pSetLayouts = m_bindlessDescriptorSetLayout->GetHandlePtr();

        VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo;
        countInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
        uint32_t maxBinding = rhi::MAX_BINDLESS_RESOURCES - 1;
        countInfo.descriptorSetCount = 1;
        // This number is the max allocatable count
        countInfo.pDescriptorCounts = &maxBinding;
        countInfo.pNext = nullptr;

        descriptorSetAllocInfo.pNext = &countInfo;
        m_bindlessDescriptorSets = makeUnique< DescriptorSet >( *m_device, BINDLESS_SET, descriptorSetAllocInfo );
    }

    BufferProperties tempFrameBuffer;
    tempFrameBuffer.m_DebugName = "TransientBuffer-0";
    tempFrameBuffer.m_Size = 1ull << 28ull; // 8mb
    tempFrameBuffer.m_UsageFlags = static_cast< uint8_t >( BufferUsage::Storage | BufferUsage::Indirect |
                                                           BufferUsage::DeviceAddress );
    tempFrameBuffer.m_CpuAccess = CPUAccess::Write;
    tempFrameBuffer.m_GpuAccess = GPUAccess::Write;
    // tempFrameBuffer.m_IsWritable = true;

    for( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        tempFrameBuffer.m_DebugName = format::format( "TransientBuffer-{}", i );
        createBuffer( m_ringBuffer[ i ], tempFrameBuffer );
    }

    // TODO: Add proper thread count
    m_commandBufferManager = makeUnique< CommandBufferManager >();
    m_commandBufferManager->Init( *this, m_physicalDevice->getGraphicsQueueIndex(), 4 );

    m_computeCommandBufferManager = makeUnique< CommandBufferManager >();
    m_computeCommandBufferManager->Init( *this, m_physicalDevice->getComputeQueueIndex(), 4 );

    m_graphicsSemaphore = makeUnique< Semaphore >( *m_device, isTimelineSemaphoreEnabled() );
    m_computeSemaphore = makeUnique< Semaphore >( *m_device, isTimelineSemaphoreEnabled() );

    m_graphicsSingleSubmitFence = makeUnique< Fence >( *m_device, false );
    m_computeSingleSubmitFence = makeUnique< Fence >( *m_device, false );

    SamplerProperties samplerCreateInfo{};
    samplerCreateInfo.MinFilter = SamplerFilter::Linear;
    samplerCreateInfo.MagFilter = SamplerFilter::Linear;
    samplerCreateInfo.MipFilter = SamplerMipMapMode::Linear;
    samplerCreateInfo.AddressModeU = SamplerAddressMode::Repeat;
    samplerCreateInfo.AddressModeV = SamplerAddressMode::Repeat;
    samplerCreateInfo.AddressModeW = SamplerAddressMode::Repeat;

    uint32_t defaultSamplerHash = samplerCreateInfo.Hash();
    m_samplers[ defaultSamplerHash ] = Reference< Sampler >::create( *m_device, samplerCreateInfo );

    samplerCreateInfo.AddressModeU = SamplerAddressMode::ClampToEdge;
    samplerCreateInfo.AddressModeV = SamplerAddressMode::ClampToEdge;
    samplerCreateInfo.MipFilter = SamplerMipMapMode::Nearest;
    uint32_t samplerHash = samplerCreateInfo.Hash();
    m_samplers[ samplerHash ] = Reference< Sampler >::create( *m_device, samplerCreateInfo );

    samplerCreateInfo.AddressModeW = SamplerAddressMode::ClampToEdge;
    samplerHash = samplerCreateInfo.Hash();
    m_samplers[ samplerHash ] = Reference< Sampler >::create( *m_device, samplerCreateInfo );

    samplerCreateInfo.MinFilter = SamplerFilter::Nearest;
    samplerCreateInfo.MagFilter = SamplerFilter::Nearest;
    samplerCreateInfo.MipFilter = SamplerMipMapMode::Nearest;
    samplerCreateInfo.AddressModeU = SamplerAddressMode::ClampToEdge;
    samplerCreateInfo.AddressModeV = SamplerAddressMode::ClampToEdge;
    samplerCreateInfo.AddressModeW = SamplerAddressMode::ClampToEdge;

    uint32_t pointSamplerHash = samplerCreateInfo.Hash();
    m_samplers[ pointSamplerHash ] = Reference< Sampler >::create( *m_device, samplerCreateInfo );
}

void VulkanGraphicsApi::shutdown() {
    m_ringBuffer.clear();

    m_commandBufferManager.reset();
    m_computeCommandBufferManager.reset();

    m_graphicsSemaphore.reset();
    m_computeSemaphore.reset();
    m_computeSingleSubmitFence.reset();
    m_graphicsSingleSubmitFence.reset();

    m_samplers.clear();

    m_bindlessDescriptorSetLayout.reset();
    m_bindlessDescriptorSets.reset();

    m_bindlessDescriptorPool.reset();
    m_descriptorPool.reset();

    for( InplaceFunction< bool(), 48 >& deletionFunctor : m_deletionQueue ) {
        std::ignore = deletionFunctor();
    }
    m_deletionQueue.clear();

    m_allocator.reset();

    m_swapChain.reset();
    m_device.reset();
    m_physicalDevice.reset();
    m_surface.reset();
    m_debugUtilsMessenger.reset();
    m_instance.reset();
}

bool VulkanGraphicsApi::beginFrame( const FrameContext& context ) {
    if( m_swapChain == nullptr )
        return false;

    if( isTimelineSemaphoreEnabled() && context.AbsoluteFrame >= MAX_FRAMES_IN_FLIGHT ) {
        ONYX_PROFILE_SECTION( SemaphoreWait );

        uint64_t graphicsTimelineValue = context.AbsoluteFrame - ( MAX_FRAMES_IN_FLIGHT - 1 );
        uint64_t computeTimelineValue = context.ComputeFrame;

        uint64_t waitValues[]{ graphicsTimelineValue, computeTimelineValue };
        const bool hasAsyncWork = false;

        VkSemaphore semaphores[]{ m_graphicsSemaphore->GetHandle(), m_computeSemaphore->GetHandle() };

        VkSemaphoreWaitInfo semaphoreWaitInfo{};
        semaphoreWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
        semaphoreWaitInfo.semaphoreCount = hasAsyncWork ? 2 : 1;
        semaphoreWaitInfo.pSemaphores = semaphores;
        semaphoreWaitInfo.pValues = waitValues;
        semaphoreWaitInfo.pNext = nullptr;

        vkWaitSemaphores( m_device->GetHandle(), &semaphoreWaitInfo, std::numeric_limits< uint64_t >::max() );
    }

    bool hasAcquiredImage = m_swapChain->BeginFrame( context.FrameIndex );

    m_commandBufferManager->Reset( *m_device, context.FrameIndex );
    m_computeCommandBufferManager->Reset( *m_device, context.FrameIndex );
    m_currentRingBufferSize = 0;
    m_ringBuffer[ context.FrameIndex ].Buffer->ClearAliases();

    return hasAcquiredImage;
}

bool VulkanGraphicsApi::endFrame( const FrameContext& context ) {
    ONYX_PROFILE_FUNCTION;

    const uint8_t commandBufferCount = m_queuedCommandBuffer.size();
    InplaceArray< VkCommandBuffer, CommandBufferCount > enqueuedCommandBuffers;
    for( uint8_t i = 0; i < commandBufferCount; ++i ) {
        VulkanCommandBuffer* cmdBuffer = static_cast< VulkanCommandBuffer* >( m_queuedCommandBuffer[ i ] );

        enqueuedCommandBuffers.add( cmdBuffer->GetHandle() );
        cmdBuffer->end();
    }

    const uint8_t computeCommandBufferCount = m_queuedComputeCommandBuffer.size();
    InplaceArray< VkCommandBuffer, CommandBufferCount > enqueuedComputeCommandBuffers;
    for( uint8_t i = 0; i < computeCommandBufferCount; ++i ) {
        VulkanCommandBuffer* cmdBuffer = static_cast< VulkanCommandBuffer* >( m_queuedComputeCommandBuffer[ i ] );

        enqueuedComputeCommandBuffers.add( cmdBuffer->GetHandle() );
        cmdBuffer->end();
    }

    // update bindless textures or should update them at the start of the frame?
    uint32_t currentIndex = 0;
    if( m_bindlessTexturesToUpdate.empty() == false ) {
        DynamicArray< VkWriteDescriptorSet > bindlessDescriptorWrites;
        bindlessDescriptorWrites.reserve( MAX_BINDLESS_RESOURCES );

        int32_t count = static_cast< int32_t >( m_bindlessTexturesToUpdate.size() );
        for( int32_t i = 0; i < count; ++i ) {
            TextureUpdate& textureUpdate = m_bindlessTexturesToUpdate[ i ];
            // TODO: This is probably not the best way to handle textures that get allocated and dealloacted in the same
            // frame TextureDeleter clears the index which is a bit hacky just to ensure resizing of the depth texture
            if( textureUpdate.Texture->GetIndex() == std::numeric_limits< uint32_t >::max() )
                continue;

            VkWriteDescriptorSet& descriptorWrite = bindlessDescriptorWrites.emplace_back();
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.dstArrayElement = textureUpdate.Index;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.dstSet = m_bindlessDescriptorSets->GetHandle();
            descriptorWrite.dstBinding = BindlessTextureBinding;
            descriptorWrite.pImageInfo = &( textureUpdate.Texture->GetDescriptorInfo() );

            ++currentIndex;
        }

        if( currentIndex != 0 )
            vkUpdateDescriptorSets( m_device->GetHandle(), currentIndex, bindlessDescriptorWrites.data(), 0, nullptr );

        m_bindlessTexturesToUpdate.clear();
    }

    uint8_t backBufferIndex = numericCast< uint8_t >( m_swapChain->GetAcquiredBackbufferIndex() );
    const UniquePtr< Semaphore >& renderCompleteSemaphore = m_swapChain->GetRenderCompleteSemaphore( backBufferIndex );
    const UniquePtr< Semaphore >& backbufferAcquiredSemaphore = m_swapChain->GetBackbufferAcquiredSemaphore(
        context.FrameIndex );
    // Submit
    if( isTimelineSemaphoreEnabled() ) {
        const bool shouldWaitForCompute = ( context.ComputeFrame > 0 ) && ( computeCommandBufferCount > 0 );
        const bool shouldWaitForGraphics = context.AbsoluteFrame >= MAX_FRAMES_IN_FLIGHT;

        // index of the previous frame in flight to be finished
        const uint64_t waitForFrameIndex = context.AbsoluteFrame - ( MAX_FRAMES_IN_FLIGHT - 1 );

        if( isSynchronization2Enabled() ) {
            VkCommandBufferSubmitInfo commandBufferInfo[ CommandBufferCount ]{};
            for( uint8_t i = 0; i < commandBufferCount; ++i ) {
                commandBufferInfo[ i ].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
                commandBufferInfo[ i ].commandBuffer = enqueuedCommandBuffers[ i ];
            }

            InplaceArray< VkSemaphoreSubmitInfo, 4 > waitSemaphores;
            waitSemaphores.add( { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                  nullptr,
                                  backbufferAcquiredSemaphore->GetHandle(),
                                  0,
                                  VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                                  0 } );

            if( shouldWaitForCompute )
                waitSemaphores.emplace( VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                        nullptr,
                                        m_computeSemaphore->GetHandle(),
                                        context.ComputeFrame,
                                        VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR,
                                        0 );

            if( shouldWaitForGraphics )
                waitSemaphores.emplace( VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                        nullptr,
                                        m_graphicsSemaphore->GetHandle(),
                                        waitForFrameIndex,
                                        VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR,
                                        0 );

            VkSemaphoreSubmitInfo signalSemaphores[]{ { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                                        nullptr,
                                                        renderCompleteSemaphore->GetHandle(),
                                                        0,
                                                        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                                                        0 },
                                                      { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                                        nullptr,
                                                        m_graphicsSemaphore->GetHandle(),
                                                        context.AbsoluteFrame + 1,
                                                        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                                                        0 } };

            VkSubmitInfo2 submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
            submitInfo.waitSemaphoreInfoCount = waitSemaphores.size();
            submitInfo.pWaitSemaphoreInfos = waitSemaphores.data();
            submitInfo.commandBufferInfoCount = commandBufferCount;
            submitInfo.pCommandBufferInfos = commandBufferInfo;
            submitInfo.signalSemaphoreInfoCount = 2;
            submitInfo.pSignalSemaphoreInfos = signalSemaphores;
            submitInfo.pNext = nullptr;

            std::lock_guard lock( m_graphicsMutex );
            VK_CHECK_RESULT( vkQueueSubmit2( m_device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE ) );
        } else {
            InplaceArray< VkSemaphore, 4 > waitSemaphores;
            InplaceArray< uint64_t, 4 > waitValues;
            InplaceArray< VkPipelineStageFlags, 4 > waitStages;

            waitSemaphores.add( backbufferAcquiredSemaphore->GetHandle() );
            waitValues.add( 0 );
            waitStages.add( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );

            if( shouldWaitForCompute ) {
                waitSemaphores.add( m_computeSemaphore->GetHandle() );
                waitValues.add( context.ComputeFrame );
                waitStages.add( VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
            }

            if( shouldWaitForGraphics ) {
                waitSemaphores.add( m_graphicsSemaphore->GetHandle() );
                waitValues.add( waitForFrameIndex );
                waitStages.add( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
            }

            InplaceArray< VkSemaphore, 2 > signalSemaphores{ renderCompleteSemaphore->GetHandle(),
                                                             m_graphicsSemaphore->GetHandle() };
            InplaceArray< uint64_t, 2 > signalValues{ 0, context.AbsoluteFrame + 1 };

            VkTimelineSemaphoreSubmitInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
            semaphoreInfo.signalSemaphoreValueCount = 2;
            semaphoreInfo.pSignalSemaphoreValues = signalValues.data();
            semaphoreInfo.waitSemaphoreValueCount = waitValues.size();
            semaphoreInfo.pWaitSemaphoreValues = waitValues.data();
            semaphoreInfo.pNext = nullptr;

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = waitSemaphores.size();
            submitInfo.pWaitSemaphores = waitSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStages.data();
            submitInfo.commandBufferCount = commandBufferCount;
            submitInfo.pCommandBuffers = enqueuedCommandBuffers.data();
            submitInfo.signalSemaphoreCount = 2;
            submitInfo.pSignalSemaphores = signalSemaphores.data();
            submitInfo.pNext = &semaphoreInfo;

            std::lock_guard lock( m_graphicsMutex );
            VK_CHECK_RESULT( vkQueueSubmit( m_device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE ) );
        }
    } else {
        //    const UniquePtr<Fence>& renderCompleteFence = m_SwapChain->GetRenderCompleteFence(context.FrameIndex);
        //
        //    if (IsSynchronization2Enabled())
        //    {
        //        InplaceArray<VkCommandBufferSubmitInfoKHR, 4> commandBufferInfo;
        //        for (uint8_t i = 0; i < commandBufferCount; ++i)
        //        {
        //            commandBufferInfo[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
        //            commandBufferInfo[i].commandBuffer = enqueuedCommandBuffers[i];
        //        }
        //
        //        InplaceArray<VkSemaphoreSubmitInfoKHR, 4> wait_semaphores;
        //        wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr,
        //        backbufferAcquiredSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0);
        //        wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr,
        //        m_ComputeSemaphore->GetHandle(), 0, VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR, 0);
        //
        //        //if (has_pending_sparse_bindings)
        //        //  wait_semaphores.Emplace(VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr,
        //        vulkan_bind_semaphore, 0, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR, 0);
        //
        //        InplaceArray<VkSemaphoreSubmitInfoKHR, 1> signalSemaphores
        //        {
        //            { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR, nullptr, renderCompleteSemaphore->GetHandle(), 0,
        //            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, 0 }
        //        };
        //
        //        VkSubmitInfo2 submitInfo;
        //        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        //        submitInfo.waitSemaphoreInfoCount = wait_semaphores.size();
        //        submitInfo.pWaitSemaphoreInfos = wait_semaphores.data();
        //        submitInfo.commandBufferInfoCount = commandBufferCount;
        //        submitInfo.pCommandBufferInfos = commandBufferInfo.data();
        //        submitInfo.signalSemaphoreInfoCount = 1;
        //        submitInfo.pSignalSemaphoreInfos = signalSemaphores.data();
        //        submitInfo.pNext = nullptr;
        //
        //        std::lock_guard lock(m_GraphicsMutex);
        //        VK_CHECK_RESULT(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submitInfo,
        //        renderCompleteFence->GetHandle()));
        //    }
        //    else
        //    {
        //        InplaceArray<VkSemaphore, 4> waitSemaphores;
        //        InplaceArray<VkPipelineStageFlags, 4> waitStages;
        //
        //        waitSemaphores.Add(backbufferAcquiredSemaphore->GetHandle());
        //        waitStages.Add(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        //
        //        waitSemaphores.Add(m_ComputeSemaphore->GetHandle());
        //        waitStages.Add(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
        //
        //        /*if (has_pending_sparse_bindings)
        //        {
        //            wait_semaphores.push(vulkan_bind_semaphore);
        //            wait_stages.push(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        //        }*/
        //
        //        VkSubmitInfo submitInfo;
        //        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        //        submitInfo.pWaitSemaphores = waitSemaphores.data();
        //        submitInfo.pWaitDstStageMask = waitStages.data();
        //        submitInfo.commandBufferCount = commandBufferCount;
        //        submitInfo.pCommandBuffers = enqueuedCommandBuffers.data();
        //        submitInfo.signalSemaphoreCount = 1;
        //        submitInfo.pSignalSemaphores = renderCompleteSemaphore->GetHandlePtr();
        //        submitInfo.pNext = nullptr;
        //
        //        std::lock_guard lock(m_GraphicsMutex);
        //        VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo,
        //        renderCompleteFence->GetHandle()));
        //    }
    }

    // submit compute buffers
    // m_HasComputeWork = false;
    // move to function
    if( computeCommandBufferCount != 0 ) {
        bool hasWaitSemaphore = context.ComputeFrame > 0;
        VkSemaphoreSubmitInfoKHR waitSemaphores[]{ { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
                                                     nullptr,
                                                     m_computeSemaphore->GetHandle(),
                                                     context.ComputeFrame,
                                                     VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                                                     0 } };

        VkSemaphoreSubmitInfoKHR signalSemaphores[]{
            { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
              nullptr,
              m_computeSemaphore->GetHandle(),
              context.ComputeFrame + 1,
              VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
              0 },
        };

        InplaceArray< VkCommandBufferSubmitInfoKHR, 4 > commandBufferInfo;
        for( uint8_t i = 0; i < computeCommandBufferCount; ++i ) {
            commandBufferInfo[ i ].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
            commandBufferInfo[ i ].commandBuffer = enqueuedComputeCommandBuffers[ i ];
        }

        VkSubmitInfo2 submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submitInfo.waitSemaphoreInfoCount = hasWaitSemaphore ? 1 : 0;
        submitInfo.pWaitSemaphoreInfos = waitSemaphores;
        submitInfo.commandBufferInfoCount = commandBufferInfo.size();
        submitInfo.pCommandBufferInfos = commandBufferInfo.data();
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = signalSemaphores;
        submitInfo.pNext = nullptr;

        vkQueueSubmit2( m_device->getComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE );
    }

    m_queuedCommandBuffer.clear();
    m_queuedComputeCommandBuffer.clear();

    // Delete resources
    for( auto it = m_deletionQueue.begin(); it != m_deletionQueue.end(); ) {
        if( it->operator()() ) {
            it = m_deletionQueue.erase( it );
        } else {
            ++it;
        }
    }

    return true;
}

Reference< rhi::Sampler > VulkanGraphicsApi::getSampler( SamplerProperties properties ) const {
    uint32_t hash = properties.Hash();
    if( m_samplers.contains( hash ) == false ) {
        //, "Unknown sampler");
        return {};
    }

    return m_samplers.at( hash );
}

std::lock_guard< std::mutex > VulkanGraphicsApi::lockGraphicsQueue() {
    return std::lock_guard( m_graphicsMutex );
}

void VulkanGraphicsApi::releaseTexture( const VulkanTexture& texture ) {
    ONYX_ASSERT( texture.getRefCount() == 0 );
    m_deletionQueue.emplace_back( [ textureIndex = texture.GetIndex(), this ]() mutable {
        m_textures.Release( textureIndex );
        return true;
    } );
}

void VulkanGraphicsApi::waitIdle() const {
    vkDeviceWaitIdle( m_device->GetHandle() );
    vkQueueWaitIdle( m_device->getGraphicsQueue() );
}

void VulkanGraphicsApi::createSwapchain( const platform::Window& window ) {
    m_surface = makeUnique< Surface >( *m_instance, window );

    // m_PhysicalDevice->RetrieveQueueFamilyIndices(*m_Surface);

    m_swapChain = makeUnique< SwapChain >( *this, *m_surface, window );
}

CommandBuffer& VulkanGraphicsApi::getCommandBuffer( uint8_t frameIndex ) {
    return getCommandBuffer( frameIndex, false );
}

CommandBuffer& VulkanGraphicsApi::getCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    CommandBuffer& commandBuffer = m_commandBufferManager->GetCommandBuffer( frameIndex, 0, shouldBegin );
    m_queuedCommandBuffer.add( &commandBuffer );
    return commandBuffer;
}

CommandBuffer& VulkanGraphicsApi::getComputeCommandBuffer( uint8_t frameIndex ) {
    return getCommandBuffer( frameIndex, false );
}

CommandBuffer& VulkanGraphicsApi::getComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    CommandBuffer& commandBuffer = m_computeCommandBufferManager->GetCommandBuffer( frameIndex, 0, shouldBegin );
    m_queuedComputeCommandBuffer.add( &commandBuffer );
    return commandBuffer;
}

void VulkanGraphicsApi::submitInstantCommandBuffer( Context context,
                                                    uint8_t frameIndex,
                                                    InplaceFunction< void( CommandBuffer& ) >&& functor ) {
    CommandBufferManager* commandBufferManager = nullptr;
    VkQueue queue = nullptr;
    VkFence fence = nullptr;
    switch( context ) {
    case Context::Graphics:
        commandBufferManager = m_commandBufferManager.get();
        queue = m_device->getGraphicsQueue();
        fence = m_graphicsSingleSubmitFence->GetHandle();
        break;
    case Context::Compute:
        commandBufferManager = m_computeCommandBufferManager.get();
        queue = m_device->getComputeQueue();
        fence = m_computeSingleSubmitFence->GetHandle();
        break;
    case Context::CopyTransfer:
        break;
    case Context::Count:
        ONYX_ASSERT( false, "Invalid queue type" );
        break;
    }

    if( queue == nullptr )
        return;

    UniquePtr< CommandBuffer > commandBuffer = commandBufferManager->CreateSingleSubmitCommandBuffer( *this,
                                                                                                      frameIndex,
                                                                                                      0 );
    VulkanCommandBuffer& vulkanCmdBuffer = static_cast< VulkanCommandBuffer& >( *commandBuffer );

    vulkanCmdBuffer.begin();
    functor( vulkanCmdBuffer );
    vulkanCmdBuffer.end();

    // Submit command buffer
    if( isSynchronization2Enabled() ) {
        VkCommandBufferSubmitInfoKHR commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
        commandBufferInfo.commandBuffer = vulkanCmdBuffer.GetHandle();
        commandBufferInfo.pNext = nullptr;

        VkSubmitInfo2KHR submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferInfo;
        submitInfo.pNext = nullptr;

        std::lock_guard lock( m_graphicsMutex );
        VK_CHECK_RESULT( vkQueueSubmit2( queue, 1, &submitInfo, fence ) )
    } else {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = vulkanCmdBuffer.GetHandlePtr();
        submitInfo.pNext = nullptr;

        VK_CHECK_RESULT( vkQueueSubmit( queue, 1, &submitInfo, fence ) )
    }

    vkWaitForFences( m_device->GetHandle(), 1, &fence, true, 9999999 );
    vkResetFences( m_device->GetHandle(), 1, &fence );

    if( context == Context::Graphics ) {
        std::lock_guard lock( m_graphicsMutex );
        vkQueueWaitIdle( queue );
    }
}

TextureHandle& VulkanGraphicsApi::getAcquiredSwapChainImage() {
    return m_swapChain->GetAcquiredBackbuffer();
}

const TextureHandle& VulkanGraphicsApi::getAcquiredSwapChainImage() const {
    return m_swapChain->GetAcquiredBackbuffer();
}

uint32_t VulkanGraphicsApi::getAcquiredBackbufferIndex() const {
    return m_swapChain->GetAcquiredBackbufferIndex();
}

TextureFormat VulkanGraphicsApi::getSwapchainTextureFormat() const {
    return m_swapChain->GetFormat();
}

const Vector2s32& VulkanGraphicsApi::getSwapchainExtent() const {
    return m_swapChain->GetExtent();
}

RenderPassHandle VulkanGraphicsApi::createRenderPass( const RenderPassSettings& settings ) {
    // TODO: Test if object pool + index based handle is faster / better
    /*RenderPassHandle handle;
    handle.Handle = m_RenderPasses.AcquireObject();

    if (handle)
    {
        VulkanRenderPass& renderPass = m_RenderPasses[handle];
        renderPass.Init(*m_Device, settings);
    }

    return handle;*/
    Reference< VulkanRenderPass > renderPass = Reference< VulkanRenderPass >::create();
    renderPass->Init( *this, settings );
    return renderPass;
}

FramebufferHandle VulkanGraphicsApi::createFramebuffer( const FramebufferSettings& settings ) {
    /*FramebufferHandle handle;
    handle.Handle = m_Framebuffers.AcquireObject();

    if (handle)
    {
        VulkanFramebuffer& framebuffer = m_Framebuffers[handle.Handle];
        framebuffer.Init(*this, settings);
    }

    return handle;*/

    Reference< VulkanFramebuffer > framebuffer = Reference< VulkanFramebuffer >::create();
    framebuffer->Init( *this, settings );
    return framebuffer;
}

void VulkanGraphicsApi::createTexture( TextureHandle& outTexture,
                                       const TextureStorageProperties& storageProperties,
                                       const TextureProperties& properties ) {
    TextureHandle handle;
    Reference< VulkanTextureStorage > storage = Reference< VulkanTextureStorage >::create( *this, storageProperties );

    handle.Storage = storage;
    createTextureView( handle, storage, properties );

    std::swap( outTexture.Storage, handle.Storage );
    std::swap( outTexture.Texture, handle.Texture );

    /*if (handle)
    {
        m_DeletionQueue.emplace_back([handleToDelete = handle, this]() mutable
            {
                if (handleToDelete.Texture && handleToDelete.Texture->GetRefCount() == 1 &&
    handleToDelete.Storage->GetRefCount() == 1)
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

void VulkanGraphicsApi::createTexture( TextureHandle& outTexture,
                                       const TextureStorageProperties& storageProperties,
                                       const TextureProperties& properties,
                                       const Span< uint8_t >& initialData ) {
    TextureHandle handle;
    Reference< VulkanTextureStorage > storage = Reference< VulkanTextureStorage >::create( *this,
                                                                                           storageProperties,
                                                                                           initialData );

    handle.Storage = storage;
    createTextureView( handle, storage, properties );

    std::swap( outTexture.Storage, handle.Storage );
    std::swap( outTexture.Texture, handle.Texture );

    /*if (handle)
    {
        m_DeletionQueue.emplace_back([handleToDelete = handle, this]() mutable
            {
                if (handleToDelete.Texture && handleToDelete.Texture->GetRefCount() == 1 &&
    handleToDelete.Storage->GetRefCount() == 1)
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

void VulkanGraphicsApi::createTextureView( TextureHandle& handle,
                                           const Reference< VulkanTextureStorage >& textureStorage,
                                           const TextureProperties& properties ) {
    uint32_t index;

    VulkanTexture* texture = m_textures.AcquireAndEmplace( index, *this, properties, textureStorage.raw() );
    texture->SetIndex( index );

    handle.Texture = texture;

    // ONYX_LOG_INFO("Allocated texture on index {} with name {}", index, properties.m_DebugName);
    m_bindlessTexturesToUpdate.push_back( { index, texture } );
}

void VulkanGraphicsApi::createAlias( TextureHandle& outTexture,
                                     TextureStorageHandle& storageHandle,
                                     const TextureStorageProperties& aliasStorageProperties,
                                     const TextureProperties& aliasTextureProperties ) {
    ONYX_ASSERT( storageHandle, "Storage handle is invalid" );

    VulkanTextureStorage& parentStorage = storageHandle.as< VulkanTextureStorage >();
    ONYX_ASSERT( parentStorage.GetHandle() != nullptr, "Storage for texture is not valid" );

    outTexture.Storage = storageHandle;
    outTexture.Alias = parentStorage.Alias( aliasStorageProperties );

    uint32_t index;
    VulkanTexture* texture = m_textures.AcquireAndEmplace( index,
                                                           *this,
                                                           aliasTextureProperties,
                                                           &parentStorage,
                                                           outTexture.Alias );
    texture->SetIndex( index );

    outTexture.Texture = texture;
    m_bindlessTexturesToUpdate.push_back( { index, texture } );
}

void VulkanGraphicsApi::createBuffer( BufferHandle& outBuffer, const BufferProperties& properties ) {
    outBuffer.Buffer = Reference< VulkanBuffer >::create( *this, properties );
}

BufferHandle VulkanGraphicsApi::getTransientBuffer( uint8_t frameIndex, const BufferProperties& properties ) {
    BufferHandle& ringBuffer = m_ringBuffer[ frameIndex ];
    ONYX_ASSERT( m_currentRingBufferSize < ringBuffer.Buffer->GetProperties().m_Size );

    m_currentRingBufferSize += properties.m_Size;
    int8_t alias = ringBuffer.Buffer->Alias( properties );
    return { ringBuffer.Buffer, alias };
}

PipelineHandle VulkanGraphicsApi::createPipeline( ShaderHandle& shader, const PipelineProperties& properties ) {
    return Reference< Pipeline >::create( *this, properties, shader );
}

DynamicArray< DescriptorSetHandle > VulkanGraphicsApi::createDescriptorSet( const ShaderHandle& shader ) {
    DynamicArray< DescriptorSetHandle > sets;

    const Shader& vulkanShader = shader.as< Shader >();
    const InplaceArray< UniquePtr< DescriptorSetLayout >, MAX_DESCRIPTOR_SET_LAYOUTS >&
        descriptorSetLayouts = vulkanShader.getDescriptorSetLayouts();

    for( const UniquePtr< DescriptorSetLayout >& layout : descriptorSetLayouts ) {
#if ONYX_IS_RETAIL
        descriptorSets.emplace_back( Reference< DescriptorSet >::create( *m_Device, *m_DescriptorPool, *layout ) );
#else
        StringView shaderName = shader.getId().getPath();
        StringView debugName = format::format( "{}(set = {})", shaderName, layout->GetSet() );

        sets.emplace_back( Reference< DescriptorSet >::create( *m_device, *m_descriptorPool, *layout, debugName ) );
#endif
    }

    return sets;
}
} // namespace onyx::rhi::vulkan
