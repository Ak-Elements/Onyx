#include <onyx/graphics/vulkan/renderpass.h>

#include <onyx/graphics/vulkan/swapchain.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/texture.h>
#include <onyx/graphics/vulkan/texturestorage.h>

namespace
{
    VkImageLayout GetLayoutFromAttachmentRole(Onyx::Graphics::RenderPassSettings::AttachmentAccess access)
    {
        using namespace Onyx::Graphics;
        switch (access)
        {
        case RenderPassSettings::AttachmentAccess::Input:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::RenderTarget:
        case RenderPassSettings::AttachmentAccess::ResolveTarget:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::ShadingRate:
            return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
        case RenderPassSettings::AttachmentAccess::DepthWriteStencilWrite:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::DepthReadStencilWrite:
            return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::DepthWriteStencilRead:
            return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::DepthReadStencilRead:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case RenderPassSettings::AttachmentAccess::Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    VkAttachmentDescription2 CreateAttachmentDescription(const Onyx::Graphics::RenderPassSettings::Attachment& attachmentSettings, VkImageLayout initialLayout, VkImageLayout finalLayout, bool& outHasLoadOp)
    {
        using namespace Onyx::Graphics;
        using namespace Onyx::Graphics::Vulkan;
        TextureFormat format = static_cast<TextureFormat>(attachmentSettings.m_Format);

        VkAttachmentDescription2 attachmentDescription = {};
        attachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        attachmentDescription.format = VulkanTextureStorage::GetFormat(format);
        attachmentDescription.samples = static_cast<VkSampleCountFlagBits>(attachmentSettings.m_Samples);
        attachmentDescription.loadOp = static_cast<VkAttachmentLoadOp>(attachmentSettings.m_LoadOp);
        attachmentDescription.storeOp = static_cast<VkAttachmentStoreOp>(attachmentSettings.m_StoreOp);

        outHasLoadOp |= attachmentDescription.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD;

        if (Utils::HasStencil(format))
        {
            attachmentDescription.stencilLoadOp = static_cast<VkAttachmentLoadOp>(attachmentSettings.m_LoadOp);
            attachmentDescription.stencilStoreOp = static_cast<VkAttachmentStoreOp>(attachmentSettings.m_StoreOp);
        }
        else
        {
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachmentDescription.initialLayout = initialLayout;
        attachmentDescription.finalLayout = finalLayout;

        return attachmentDescription;
    }
}

namespace Onyx::Graphics::Vulkan
{
    void VulkanRenderPass::Init(const VulkanGraphicsApi& api, const RenderPassSettings& settings)
    {
        m_Device = &api.GetDevice();
        m_Settings = settings;

        // if we use dynamic rendering we don't create a vkRenderPass
        if (api.IsDynamicRenderingEnabled())
            return;

        // TODO: Implement legacy renderpass creation?
        if (api.IsRenderPass2ExtensionEnabled())
            CreateRenderPass();
        //else
        //    CreateLegacyRenderPass();
    }

    void VulkanRenderPass::CreateRenderPass()
    {
        const onyxU8 attachmentCount = static_cast<onyxU8>(m_Settings.m_Attachments.size());
        ONYX_ASSERT(attachmentCount <= MAX_RENDERPASS_ATTACHMENTS);

        DynamicArray<VkAttachmentDescription2> attachmentDescriptions;
        InplaceArray<VkImageLayout, MAX_RENDERPASS_ATTACHMENTS> initialLayouts;
        InplaceArray<VkImageLayout, MAX_RENDERPASS_ATTACHMENTS> finalLayouts;

        bool hasLoadOp = false;

        attachmentDescriptions.reserve(attachmentCount);

        VkImageLayout layout;
        for (onyxU8 i = 0; i < attachmentCount; ++i)
        {
            // Iterate over subpasses to determine attachment layouts
            for (const RenderPassSettings::Subpass& subPassSettings : m_Settings.m_SubPasses)
            {
                layout = GetLayoutFromAttachmentRole(subPassSettings.m_AttachmentAccesses[i]);
                if (layout == VK_IMAGE_LAYOUT_UNDEFINED)
                    continue;

                finalLayouts[i] = layout;

                if (initialLayouts[i] == VK_IMAGE_LAYOUT_UNDEFINED)
                    initialLayouts[i] = layout;

            }
        }

        for (onyxU8 i = 0; i < attachmentCount; ++i)
        {
            attachmentDescriptions.push_back(CreateAttachmentDescription(m_Settings.m_Attachments[i], initialLayouts[i], finalLayouts[i], hasLoadOp));
        }

        InplaceArray<VkSubpassDescription2, MAX_SUBPASSES> subPassDescriptions;

        const onyxU8 subPassCount = m_Settings.m_SubPasses.size();
        for (onyxU8 subPassIndex = 0; subPassIndex < subPassCount; ++subPassIndex)
        {
            bool hasDepthAttachment = false;
            bool hasResolveAttachment = false;
            //bool hasShadingRateAttachment = false;
            //onyxU32 inputAttachmentCount = 0;
            onyxU8 renderTargetCount = 0;
            //onyxU32 preserveAttachmentCount = 0;

            DynamicArray<VkAttachmentReference2> colorAttachments;
            DynamicArray<VkAttachmentReference2> resolveAttachments;
            DynamicArray<VkAttachmentReference2> inputAttachments;

            VkAttachmentReference2 depthStencilAttachment { VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 };
            depthStencilAttachment.pNext = nullptr;
            depthStencilAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

            const RenderPassSettings::Subpass& subpass = m_Settings.m_SubPasses[subPassIndex];
            for (onyxU8 attachmentIndex = 0; attachmentIndex < attachmentCount; ++attachmentIndex)
            {
                if (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::Unused)
                    continue;

                if (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::RenderTarget)
                {
                    VkAttachmentReference2& ref2 = colorAttachments.emplace_back();
                    ref2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                    ref2.attachment = attachmentIndex;
                    ref2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    ref2.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    ref2.pNext = nullptr;

                    ++renderTargetCount;
                }
                else if (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::ResolveTarget)
                {
                    VkAttachmentReference2& ref2 = resolveAttachments.emplace_back();
                    ref2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                    ref2.attachment = attachmentIndex;
                    ref2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    ref2.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    ref2.pNext = nullptr;

                    hasResolveAttachment = true;
                }
                else if (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::Preserve)
                {
                    // TODO: Implement
                }
                else if ((subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::Input) ||
                        (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthReadStencilWrite) ||
                        (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthWriteStencilRead) ||
                        (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthReadStencilRead))
                {
                    VkAttachmentReference2& ref2 = inputAttachments.emplace_back();
                    ref2.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                    ref2.attachment = attachmentIndex;
                    ref2.pNext = nullptr;

                    if (Utils::IsDepthFormat(static_cast<TextureFormat>(m_Settings.m_Attachments[attachmentIndex].m_Format)))
                    {
                        ref2.layout = GetLayoutFromAttachmentRole(subpass.m_AttachmentAccesses[attachmentIndex]);
                        ref2.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                    }
                    else
                    {
                        ref2.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        ref2.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    }
                }

                if ((subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthWriteStencilWrite) ||
                    (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthReadStencilWrite) ||
                    (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthWriteStencilRead) ||
                    (subpass.m_AttachmentAccesses[attachmentIndex] == RenderPassSettings::AttachmentAccess::DepthReadStencilRead))
                {
                    ONYX_ASSERT(hasDepthAttachment == false);

                    hasDepthAttachment = true;
                    depthStencilAttachment.layout = GetLayoutFromAttachmentRole(subpass.m_AttachmentAccesses[attachmentIndex]);
                    depthStencilAttachment.attachment = attachmentIndex;
                }

                //TODO: implement shading rate
            }

            VkSubpassDescription2& subpassDescription2 = subPassDescriptions[subPassIndex];
            subpassDescription2.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
            subpassDescription2.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescription2.inputAttachmentCount = static_cast<onyxU32>(inputAttachments.size());
            subpassDescription2.pInputAttachments = inputAttachments.empty() ? nullptr : inputAttachments.data();
            subpassDescription2.colorAttachmentCount = static_cast<onyxU32>(colorAttachments.size());
            subpassDescription2.pColorAttachments = colorAttachments.empty() ? nullptr : colorAttachments.data();
            subpassDescription2.pResolveAttachments = resolveAttachments.empty() ? nullptr : resolveAttachments.data();
            subpassDescription2.pDepthStencilAttachment = hasDepthAttachment ? &depthStencilAttachment : nullptr;
            subpassDescription2.preserveAttachmentCount = 0;
            subpassDescription2.pPreserveAttachments = nullptr; // preserve
            subpassDescription2.pNext = nullptr; // shading rate
        }

        DynamicArray<VkSubpassDependency2> subPassDependencies;
        subPassDependencies.reserve(MAX_RENDERPASS_ATTACHMENTS * MAX_SUBPASSES);

        VkAccessFlags vkAccessToClearWhenLoadingSubPass = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (hasLoadOp)
            vkAccessToClearWhenLoadingSubPass |= VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

        VkSubpassDependency2& dependencyExt = subPassDependencies.emplace_back();
        dependencyExt.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
        dependencyExt.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencyExt.dstSubpass = 0;
        dependencyExt.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencyExt.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencyExt.srcAccessMask = 0;
        dependencyExt.dstAccessMask = vkAccessToClearWhenLoadingSubPass;
        dependencyExt.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        const onyxU8 subPassDescriptionsCount = subPassDescriptions.size();
        for (onyxU8 i = 0; i < (subPassDescriptionsCount - 1); ++i)
        {
            VkSubpassDependency2& dependency = subPassDependencies.emplace_back();
            dependency.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
            dependency.srcSubpass = i;
            dependency.dstSubpass = i + 1;
            dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        }

        VkRenderPassCreateInfo2 createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 };
        createInfo.attachmentCount = static_cast<onyxU32>(attachmentDescriptions.size());
        createInfo.pAttachments = attachmentDescriptions.data();
        createInfo.dependencyCount = static_cast<onyxU32>(subPassDependencies.size());
        createInfo.pDependencies = subPassDependencies.data();
        createInfo.subpassCount = subPassDescriptionsCount;
        createInfo.pSubpasses = subPassDescriptions.data();

        VK_CHECK_RESULT(vkCreateRenderPass2(m_Device->GetHandle(), &createInfo, nullptr, &m_RenderPass));
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        if (m_RenderPass)
        {
            vkDestroyRenderPass(m_Device->GetHandle(), m_RenderPass, nullptr);
            m_RenderPass = nullptr;
        }
    }
}
