#include <onyx/graphics/vulkan/pipeline.h>

#include <onyx/graphics/texture.h>
#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/pipelinelayout.h>
#include <onyx/graphics/vulkan/renderpass.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/graphics/vulkan/texturestorage.h>

namespace Onyx::Graphics::Vulkan
{
    namespace
    {
        VkVertexInputRate ToVulkanInputRate(VertexStreamInputRate rate)
        {
            switch (rate)
            {
                case VertexStreamInputRate::Vertex:
                    return VK_VERTEX_INPUT_RATE_VERTEX;
                case VertexStreamInputRate::Instance:
                    return VK_VERTEX_INPUT_RATE_INSTANCE;
                default:
                    ONYX_ASSERT(false, "Unsupported vertex stream input rate %s", Enums::ToString(rate).data());
                    return VK_VERTEX_INPUT_RATE_MAX_ENUM;
            }
        }

        VkPrimitiveTopology ToVulkanTopology(PrimitiveTopology topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Point:
                    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                case PrimitiveTopology::Line:
                    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                case PrimitiveTopology::LineStrip:
                    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
                case PrimitiveTopology::Triangle:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                case PrimitiveTopology::TriangleStrip:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
                case PrimitiveTopology::TriangleFan:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
                case PrimitiveTopology::Patch:
                    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
                default:
                    ONYX_ASSERT(false, "Unhandled primitive topology %s", Enums::ToString(topology).data());
                    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
            }
        }

        VkBlendFactor ToVulkanBlendFactor(Blend factor)
        {
            switch (factor)
            {
                case Blend::Zero:
                    return VK_BLEND_FACTOR_ZERO;
                case Blend::One: 
                    return VK_BLEND_FACTOR_ONE;
                case Blend::SrcColor:
                    return VK_BLEND_FACTOR_SRC_COLOR;
                case Blend::OneMinusSrcColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                case Blend::DstColor:
                    return VK_BLEND_FACTOR_DST_COLOR;
                case Blend::OneMinusDstColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                case Blend::SrcAlpha:
                    return VK_BLEND_FACTOR_SRC_ALPHA;
                case Blend::OneMinusSrcAlpha: 
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                case Blend::DstAlpha:
                    return VK_BLEND_FACTOR_DST_ALPHA;
                case Blend::OneMinusDstAlpha: 
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                case Blend::ConstantColor:
                    return VK_BLEND_FACTOR_CONSTANT_COLOR;
                case Blend::OneMinusConstantColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
                case Blend::ConstantAlpha:
                    return VK_BLEND_FACTOR_CONSTANT_ALPHA;
                case Blend::OneMinusConstantAlpha: 
                    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
                case Blend::SrcAlphaSaturate:
                    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
                case Blend::Src1Color:
                    return VK_BLEND_FACTOR_SRC1_COLOR;
                case Blend::OneMinusSrc1Color:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
                case Blend::Src1Alpha: 
                    return VK_BLEND_FACTOR_SRC1_ALPHA;
                case Blend::OneMinusSrc1Alpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
            }

            ONYX_ASSERT(false, "Unhandled blend factor %s", Enums::ToString(factor).data());
            return VK_BLEND_FACTOR_MAX_ENUM;
        }

        VkBlendOp ToVulkanBlendOpration(BlendOperation operation)
        {
            switch (operation)
            {
                case BlendOperation::Add:
                    return VK_BLEND_OP_ADD;
                case BlendOperation::Subtract:
                    return VK_BLEND_OP_SUBTRACT;
                case BlendOperation::ReverseSubtract:
                    return VK_BLEND_OP_REVERSE_SUBTRACT;
                case BlendOperation::Min:
                    return VK_BLEND_OP_MIN;
                case BlendOperation::Max:
                    return VK_BLEND_OP_MAX;
            }

            ONYX_ASSERT(false, "Unhandeld blend operation %s", Enums::ToString(operation).data());
            return VK_BLEND_OP_MAX_ENUM;
        }

        VkCompareOp ToVulkanCompareOp(CompareOperation operation)
        {
            switch (operation)
            {
                case CompareOperation::Never:
                    return VK_COMPARE_OP_NEVER;
                case CompareOperation::Less:
                    return VK_COMPARE_OP_LESS;
                case CompareOperation::Equal: 
                    return VK_COMPARE_OP_EQUAL;
                case CompareOperation::LessOrEqual: 
                    return VK_COMPARE_OP_LESS_OR_EQUAL;
                case CompareOperation::Greater: 
                    return VK_COMPARE_OP_GREATER;
                case CompareOperation::NotEqual: 
                    return VK_COMPARE_OP_NOT_EQUAL;
                case CompareOperation::GreaterOrEqual: 
                    return VK_COMPARE_OP_GREATER_OR_EQUAL;
                case CompareOperation::Always: 
                    return VK_COMPARE_OP_ALWAYS;
                default: 
                    ONYX_ASSERT(false, "Unhandeld compare operation %s", Enums::ToString(operation).data());
                    return VK_COMPARE_OP_MAX_ENUM;
            }
        }

        VkStencilOp ToVkStencilOperation(StencilOperation operation)
        {
            switch (operation)
            {
            case StencilOperation::Keep: 
                return VK_STENCIL_OP_KEEP;
            case StencilOperation::Zero: 
                return VK_STENCIL_OP_ZERO;
            case StencilOperation::Replace: 
                return VK_STENCIL_OP_REPLACE;
            case StencilOperation::IncrementClamp: 
                return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case StencilOperation::DecrementClamp: 
                return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            case StencilOperation::Invert: 
                return VK_STENCIL_OP_INVERT;
            case StencilOperation::IncrementWrap: 
                return VK_STENCIL_OP_INCREMENT_AND_WRAP;
            case StencilOperation::DecrementWrap: 
                return VK_STENCIL_OP_DECREMENT_AND_WRAP;
            default:
                ONYX_ASSERT(false, "Unhandeld stencil operation %s", Enums::ToString(operation).data());
                return VK_STENCIL_OP_MAX_ENUM;
            
            }
        }

        VkStencilOpState ToVulkanStencilOpState(const StencilOperationState& stencilOperation)
        {
            VkStencilOpState state;
            state.failOp = ToVkStencilOperation(stencilOperation.Fail);
            state.passOp = ToVkStencilOperation(stencilOperation.Pass);
            state.depthFailOp = ToVkStencilOperation(stencilOperation.DepthFail);
            state.compareOp = ToVulkanCompareOp(stencilOperation.Compare);
            state.compareMask = stencilOperation.CompareMask;
            state.writeMask = stencilOperation.WriteMask;
            state.reference = stencilOperation.Reference;
            return state;
        }

        VkCullModeFlags ToVulkanCullMode(CullMode mode)
        {
            switch (mode)
            {
                case CullMode::None:
                    return VK_CULL_MODE_NONE;
                case CullMode::Front:
                    return VK_CULL_MODE_FRONT_BIT;
                case CullMode::Back: 
                    return VK_CULL_MODE_BACK_BIT;
            }

            if (Enums::HasAllFlags(mode, CullMode::Front | CullMode::Back))
                return VK_CULL_MODE_FRONT_AND_BACK;

            ONYX_ASSERT(false, "Unhandeled cull mode {}", Enums::ToString(mode).data());
            return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        }

        VkPolygonMode ToVulkanFillMode(FillMode mode)
        {
            switch (mode)
            {
                case FillMode::Wireframe: 
                    return VK_POLYGON_MODE_LINE;
                case FillMode::Solid: 
                    return VK_POLYGON_MODE_FILL;
                case FillMode::Point: 
                    return VK_POLYGON_MODE_POINT;
                default: 
                    ONYX_ASSERT(false, "Unhandeled fill mode {}", Enums::ToString(mode).data());
                    return VK_POLYGON_MODE_MAX_ENUM;
            }
        }
    }

    Pipeline::Pipeline(const VulkanGraphicsApi& api, const PipelineProperties& properties)
        : Graphics::Pipeline(properties)
        , m_Device(api.GetDevice())
    {
        const Shader& shader = properties.Shader.As<Shader>();

        m_PipelineLayout = MakeUnique<PipelineLayout>(api, shader);
        CreatePipeline(api, properties, shader);
        SetResourceName(api.GetDevice().GetHandle(), VK_OBJECT_TYPE_PIPELINE, (onyxU64)m_Pipeline, properties.m_DebugName.empty() ? "Unnamed Pipeline" : properties.m_DebugName.c_str());
    }

    Pipeline::~Pipeline()
    {
        if (m_Pipeline != nullptr)
        {
            vkDestroyPipeline(m_Device.GetHandle(), m_Pipeline, nullptr);
            m_Pipeline = nullptr;
        }

        m_PipelineLayout.reset();
    }

    void Pipeline::CreatePipeline(const VulkanGraphicsApi& api, const PipelineProperties& properties, const Shader& shader)
    {
        const DynamicArray<VkPipelineShaderStageCreateInfo>& pipelineStageCreateInfos = shader.GetPipelineShaderStageCreateInfos();

        VkPipelineCache pipelineCache = nullptr;

        if (shader.IsComputeShader())
        {
            VkComputePipelineCreateInfo pipelineCreateInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };

            //// Shader stages
            pipelineCreateInfo.stage = pipelineStageCreateInfos[0];
            pipelineCreateInfo.layout = m_PipelineLayout->GetHandle();

            VK_CHECK_RESULT(vkCreateComputePipelines(m_Device.GetHandle(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline))

            m_BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
        }
        else
        {
            const RenderPassSettings& renderPassSettings = properties.RenderPass->GetSettings();

            DynamicArray<VkFormat> colorTargetFormats;
            VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;

            for (const RenderPassSettings::Attachment& attachment : renderPassSettings.m_Attachments)
            {
                TextureFormat format = Enums::ToEnum<TextureFormat>(attachment.m_Format);
                if (Utils::IsDepthFormat(format))
                {
                    ONYX_ASSERT(depthStencilFormat == VK_FORMAT_UNDEFINED);
                    depthStencilFormat = VulkanTextureStorage::GetFormat(format);
                }
                else
                {
                    colorTargetFormats.push_back(VulkanTextureStorage::GetFormat(format));
                }
            }

            const onyxU32 colorTargetsCount = static_cast<onyxU32>(colorTargetFormats.size());

            VkGraphicsPipelineCreateInfo pipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

            //// Shader stages
            pipelineCreateInfo.stageCount = static_cast<onyxU32>(pipelineStageCreateInfos.size());
            pipelineCreateInfo.pStages = pipelineStageCreateInfos.data();

            //// PipelineLayout
            pipelineCreateInfo.layout = m_PipelineLayout->GetHandle();

            //// Vertex layout
            const Set<VertexInput>& vertexInputs = shader.GetVertexInputs();

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
           
            const onyxU32 vertexAttributesCount = static_cast<onyxU32>(vertexInputs.size());
            // TODO: Support more than 1 stream

            DynamicArray<VkVertexInputBindingDescription> vertexBindingDescriptions;
            DynamicArray<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

            if (vertexAttributesCount != 0)
            {
                vertexAttributeDescriptions.reserve(vertexAttributesCount);

                onyxU32 stride = 0;
                onyxU32 offset = 0;
                for (const VertexInput& input : vertexInputs)
                {
                    vertexAttributeDescriptions.emplace_back(input.Location, 0, VulkanTextureStorage::GetFormat(input.Format), offset);

                    onyxU32 size = GetShaderTypeByteSize(input.Format);
                    offset += size;
                    stride += size;
                }

                vertexBindingDescriptions.emplace_back(0, stride, VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX);
            }
            
            vertexInputInfo.pVertexBindingDescriptions = static_cast<onyxU32>(vertexBindingDescriptions.size()) == 0 ? nullptr : vertexBindingDescriptions.data();
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<onyxU32>(vertexBindingDescriptions.size());

            vertexInputInfo.pVertexAttributeDescriptions = vertexAttributesCount == 0 ? nullptr : vertexAttributeDescriptions.data();
            vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributesCount;

            pipelineCreateInfo.pVertexInputState = &vertexInputInfo;

            //// Input Assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
            inputAssembly.topology = ToVulkanTopology(properties.Topology);
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

            //// Color Blending
            InplaceArray<VkPipelineColorBlendAttachmentState, 8> colorBlendAttachment;

            const InplaceArray<BlendState, MAX_RENDERPASS_ATTACHMENTS>& blendStates = properties.BlendStates;
            if (blendStates.empty() == false)
            {
                ONYX_ASSERT(blendStates.size() == colorTargetsCount, "Blend states (count: {}) mismatch with output targets (count {})!If blend states are active, they must be defined for all outputs", properties.BlendStates.size(), colorTargetsCount);

                for (onyxU8 i = 0; i < blendStates.size(); i++)
                {
                    const BlendState& blend_state = blendStates[i];

                    colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                    colorBlendAttachment[i].blendEnable = blend_state.IsBlendEnabled ? VK_TRUE : VK_FALSE;
                    colorBlendAttachment[i].srcColorBlendFactor = ToVulkanBlendFactor(blend_state.SourceColor);
                    colorBlendAttachment[i].dstColorBlendFactor = ToVulkanBlendFactor(blend_state.DestinationColor);
                    colorBlendAttachment[i].colorBlendOp = ToVulkanBlendOpration(blend_state.ColorOperation);

                    if ((blend_state.SourceAlpha != Blend::Invalid) || (blend_state.DestinationAlpha != Blend::Invalid))
                    {
                        colorBlendAttachment[i].srcAlphaBlendFactor = ToVulkanBlendFactor(blend_state.SourceAlpha);
                        colorBlendAttachment[i].dstAlphaBlendFactor = ToVulkanBlendFactor(blend_state.DestinationAlpha);
                        colorBlendAttachment[i].alphaBlendOp = ToVulkanBlendOpration(blend_state.AlphaOperation);
                    }
                    else
                    {
                        colorBlendAttachment[i].srcAlphaBlendFactor = ToVulkanBlendFactor(blend_state.SourceColor);
                        colorBlendAttachment[i].dstAlphaBlendFactor = ToVulkanBlendFactor(blend_state.DestinationColor);
                        colorBlendAttachment[i].alphaBlendOp = ToVulkanBlendOpration(blend_state.ColorOperation);
                    }
                }
            }
            else
            {
                // Default non blended state
                for (onyxU8 i = 0; i < colorTargetsCount; ++i)
                {
                    colorBlendAttachment[i] = {};
                    colorBlendAttachment[i].blendEnable = VK_FALSE;
                    colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                }
            }

            VkPipelineColorBlendStateCreateInfo colorBlending{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = colorBlendAttachment.size();
            colorBlending.pAttachments = colorBlendAttachment.data();
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            pipelineCreateInfo.pColorBlendState = &colorBlending;

            //// Depth Stencil
            VkPipelineDepthStencilStateCreateInfo depthStencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

            depthStencil.depthTestEnable = properties.DepthStencil.IsDepthEnabled ? VK_TRUE : VK_FALSE;
            depthStencil.depthCompareOp = ToVulkanCompareOp(properties.DepthStencil.Compare);
            depthStencil.stencilTestEnable = properties.DepthStencil.IsStencilEnabled ? VK_TRUE : VK_FALSE;
            depthStencil.depthWriteEnable = properties.DepthStencil.IsDepthWriteEnabled ? VK_TRUE : VK_FALSE;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f;
            depthStencil.maxDepthBounds = 1.0f;

            if (properties.DepthStencil.IsStencilEnabled)
            {
                depthStencil.front = ToVulkanStencilOpState(properties.DepthStencil.Front);
                depthStencil.back = ToVulkanStencilOpState(properties.DepthStencil.Back);
            }
            {
                //depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                //depthStencilState.depthTestEnable = VK_FALSE;
                //depthStencilState.depthWriteEnable = VK_FALSE;
                //depthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
                depthStencil.back.failOp = VK_STENCIL_OP_KEEP;
                depthStencil.back.passOp = VK_STENCIL_OP_KEEP;
                depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
                depthStencil.front = depthStencil.back;
            }

            pipelineCreateInfo.pDepthStencilState = &depthStencil;

            //// Multisample
            VkPipelineMultisampleStateCreateInfo multisampling{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            pipelineCreateInfo.pMultisampleState = &multisampling;

            //// Rasterizer
            VkPipelineRasterizationStateCreateInfo rasterizer{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = ToVulkanFillMode(properties.Rasterization.FillMode);
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = ToVulkanCullMode(properties.Rasterization.CullMode);
            rasterizer.frontFace = properties.Rasterization.IsFrontFacing ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

            pipelineCreateInfo.pRasterizationState = &rasterizer;

            //// Tessellation
            //pipelineCreateInfo.pTessellationState;


            //// Viewport state
            VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
            viewport_state.viewportCount = 1;
            viewport_state.scissorCount = 1;

            pipelineCreateInfo.pViewportState = &viewport_state;

            //// Render Pass
            VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR };
            if (api.IsDynamicRenderingEnabled())
            {
                pipelineRenderingCreateInfo.viewMask = 0;

                if (colorTargetsCount != 0)
                {

                    pipelineRenderingCreateInfo.colorAttachmentCount = colorTargetsCount;
                    pipelineRenderingCreateInfo.pColorAttachmentFormats = colorTargetFormats.data();
                }
                else
                {
                    pipelineRenderingCreateInfo.colorAttachmentCount = 0;
                    pipelineRenderingCreateInfo.pColorAttachmentFormats = nullptr;
                }

                pipelineRenderingCreateInfo.depthAttachmentFormat = depthStencilFormat;
                pipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

                pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
            }
            else
            {
                const VulkanRenderPass& renderPass = properties.RenderPass.As<VulkanRenderPass>();
                pipelineCreateInfo.renderPass = renderPass.GetHandle();
            }

            //// Dynamic states
            InplaceArray<VkDynamicState, 3> dynamicStates({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });

            VkPipelineDynamicStateCreateInfo dynamicStatesInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
            dynamicStatesInfo.dynamicStateCount = dynamicStates.size();
            dynamicStatesInfo.pDynamicStates = dynamicStates.data();

            pipelineCreateInfo.pDynamicState = &dynamicStatesInfo;

            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            VK_CHECK_RESULT(vkCreatePipelineCache(m_Device.GetHandle(), &pipelineCacheCreateInfo, nullptr, &pipelineCache));

            VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device.GetHandle(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline))
            
            m_BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            vkDestroyPipelineCache(m_Device.GetHandle(), pipelineCache, nullptr);
        }
    }
}
