#include <onyx/rhi/shader/compiler/slang/slangcompiler.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shader.h>
#include <onyx/rhi/shader/shadercache.h>
#include <onyx/rhi/vulkan/shader.h>

// clang-format off
#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
// clang-format on

namespace onyx::rhi::shader_compiler::slang {

namespace {
ShaderStage toShaderStage( ::SlangStage stage ) {
    switch( stage ) {
    case SLANG_STAGE_NONE:
        return ShaderStage::Invalid;
    case SLANG_STAGE_VERTEX:
        return ShaderStage::Vertex;
    case SLANG_STAGE_HULL:
        return ShaderStage::Hull;
    case SLANG_STAGE_DOMAIN:
        return ShaderStage::Domain;
    case SLANG_STAGE_GEOMETRY:
        return ShaderStage::Geometry;
    case SLANG_STAGE_FRAGMENT:
        return ShaderStage::Fragment;
    case SLANG_STAGE_COMPUTE:
        return ShaderStage::Compute;
    case SLANG_STAGE_RAY_GENERATION:
        return ShaderStage::RayGeneration;
    case SLANG_STAGE_INTERSECTION:
        return ShaderStage::Intersection;
    case SLANG_STAGE_ANY_HIT:
        return ShaderStage::AnyHit;
    case SLANG_STAGE_CLOSEST_HIT:
        return ShaderStage::ClosestHit;
    case SLANG_STAGE_MISS:
        return ShaderStage::Miss;
    case SLANG_STAGE_CALLABLE:
        return ShaderStage::Callable;
    case SLANG_STAGE_MESH:
        return ShaderStage::Mesh;
    case SLANG_STAGE_AMPLIFICATION:
        return ShaderStage::Amplification;
    case SLANG_STAGE_DISPATCH:
        return ShaderStage::Dispatch;
    case SLANG_STAGE_NODE:
    case SLANG_STAGE_COUNT:
        return ShaderStage::Invalid;
        break;
    }
}

TextureFormat getFormat( ::slang::TypeReflection& type ) {
    const uint32_t attributesCount = type.getUserAttributeCount();
    for( uint32_t i = 0; i < attributesCount; ++i ) {
        ::slang::UserAttribute* attribute = type.getUserAttributeByIndex( i );
        attribute->getArgumentType( 0 );
        if( ignoreCaseEqual( attribute->getName(), "Format" ) ) {
            StringView::size_type length;
            const char* rawValue = attribute->getArgumentValueString( 0, &length );
            StringView value( rawValue, length );
            TextureFormat format = enums::fromString< TextureFormat >( value );
            return format;
        }
    }

    ::slang::TypeReflection::Kind kind = type.getKind();
    if( kind == ::slang::TypeReflection::Kind::Vector ) {
        ::slang::TypeReflection* elementType = type.getElementType();
        const uint32_t elementCount = type.getElementCount();
        switch( elementType->getScalarType() ) {
        case ::slang::TypeReflection::ScalarType::Float32: {
            switch( elementCount ) {
            case 2:
                return TextureFormat::RG_FLOAT32;
            case 3:
                return TextureFormat::RGB_FLOAT32;
            case 4:
                return TextureFormat::RGBA_FLOAT32;
            }
        }
        case ::slang::TypeReflection::ScalarType::UInt32: {
            // TODO: Implement
        }
        default:
            ONYX_ASSERT( false, "Unhandeled shader base type" );
            return TextureFormat::Invalid;
        }
    } else {
        switch( type.getScalarType() ) {
        case ::slang::TypeReflection::ScalarType::Bool:
            return TextureFormat::R_UINT8;
        case ::slang::TypeReflection::ScalarType::Float32: {
            return TextureFormat::R_FLOAT32;
        }
        case ::slang::TypeReflection::ScalarType::UInt32: {
            return TextureFormat::R_UINT32;
        }
        default:
            ONYX_ASSERT( false, "Unhandeled shader base type" );
            return TextureFormat::Invalid;
        }
    }
}

TextureFormat getFormat( ::slang::VariableLayoutReflection& variable ) {
    ::slang::VariableReflection* variableReflection = variable.getVariable();
    ONYX_ASSERT( variableReflection != nullptr );
    uint32_t attributesCount = variableReflection->getUserAttributeCount();

    for( uint32_t i = 0; i < attributesCount; ++i ) {
        ::slang::UserAttribute* attribute = variableReflection->getUserAttributeByIndex( i );
        if( ignoreCaseEqual( attribute->getName(), "format" ) ) {
            int32_t value;
            auto result = attribute->getArgumentValueInt( 0, &value );
            if( SLANG_FAILED( result ) ) {
                // TODO:: acually we should cancel the shader gen here instead of asserting
                ONYX_ASSERT( false, "Invalid texture format" );
                return TextureFormat::Invalid;
            }
            TextureFormat format = enums::toEnum< TextureFormat >( value );
            return format;
        }
    }

    return getFormat( *variable.getType() );
}

void reflectVertexStage( ::slang::VariableLayoutReflection& parameter,
                         ::slang::TypeLayoutReflection& parameterType,
                         ShaderReflectionInfo& outReflectionInfo ) {
    size_t location = parameter.getOffset( SLANG_PARAMETER_CATEGORY_VARYING_INPUT );
    if( location == SLANG_UNKNOWN_SIZE )
        return;

    if( parameterType.getKind() == ::slang::TypeReflection::Kind::Struct ) {
        const uint32_t fieldCount = parameterType.getFieldCount();
        for( uint32_t fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex ) {
            ::slang::VariableLayoutReflection* field = parameterType.getFieldByIndex( fieldIndex );
            ONYX_ASSERT( field != nullptr );

            location = field->getOffset( SLANG_PARAMETER_CATEGORY_VARYING_INPUT );

            TextureFormat format = getFormat( *field );
            outReflectionInfo.VertexInput.add( location, format );
        }
    } else {
        ::slang::TypeReflection* typeReflection = parameterType.getType();
        ONYX_ASSERT( typeReflection != nullptr );

        TextureFormat format = getFormat( *typeReflection );
        outReflectionInfo.VertexInput.add( location, format );
    }
}

void reflectPushConstant( ::slang::TypeLayoutReflection& pushConstantTypeLayout,
                          PushConstantRange& pushConstantRange ) {
    auto elementTypeLayout = pushConstantTypeLayout.getElementTypeLayout();
    ONYX_ASSERT( elementTypeLayout != nullptr );

    pushConstantRange.Size += elementTypeLayout->getSize();
}

Optional< ShaderReflectionInfo > reflect( ::slang::IComponentType& linkedShader ) {
    ShaderReflectionInfo reflectInfo;

    ::slang::ProgramLayout* programLayoutPtr = linkedShader.getLayout();
    ONYX_ASSERT( programLayoutPtr != nullptr );

    ::Slang::ComPtr< ::slang::IMetadata > targetMetadata;
    linkedShader.getTargetMetadata( 0, targetMetadata.writeRef() );

    ::slang::ProgramLayout& programLayout = *programLayoutPtr;

    auto bindlessMetadata = static_cast< ::slang::IBindlessResourceMetadata* >(
        targetMetadata->castAs( ::slang::IBindlessResourceMetadata::getTypeGuid() ) );

    const bool isBindless = bindlessMetadata && bindlessMetadata->usesBindlessResourceHeap();
    if( isBindless ) {
        reflectInfo.IsUsingBindless = true;

        ShaderDescriptorSet& descriptorSet = reflectInfo.ShaderDescriptorSets.emplace_back();
        descriptorSet.Set = 0;

        ImageSampler& sampler = descriptorSet.SeparateSamplers[ Bindless::SamplerBinding ];
        sampler.Stage = ShaderStage::All;
        sampler.Name = ""; // parameter->getName();
        sampler.BindingPoint = 0;
        sampler.DescriptorSet = 0;

        ImageSampler& imageSampler = descriptorSet.ImageSamplers[ Bindless::CombinedImageBinding ];
        imageSampler.Stage = ShaderStage::All;
        imageSampler.Name = ""; // parameter->getName();
        imageSampler.BindingPoint = 0;
        imageSampler.DescriptorSet = 0;

        ImageSampler& textures = descriptorSet.SeparateTextures[ Bindless::SampledImageBinding ];
        textures.Stage = ShaderStage::All;
        textures.Name = ""; // parameter->getName();
        textures.BindingPoint = 0;
        textures.DescriptorSet = 0;
    }

    // global scope
    PushConstantRange globalPushConstants{ ShaderStage::All };
    const uint32_t globalParametersCount = programLayout.getParameterCount();
    for( uint32_t parameterIndex = 0; parameterIndex < globalParametersCount; ++parameterIndex ) {
        ::slang::VariableLayoutReflection* parameter = programLayout.getParameterByIndex( parameterIndex );
        ONYX_ASSERT( parameter != nullptr );
        ::slang::ParameterCategory category = parameter->getCategory();

        if( category == ::slang::ParameterCategory::DescriptorTableSlot ) {
            // StringId32 parameterNameHash( toLower( parameter->getName() ) );
            // if( ignoreCaseEqual( parameter->getName(), "bindlesstextures" ) ) {
            //     bindlessSpaceIndex = bindingSpace;
            //     bindlessSlotIndex = parameter->getBindingIndex();
            //
            //     ShaderDescriptorSet& descriptorSet = reflectInfo.ShaderDescriptorSets.emplace_back();
            //     descriptorSet.Set = bindlessSlotIndex;
            //
            //     ImageSampler& imageSampler = descriptorSet.ImageSamplers[ bindlessSpaceIndex ];
            //     imageSampler.Stage = ShaderStage::All;
            //     imageSampler.Name = parameter->getName();
            //     imageSampler.BindingPoint = bindlessSlotIndex;
            //     imageSampler.DescriptorSet = bindlessSpaceIndex;
            // }
        }

        if( category == ::slang::ParameterCategory::PushConstantBuffer ) {
            auto pushConstantTypeLayout = parameter->getTypeLayout();
            ONYX_ASSERT( pushConstantTypeLayout != nullptr );
            reflectPushConstant( *pushConstantTypeLayout, globalPushConstants );
        }
    }

    // entry point scope
    ::Slang::ComPtr< ::slang::IMetadata > entryPointMetadata;
    const uint32_t entryPointCount = programLayout.getEntryPointCount();
    for( uint32_t i = 0; i < entryPointCount; ++i ) {
        ::slang::EntryPointReflection* entryPoint = programLayout.getEntryPointByIndex( i );
        ONYX_ASSERT( entryPoint != nullptr );

        auto result = linkedShader.getEntryPointMetadata( i, 0, entryPointMetadata.writeRef() );
        if( SLANG_FAILED( result ) ) {
            return std::nullopt;
        }

        // bool isBindlessUsed = false;
        // entryPointMetadata->isParameterLocationUsed( SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT,
        //                                              bindlessSpaceIndex,
        //                                              0,
        //                                              isBindlessUsed );
        // if( isBindlessUsed ) {
        //     reflectInfo.IsUsingBindless = true;
        // }

        ShaderStage stage = toShaderStage( entryPoint->getStage() );
        reflectInfo.Stages |= stage;

        const uint32_t parameterCount = entryPoint->getParameterCount();
        for( uint32_t parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex ) {
            ::slang::VariableLayoutReflection* parameter = entryPoint->getParameterByIndex( parameterIndex );
            ONYX_ASSERT( parameter != nullptr );

            [[maybe_unused]] auto category = parameter->getCategory();
            ::slang::TypeLayoutReflection* typeLayout = parameter->getTypeLayout();
            ONYX_ASSERT( typeLayout != nullptr );

            // this should move into the parameter loop instead of looping twice over params
            if( stage == ShaderStage::Vertex ) {
                reflectVertexStage( *parameter, *typeLayout, reflectInfo );
            }
        }

        // push constant inside entry point parameters
        ::slang::TypeLayoutReflection* entryPointTypeLayout = entryPoint->getTypeLayout();
        const uint32_t bindingRangeCount = entryPointTypeLayout->getBindingRangeCount();
        for( uint32_t bindingRangeIndex = 0; bindingRangeIndex < bindingRangeCount; ++bindingRangeIndex ) {
            ::slang::BindingType bindingType = entryPointTypeLayout->getBindingRangeType( bindingRangeIndex );

            if( bindingType == ::slang::BindingType::PushConstant ) {
                auto pushConstantTypeLayout = entryPointTypeLayout->getBindingRangeLeafTypeLayout( bindingRangeIndex );
                ONYX_ASSERT( pushConstantTypeLayout != nullptr );

                PushConstantRange& pushConstantRange = reflectInfo.getPushConstantRange( stage );
                reflectPushConstant( *pushConstantTypeLayout, pushConstantRange );
            }
        }
    }

    if( reflectInfo.PushConstantRanges.empty() ) {
        reflectInfo.PushConstantRanges.emplace_back( globalPushConstants );
    } else {
        // adjust push constant ranges per stage based on global push constants
        for( auto& pushConstantRange : reflectInfo.PushConstantRanges ) {
            pushConstantRange.Size += globalPushConstants.Size;
        }
    }

    return reflectInfo;
}

} // namespace
template < typename T >
using SlangPtr = ::Slang::ComPtr< T >;

SlangPtr< ::slang::IGlobalSession > g_globalSession = nullptr;
SlangPtr< ::slang::ISession > g_session = nullptr;

bool init() {
    if( g_globalSession != nullptr )
        return true;

    auto returnCode = ::slang::createGlobalSession( g_globalSession.writeRef() );

    if( SLANG_FAILED( returnCode ) ) {
        return false;
    }

    ::slang::TargetDesc targetDescription{};
    targetDescription.format = SLANG_SPIRV;
    targetDescription.profile = g_globalSession->findProfile( "glsl_vk" );

    ::slang::SessionDesc sessionDescription{};
    sessionDescription.targets = &targetDescription;
    sessionDescription.targetCount = 1;

    FilePath engineShaders = file_system::path::getFullPath( "engine:/shaders/" );
    FilePath projectShaders = file_system::path::getFullPath( "project:/shaders/" );

    const auto includePaths = std::array{ engineShaders.generic_string().c_str(),
                                          projectShaders.generic_string().c_str() };

    sessionDescription.searchPaths = includePaths.data();
    sessionDescription.searchPathCount = includePaths.size();

    // auto nonSemanticInfo = g_globalSession->findCapability( "SPV_KHR_non_semantic_info" );
    DynamicArray< ::slang::CompilerOptionEntry > options{
        { ::slang::CompilerOptionName::EmitSpirvDirectly,
          { ::slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr } },
        { ::slang::CompilerOptionName::VulkanUseEntryPointName,
          { ::slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr } },
        // This is completely useless to set, slangc seems to ignore it
        { ::slang::CompilerOptionName::BindlessSpaceIndex,
          { ::slang::CompilerOptionValueKind::Int, 0, 0, nullptr, nullptr } },
        { ::slang::CompilerOptionName::EnableWarning,
          { ::slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr } },
        { ::slang::CompilerOptionName::WarningsAsErrors,
          { ::slang::CompilerOptionValueKind::String, 1, 0, "all", nullptr } },
    };
    // { ::slang::CompilerOptionName::Capability,
    //   { ::slang::CompilerOptionValueKind::Int, nonSemanticInfo, 0, nullptr, nullptr } } };

    returnCode = g_globalSession->createSession( sessionDescription, g_session.writeRef() );
    if( SLANG_FAILED( returnCode ) ) {
        return false;
    }

    return true;
}
bool compile( const GraphicsSystem& graphicsSystem,
              const FilePath& shaderPath,
              const String& shaderCode,
              Shader& outShader ) {
    SlangPtr< ::slang::IBlob > diagnostics;

    String moduleName = shaderPath.stem().generic_string();
    String modulePath = file_system::path::getFullPath( shaderPath ).generic_string();

    auto session = g_session;
    ::slang::IModule* module = session->loadModuleFromSourceString( moduleName.c_str(),
                                                                    modulePath.c_str(),
                                                                    shaderCode.c_str(),
                                                                    diagnostics.writeRef() );

    if( diagnostics && diagnostics->getBufferSize() > 0 ) {
        StringView str( (const char*)diagnostics->getBufferPointer(), diagnostics->getBufferSize() );
        ONYX_LOG_ERROR( "Failed loading slang shader" );
        ONYX_LOG_ERROR( "{}", str.data() );
        return false;
    }

    if( module == nullptr ) {
        ONYX_LOG_ERROR( "Failed loading slang shader" );
        return false;
    }

    ::SlangResult result;
    int32_t entryPointCount = module->getDefinedEntryPointCount();

    DynamicArray< ::slang::IComponentType* > components;
    DynamicArray< SlangPtr< ::slang::IEntryPoint > > entryPoints;

    components.reserve( entryPointCount + 1 ); // + 1 for module
    entryPoints.reserve( entryPointCount );

    components.push_back( module );

    for( int32_t i = 0; i < entryPointCount; ++i ) {
        SlangPtr< ::slang::IEntryPoint >& entryPoint = entryPoints.emplace_back();
        result = module->getDefinedEntryPoint( i, entryPoint.writeRef() );
        if( SLANG_FAILED( result ) ) {
            return false;
        }

        components.push_back( entryPoint );
    }

    ::Slang::ComPtr< ::slang::IComponentType > composite;
    g_session->createCompositeComponentType( components.data(),
                                             numericCast< int32_t >( components.size() ),
                                             composite.writeRef(),
                                             diagnostics.writeRef() );

    SlangPtr< ::slang::IComponentType > linked;
    result = composite->link( linked.writeRef(), diagnostics.writeRef() );
    if( SLANG_FAILED( result ) ) {
        ONYX_LOG_ERROR( "Failed compositing slang shader" );
        return false;
    }

    if( diagnostics && diagnostics->getBufferSize() > 0 ) {
        ONYX_LOG_ERROR( "Failed compositing slang shader" );
        return false;
    }

    switch( graphicsSystem.getApiType() ) {
    case ApiType::Vulkan: {
        SlangPtr< ::slang::IBlob > spirvCode;
        result = linked->getTargetCode( 0, spirvCode.writeRef() );
        if( SLANG_FAILED( result ) ) {
            ONYX_LOG_ERROR( "Failed compiling slang shader. {}", shaderPath );
            return false;
        }

        Optional< ShaderReflectionInfo > reflectInfo = reflect( *linked );
        if( reflectInfo.has_value() == false )
            return false;

        vulkan::Shader& vulkanShader = static_cast< vulkan::Shader& >( outShader );
        Span< const uint8_t > byteCode{ std::bit_cast< uint8_t* >( spirvCode->getBufferPointer() ),
                                        spirvCode->getBufferSize() };
        vulkanShader.create( graphicsSystem, byteCode, *reflectInfo );
        break;
    }
    case ApiType::Dx12:
    case ApiType::None:
        ONYX_ASSERT( false, "Slang compiler not implemented for api {}", graphicsSystem.getApiType() );
        break;
    }

    return true;
}

} // namespace onyx::rhi::shader_compiler::slang
