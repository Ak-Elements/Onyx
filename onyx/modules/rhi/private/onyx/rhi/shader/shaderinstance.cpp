
#include <onyx/rhi/shader/shaderinstance.h>

#include <onyx/rhi/descriptorset.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shader.h>

#include <utility>

namespace onyx::rhi {
ShaderInstance::ShaderInstance( const GraphicsSystem& api, PipelineHandle pipeline, ShaderHandle shader )
    : m_api( &api )
    , m_shader( std::move( shader ) )
    , m_pipeline( std::move( pipeline ) ) {
    m_shader->getOnLoadedEvent().connect< &ShaderInstance::onShaderLoaded >( *this );
    if( m_shader->isLoaded() ) {
        onShaderLoaded( m_shader );
    }
}

ShaderInstance::~ShaderInstance() {
    if( m_shader.isValid() ) {
        m_shader->getOnLoadedEvent().disconnect( this );
    }
}

DynamicArray< DescriptorSetHandle >& ShaderInstance::getDescriptorSets( uint8_t frameIndex ) {
    ONYX_ASSERT( frameIndex < m_descriptorSets.size(),
                 "Frame index ({}) out of descriptor set bounds ({})",
                 frameIndex,
                 m_descriptorSets.size() );
    return m_descriptorSets[ frameIndex ];
}

const DynamicArray< DescriptorSetHandle >& ShaderInstance::getDescriptorSets( uint8_t frameIndex ) const {
    ONYX_ASSERT( frameIndex < m_descriptorSets.size(),
                 "Frame index ({}) out of descriptor set bounds ({})",
                 frameIndex,
                 m_descriptorSets.size() );
    return m_descriptorSets[ frameIndex ];
}

bool ShaderInstance::isCompute() const {
    return m_shader.isValid() && m_shader->isComputeShader();
}

bool ShaderInstance::isValid() const {
    return m_shader.isValid() && m_shader->isLoaded();
}

void ShaderInstance::bind( const TextureHandle& texture, const String& bindingName, uint8_t frameIndex ) {
    const uint8_t descriptorSetIndex = m_bindingIdToDescriptorSet[ bindingName ];
    getDescriptorSet( frameIndex, descriptorSetIndex )->Bind( texture, bindingName );
}

void ShaderInstance::bind( const BufferHandle& buffer, const String& bindingName, uint8_t frameIndex ) {
    const uint8_t descriptorSetIndex = m_bindingIdToDescriptorSet[ bindingName ];
    getDescriptorSet( frameIndex, descriptorSetIndex )->Bind( buffer, bindingName );
}

void ShaderInstance::preDraw( uint8_t frameIndex ) {
    if( m_descriptorSets.empty() )
        return;

    for( DescriptorSetHandle& descriptorSet : m_descriptorSets[ frameIndex ] ) {
        if( descriptorSet->HasPendingUpdates() ) {
            descriptorSet->UpdateDescriptors();
        }
    }
}

DescriptorSetHandle& ShaderInstance::getDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) {
    DynamicArray< DescriptorSetHandle >& descriptorSets = getDescriptorSets( frameIndex );
    ONYX_ASSERT( descriptorSetIndex < descriptorSets.size() );
    return descriptorSets[ descriptorSetIndex ];
}

const DescriptorSetHandle& ShaderInstance::getDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) const {
    const DynamicArray< DescriptorSetHandle >& descriptorSets = getDescriptorSets( frameIndex );
    ONYX_ASSERT( descriptorSetIndex < descriptorSets.size() );
    return descriptorSets[ descriptorSetIndex ];
}

void ShaderInstance::onShaderLoaded( assets::AssetHandle< Shader > /*shader*/ ) {
    if( m_shader->hasDescriptorSetLayout() ) {
        for( uint8_t frameIndex = 0; frameIndex < MaxFramesInFlight; ++frameIndex ) {
            m_descriptorSets[ frameIndex ] = m_api->createDescriptorSet( m_shader );
        }

        // bindings stay the same between frames so we can just peek at frame 0
        if( m_descriptorSets.empty() == false ) {
            const DynamicArray< DescriptorSetHandle >& descriptorSets = m_descriptorSets[ 0 ];
            const uint8_t descriptorSetsCount = numericCast< uint8_t >( descriptorSets.size() );
            for( uint8_t i = 0; i < descriptorSetsCount; ++i ) {
                const DescriptorSetHandle& descriptorSet = descriptorSets[ i ];
                HashSet< String > bindingIds = descriptorSet->GetBindingIds();
                for( const String& bindingId : bindingIds ) {
                    m_bindingIdToDescriptorSet[ bindingId ] = i;
                }
            }
        }
    }
}
} // namespace onyx::rhi
