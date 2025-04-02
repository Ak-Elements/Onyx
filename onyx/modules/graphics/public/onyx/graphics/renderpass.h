#pragma once

#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
    // TODO: Move out of render pass maybe? LoadOp etc.
    // Simplify?
    struct RenderPassSettings
    {
        enum class AttachmentAccess : onyxU8
        {
            Unused,
            Input,
            RenderTarget,
            ResolveTarget,
            DepthWriteStencilWrite,
            DepthReadStencilWrite,
            DepthWriteStencilRead,
            DepthReadStencilRead,
            Preserve,
            Present,
            ShadingRate // extension
        };

        enum class LoadOp : onyxU8
        {
            Load,
            Clear,
            DontCare
        };

        struct Attachment
        {
            onyxU16 m_Format : 8;
            onyxU16 m_Samples : 4;
            onyxU16 m_LoadOp : 2;
            onyxU16 m_StoreOp : 2;
            onyxU8 m_ClearColor[4];

            bool operator==(const Attachment& other) const { return std::memcmp(this, &other, sizeof(Attachment) == 0); }
        };

        struct Subpass
        {
            InplaceArray<AttachmentAccess, MAX_RENDERPASS_ATTACHMENTS> m_AttachmentAccesses;
        };

        InplaceArray<Attachment, MAX_RENDERPASS_ATTACHMENTS> m_Attachments;
        InplaceArray<Subpass, MAX_SUBPASSES> m_SubPasses;

        String m_DebugName;

        // TODO: this is not a proper compare I think as the content of the attachments are not compared
        bool operator==(const RenderPassSettings& other) const { return std::memcmp(this, &other, sizeof(RenderPassSettings)); }
    };

	class RenderPass : public RefCounted
	{
	public:
        RenderPass() = default;
        RenderPass(const RenderPassSettings& settings)
	        : m_Settings(settings)
        {
        }
		virtual ~RenderPass() = default;

        virtual RenderPassSettings& GetSettings() { return m_Settings; };
        virtual const RenderPassSettings& GetSettings() const { return m_Settings; };

	protected:
        RenderPassSettings m_Settings;
	};
}
