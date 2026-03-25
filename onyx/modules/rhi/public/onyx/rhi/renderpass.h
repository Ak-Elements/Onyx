#pragma once

#include <onyx/rhi/graphicstypes.h>

namespace onyx::rhi {
// TODO: Move out of render pass maybe? LoadOp etc.
// Simplify?
struct RenderPassSettings {
    enum class AttachmentAccess : uint8_t {
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

    enum class LoadOp : uint8_t { Load, Clear, DontCare };

    struct Attachment {
        uint16_t m_Format : 8;
        uint16_t m_Samples : 4;
        uint16_t m_LoadOp : 2;
        uint16_t m_StoreOp : 2;
        uint8_t m_ClearColor[ 4 ];

        bool operator==( const Attachment& other ) const {
            return std::memcmp( this, &other, sizeof( Attachment ) ) == 0;
        }
    };

    struct Subpass {
        InplaceArray< AttachmentAccess, MAX_RENDERPASS_ATTACHMENTS > m_AttachmentAccesses;
    };

    InplaceArray< Attachment, MAX_RENDERPASS_ATTACHMENTS > m_Attachments;
    InplaceArray< Subpass, MAX_SUBPASSES > m_SubPasses;

    // TODO: this is not a proper compare I think as the content of the attachments are not compared
    bool operator==( const RenderPassSettings& other ) const {
        return std::memcmp( this, &other, sizeof( RenderPassSettings ) );
    }
};

class RenderPass : public RefCounted {
  public:
    RenderPass() = default;
    RenderPass( const RenderPassSettings& settings )
        : m_Settings( settings ) {}
    virtual ~RenderPass() = default;

    virtual RenderPassSettings& GetSettings() { return m_Settings; };
    virtual const RenderPassSettings& GetSettings() const { return m_Settings; };

  protected:
    RenderPassSettings m_Settings;
};
} // namespace onyx::rhi
