
#include <onyx/rhi/shader/shaderinstance.h>

#include <onyx/rhi/descriptorset.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shader.h>

namespace onyx::rhi {
ShaderInstance::ShaderInstance( const GraphicsSystem& api, const PipelineHandle& pipeline, const ShaderHandle& shader )
    : m_Api( &api )
    , m_Shader( shader )
    , m_Pipeline( pipeline ) {
    m_Shader->getOnLoadedEvent().Connect< &ShaderInstance::OnShaderLoaded >( *this );
    if( m_Shader->isLoaded() ) {
        OnShaderLoaded( m_Shader );
    }
}

ShaderInstance::~ShaderInstance() {
    if( m_Shader.isValid() ) {
        m_Shader->getOnLoadedEvent().Disconnect( this );
    }
}

DynamicArray< DescriptorSetHandle >& ShaderInstance::GetDescriptorSets( uint8_t frameIndex ) {
    ONYX_ASSERT( frameIndex < m_DescriptorSets.size(),
                 "Frame index ({}) out of descriptor set bounds ({})",
                 frameIndex,
                 m_DescriptorSets.size() );
    return m_DescriptorSets[ frameIndex ];
}

const DynamicArray< DescriptorSetHandle >& ShaderInstance::GetDescriptorSets( uint8_t frameIndex ) const {
    ONYX_ASSERT( frameIndex < m_DescriptorSets.size(),
                 "Frame index ({}) out of descriptor set bounds ({})",
                 frameIndex,
                 m_DescriptorSets.size() );
    return m_DescriptorSets[ frameIndex ];
}

bool ShaderInstance::IsCompute() const {
    return m_Shader.isValid() && m_Shader->isComputeShader();
}

bool ShaderInstance::IsValid() const {
    return m_Shader.isValid() && m_Shader->isLoaded();
}

void ShaderInstance::Bind( const TextureHandle& texture, const String& bindingName, uint8_t frameIndex ) {
    const int8_t descriptorSetIndex = m_BindingIdToDescriptorSet[ bindingName ];
    GetDescriptorSet( frameIndex, descriptorSetIndex )->Bind( texture, bindingName );
}

void ShaderInstance::Bind( const BufferHandle& buffer, const String& bindingName, uint8_t frameIndex ) {
    const int8_t descriptorSetIndex = m_BindingIdToDescriptorSet[ bindingName ];
    GetDescriptorSet( frameIndex, descriptorSetIndex )->Bind( buffer, bindingName );
}

void ShaderInstance::PreDraw( uint8_t frameIndex ) {
    if( m_DescriptorSets.empty() )
        return;

    for( DescriptorSetHandle& descriptorSet : m_DescriptorSets[ frameIndex ] ) {
        if( descriptorSet->HasPendingUpdates() ) {
            descriptorSet->UpdateDescriptors();
        }
    }
}

DescriptorSetHandle& ShaderInstance::GetDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) {
    DynamicArray< DescriptorSetHandle >& descriptorSets = GetDescriptorSets( frameIndex );
    ONYX_ASSERT( descriptorSetIndex < descriptorSets.size() );
    return descriptorSets[ descriptorSetIndex ];
}

const DescriptorSetHandle& ShaderInstance::GetDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) const {
    const DynamicArray< DescriptorSetHandle >& descriptorSets = GetDescriptorSets( frameIndex );
    ONYX_ASSERT( descriptorSetIndex < descriptorSets.size() );
    return descriptorSets[ descriptorSetIndex ];
}

void ShaderInstance::OnShaderLoaded( assets::AssetHandle< Shader > /*shader*/ ) {
    if( m_Shader->hasDescriptorSetLayout() ) {
        for( uint8_t frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; ++frameIndex ) {
            m_DescriptorSets[ frameIndex ] = m_Api->createDescriptorSet( m_Shader );
        }

        // bindings stay the same between frames so we can just peek at frame 0
        if( m_DescriptorSets.empty() == false ) {
            const DynamicArray< DescriptorSetHandle >& descriptorSets = m_DescriptorSets[ 0 ];
            const uint8_t descriptorSetsCount = numericCast< uint8_t >( descriptorSets.size() );
            for( uint8_t i = 0; i < descriptorSetsCount; ++i ) {
                const DescriptorSetHandle& descriptorSet = descriptorSets[ i ];
                HashSet< String > bindingIds = descriptorSet->GetBindingIds();
                for( const String& bindingId : bindingIds ) {
                    m_BindingIdToDescriptorSet[ bindingId ] = i;
                }
            }
        }
    }
}
} // namespace onyx::rhi
