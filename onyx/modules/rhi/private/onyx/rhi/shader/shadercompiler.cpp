#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/log/logger.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shaderincluder.h>
#include <onyx/rhi/shader/shaderpreprocessor.h>
#include <onyx/rhi/vulkan/shader.h>

#include <shaderc/shaderc.hpp>
#include <spirv_glsl.hpp>

namespace onyx::rhi::ShaderCompiler {
// TODO: Move this to a vulkan shader compiler implementation
namespace {
constexpr StringView SHADER_SOURCE_TMP_PATH = "tmp:/shaders/source";
constexpr StringView SHADER_BINARIES_TMP_PATH = "tmp:/shaders/bin";

ShaderDataType GetShaderDataType( spirv_cross::SPIRType::BaseType type,
                                  uint32_t vectorSize,
                                  uint32_t /*componentSize*/ ) {
    using namespace spirv_cross;
    switch ( type ) {
    case SPIRType::Boolean:
        return ShaderDataType::Bool;
    case SPIRType::SByte: {
        if ( vectorSize == 4 )
            return ShaderDataType::Byte;

        return ShaderDataType::Byte4;
    }
    case SPIRType::UByte: {
        if ( vectorSize == 4 )
            return ShaderDataType::UByte4;

        return ShaderDataType::UByte;
    }
    case SPIRType::UInt: {
        if ( vectorSize == 4 )
            return ShaderDataType::UInt4;
        if ( vectorSize == 3 )
            return ShaderDataType::UInt3;
        if ( vectorSize == 2 )
            return ShaderDataType::UInt2;

        return ShaderDataType::UInt;
    }
    case SPIRType::Float: {
        if ( vectorSize == 4 )
            return ShaderDataType::Float4;
        if ( vectorSize == 3 )
            return ShaderDataType::Float3;
        if ( vectorSize == 2 )
            return ShaderDataType::Float2;

        return ShaderDataType::Float;
    }
    default:
        ONYX_ASSERT( false, "Unhandeled shader base type" );
        return ShaderDataType::Float;
    }
}

TextureFormat GetDefaultFormat( ShaderDataType type ) {
    switch ( type ) {
    case ShaderDataType::Bool:
        return TextureFormat::R_UINT8;
    case ShaderDataType::Float:
        return TextureFormat::R_FLOAT32;
    case ShaderDataType::Float2:
        return TextureFormat::RG_FLOAT32;
    case ShaderDataType::Float3:
        return TextureFormat::RGB_FLOAT32;
    case ShaderDataType::Float4:
        return TextureFormat::RGBA_FLOAT32;
    case ShaderDataType::Mat3:
        return TextureFormat::RGB_UNORM8;
        // case ShaderDataType::Mat4:
        //	return TextureFormat::RGBA_UNORM8;
        // case ShaderDataType::Byte:
        //	return TextureFormat::R_UINT8;
        // case ShaderDataType::Byte4:
        //	return TextureFormat::RGBA_UNORM8;
        // case ShaderDataType::UByte: break;
        // case ShaderDataType::UByte4: break;
        // case ShaderDataType::Short2: break;
        // case ShaderDataType::Short2Normalized: break;
        // case ShaderDataType::Short4: break;
        // case ShaderDataType::Short4Normalized: break;
    case ShaderDataType::UInt:
        return TextureFormat::R_UINT32;
        // case ShaderDataType::UInt2:
        //	return TextureFormat::RG_UINT32;
        // case ShaderDataType::UInt3:
        //	return TextureFormat::RGB_UINT32;
        // case ShaderDataType::UInt4:
        //	return TextureFormat::RGBA_UINT32;
    default:
        ONYX_ASSERT( false, "Unhandeled shader base type" );
        return TextureFormat::Invalid;
    }
}

uint32_t GetComponentSize( TextureFormat format ) {
    switch ( format ) {
    case TextureFormat::R_UNORM8:
    case TextureFormat::R_UINT8:
    case TextureFormat::RGB_UNORM8:
    case TextureFormat::RGBA_UNORM8:
    case TextureFormat::BGRA_UNORM8:
    case TextureFormat::SRGB_UNORM8:
    case TextureFormat::RG_UNORM8:
        return 1;
    case TextureFormat::R_UINT16:
    case TextureFormat::RG_FLOAT16:
    case TextureFormat::RGBA_FLOAT16:
        return 2;
    case TextureFormat::R_UINT32:
    case TextureFormat::R_FLOAT32:
    case TextureFormat::RG_FLOAT32:
    case TextureFormat::RGB_FLOAT32:
    case TextureFormat::RGBA_FLOAT32:
        return 4;
    default:
        ONYX_ASSERT( false, "Unhandeled texture format." );
        return 4;
    }
}

bool IsReservedResourceName( const String& resourceName ) {
    return ( resourceName == "BindlessTextures" ) || ( resourceName == "BindlessTextures3D" ) ||
           ( resourceName == "BindlessTexturesCubemaps" ) || ( resourceName == "BindlessTexturesCubemapsArray" ) ||
           ( resourceName == "BindlessImages2D" ) || ( resourceName == "BindlessUImages2D" );
}

StringView GetShaderExtension( ShaderStage stage ) {
    switch ( stage ) {
    case ShaderStage::Vertex:
        return "vert";
    case ShaderStage::Fragment:
        return "frag";
    case ShaderStage::Compute:
        return "comp";
    case ShaderStage::Invalid:
    case ShaderStage::All:
    case ShaderStage::Count:
        break;
    }

    ONYX_ASSERT( false, "Invalid shader stage {} passed", enums::ToString( stage ) );
    return "";
}

shaderc::CompileOptions SetupShaderOptions( const GraphicsSystem& graphicsSystem ) {
    using namespace shaderc;
    CompileOptions shaderCOptions;
    shaderCOptions.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3 );
    shaderCOptions.SetWarningsAsErrors();
    shaderCOptions.AddMacroDefinition( "ONYX_IS_BINDLESS", graphicsSystem.IsBindless() ? "1" : "0" );

#if !ONYX_IS_RETAIL
    if ( graphicsSystem.IsShaderDebugEnabled() ) {
        shaderCOptions.AddMacroDefinition( "ONYX_IS_DEBUG", graphicsSystem.IsShaderDebugEnabled() ? "1" : "0" );
        shaderCOptions.SetGenerateDebugInfo();
    }
#endif

    return shaderCOptions;
}

UniquePtr< ShaderIncluder > SetupShaderIncluder() {
    UniquePtr< ShaderIncluder > includer = makeUnique< ShaderIncluder >();
    for ( const FilePath& shaderDirectory : GetShaderDirectories() ) {
        includer->AddIncludeDirectory( shaderDirectory.generic_string().c_str() );
    }

    return includer;
}
} // namespace

namespace Glsl {
bool Preprocess( const GraphicsSystem& api,
                 const FilePath& sourcePath,
                 const String& shaderSourceCode,
                 ShaderStage stage,
                 String& outPreprocessedSource,
                 HashSet< String >& outIncludes ) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions shaderCOptions = SetupShaderOptions( api );

    UniquePtr< ShaderIncluder > shaderIncluderPtr = SetupShaderIncluder();
    ShaderIncluder* shaderIncluder = shaderIncluderPtr.get();
    shaderCOptions.SetIncluder( std::move( shaderIncluderPtr ) );

    shaderc_shader_kind shaderKind = shaderc_glsl_default_vertex_shader;
    switch ( stage ) {
    case ShaderStage::Vertex:
        shaderKind = shaderc_vertex_shader;
        break;
    case ShaderStage::Fragment:
        shaderKind = shaderc_fragment_shader;
        break;
    case ShaderStage::Compute:
        shaderKind = shaderc_compute_shader;
        break;
    case ShaderStage::Invalid:
    case ShaderStage::Count:
    case ShaderStage::All:
        ONYX_ASSERT( false, "Not implemented" );
        return false;
    }

    const shaderc::PreprocessedSourceCompilationResult& preProcessingResult = compiler.PreprocessGlsl(
        shaderSourceCode,
        shaderKind,
        sourcePath.string().c_str(),
        shaderCOptions );
    if ( preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success ) {
        ONYX_LOG_ERROR( "Failed to pre-process {} (Stage: {}) shader.\nError: {}",
                        sourcePath,
                        enums::toString( stage ),
                        preProcessingResult.GetErrorMessage() );
        return false;
    }

    outPreprocessedSource.assign( preProcessingResult.begin(), preProcessingResult.end() );

    for ( const String& include : shaderIncluder->GetIncludes() ) {
        outIncludes.emplace( include );
    }

    return true;
}

bool Compile( const GraphicsSystem& api,
              const FilePath& sourcePath,
              const String& preprocessedCode,
              ShaderStage stage,
              DynamicArray< uint32_t >& outByteCode ) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions shaderCOptions = SetupShaderOptions( api );

    // UniquePtr<ShaderIncluder> shaderIncluderPtr = SetupShaderIncluder();
    // ShaderIncluder* shaderIncluder = shaderIncluderPtr.get();
    // shaderCOptions.SetIncluder(std::move(shaderIncluderPtr));

    shaderc_shader_kind shaderKind = shaderc_glsl_default_vertex_shader;
    switch ( stage ) {
    case ShaderStage::Vertex:
        shaderKind = shaderc_vertex_shader;
        break;
    case ShaderStage::Fragment:
        shaderKind = shaderc_fragment_shader;
        break;
    case ShaderStage::Compute:
        shaderKind = shaderc_compute_shader;
        break;
    case ShaderStage::Invalid:
    case ShaderStage::Count:
    case ShaderStage::All:
        ONYX_ASSERT( false, "Not implemented" );
        return false;
    }

    // const shaderc::PreprocessedSourceCompilationResult& preProcessingResult =
    // compiler.PreprocessGlsl(shaderSourceCode, shaderKind, sourcePath.string().c_str(), shaderCOptions);
    //  (preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success)
    //{
    //	ONYX_LOG_ERROR("Failed to pre-process {} (Stage: {}) shader.\nError: {}", sourcePath, enums::ToString(stage),
    // preProcessingResult.GetErrorMessage()); 	return false;
    // }

    // StringView preprocessedSource(preProcessingResult.begin(), preProcessingResult.end());

    // Compile shader
    shaderCOptions.SetOptimizationLevel( shaderc_optimization_level_zero );
    const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv( preprocessedCode.data(),
                                                                            preprocessedCode.length(),
                                                                            shaderKind,
                                                                            sourcePath.string().c_str(),
                                                                            shaderCOptions );

    if ( module.GetCompilationStatus() != shaderc_compilation_status_success ) {
        String test = module.GetErrorMessage();
        ONYX_LOG_ERROR( "Failed to compile {} shader at stage: {}.\nError: {}",
                        sourcePath,
                        enums::toString( stage ),
                        test );
        return false;
    }

    outByteCode = DynamicArray< uint32_t >( module.begin(), module.end() );

#if !ONYX_IS_RETAIL

#if ONYX_IS_WINDOWS
    // Kind of a hack because thank you Windows
#undef CreateDirectory
#endif
    // TODO: Allow different shader language and update this export folder
    String shaderStageString = toLower( enums::toString( stage ) );
    const StringView exportShaderSourcePath = format::format( "{}/{}/glsl/{}.{}",
                                                              SHADER_SOURCE_TMP_PATH,
                                                              shaderStageString,
                                                              sourcePath.stem(),
                                                              GetShaderExtension( stage ) );

    file_system::OnyxFile shaderSourceFile( exportShaderSourcePath );
    file_system::FileStream shaderSourceFileStream = shaderSourceFile.OpenStream( file_system::OpenMode::Text |
                                                                                  file_system::OpenMode::Write );
    shaderSourceFileStream.writeRaw( preprocessedCode.data(), preprocessedCode.size() );

    const StringView exportShaderBinaryPath = format::format( "{}/{}/glsl/{}.spirv",
                                                              SHADER_BINARIES_TMP_PATH,
                                                              shaderStageString,
                                                              sourcePath.stem() );
    file_system::OnyxFile shaderBinaryFile( exportShaderBinaryPath );
    file_system::FileStream shaderBinaryFileStream = shaderBinaryFile.OpenStream( file_system::OpenMode::Binary |
                                                                                  file_system::OpenMode::Write );
    shaderBinaryFileStream.writeRaw( outByteCode, false );
#endif

    return true;
}
} // namespace Glsl

bool Preprocess( const GraphicsSystem& api,
                 const FilePath& sourcePath,
                 const String& code,
                 ShaderLanguage language,
                 ShaderStage stage,
                 String& outPreprocessedCode,
                 HashSet< String >& outIncludes ) {
    switch ( language ) {
    case ShaderLanguage::GLSL:
        return Glsl::Preprocess( api, sourcePath, code, stage, outPreprocessedCode, outIncludes );
    case ShaderLanguage::Invalid:
    case ShaderLanguage::HLSL:
    case ShaderLanguage::Count:
        break;
    }

    ONYX_ASSERT( false, "Shader compiler not implemented for language ({}).", enums::ToString( language ) );
    return false;
}

bool Compile( const GraphicsSystem& api,
              const FilePath& sourcePath,
              const String& preprocessedCode,
              ShaderLanguage language,
              ShaderStage stage,
              DynamicArray< uint32_t >& outByteCode ) {
#if !ONYX_IS_RETAIL

#if ONYX_IS_WINDOWS
    // Kind of a hack because thank you Windows
#undef CreateDirectory
#endif

    const String shaderLanguageString = toLower( enums::toString( language ) );
    for ( ShaderStage s = ShaderStage::Vertex; s < ShaderStage::All; ++s ) {
        String shaderStageString = toLower( enums::toString( s ) );

        const FilePath tempSourceDirectory = file_system::Path::GetFullPath( SHADER_SOURCE_TMP_PATH );
        const FilePath tempBinariesDirectory = file_system::Path::GetFullPath( SHADER_BINARIES_TMP_PATH );

        file_system::Path::CreateDirectory( tempSourceDirectory / shaderStageString / shaderLanguageString );
        file_system::Path::CreateDirectory( tempBinariesDirectory / shaderStageString / shaderLanguageString );
    }

#endif

    switch ( language ) {
    case ShaderLanguage::GLSL:
        return Glsl::Compile( api, sourcePath, preprocessedCode, stage, outByteCode );
    case ShaderLanguage::Invalid:
    case ShaderLanguage::HLSL:
    case ShaderLanguage::Count:
        break;
    }

    ONYX_ASSERT( false, "Shader compiler not implemented for language ({}).", enums::ToString( language ) );
    return false;
}

bool Reflect( ShaderStage shaderStage,
              const PreprocessedShader& preprocessedShader,
              const DynamicArray< uint32_t >& shaderByteCode,
              ShaderReflectionInfo& outReflectionInfo ) {
    ONYX_UNUSED( preprocessedShader );
    // TODO: Clean up & add support for alignment calculation for push constants for example
    using namespace spirv_cross;

    Compiler compiler( shaderByteCode );
    const ShaderResources& resources = compiler.get_shader_resources();

    if ( shaderStage == ShaderStage::Vertex ) {
        DynamicArray< VertexInput > inputs;
        for ( const spirv_cross::Resource& resource : resources.stage_inputs ) {
            const String& name = resource.name;
            const SPIRType& inputType = compiler.get_type( resource.base_type_id );

            uint32_t location = compiler.get_decoration( resource.id, spv::DecorationLocation );

            StringId32 id( name );
            uint32_t componentSize = inputType.width;
            TextureFormat format = TextureFormat::Invalid;
            auto keyValueIt = preprocessedShader.m_Formats.find( id );
            if ( keyValueIt != preprocessedShader.m_Formats.end() ) {
                format = keyValueIt->second;
                componentSize = GetComponentSize( format );
            }

            if ( format == TextureFormat::Invalid ) {
                ShaderDataType type = GetShaderDataType( inputType.basetype, inputType.vecsize, componentSize );
                format = GetDefaultFormat( type );
            }

            outReflectionInfo.vertexInput.Add( location, format );
        }
    }

    // Get out format for pixel shader
    if ( shaderStage == ShaderStage::Fragment ) {
        // DynamicArray<VertexInput> inputs;
        for ( const spirv_cross::Resource& resource : resources.stage_outputs ) {
            const String& name = resource.name;

            const SPIRType& inputType = compiler.get_type( resource.base_type_id );

            // uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);

            StringId32 id( name );
            uint32_t componentSize = inputType.width;
            TextureFormat format = TextureFormat::Invalid;
            auto keyValueIt = preprocessedShader.m_Formats.find( id );
            if ( keyValueIt != preprocessedShader.m_Formats.end() ) {
                format = keyValueIt->second;
                componentSize = GetComponentSize( format );
            }

            if ( format == TextureFormat::Invalid ) {
                ShaderDataType type = GetShaderDataType( inputType.basetype, inputType.vecsize, componentSize );
                format = GetDefaultFormat( type );
            }

            outReflectionInfo.OutputAttachments[ name ] = format;
        }
    }

    for ( const spirv_cross::Resource& resource : resources.uniform_buffers ) {
        const SmallVector< BufferRange >& activeBuffers = compiler.get_active_buffer_ranges( resource.id );
        // Discard unused buffers from headers
        if ( activeBuffers.empty() == false ) {
            const String& name = resource.name;
            const String& alias = compiler.get_name( resource.id );
            const SPIRType& bufferType = compiler.get_type( resource.base_type_id );

            uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
            uint8_t descriptorSet = numericCast< uint8_t >(
                compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
            uint32_t size = static_cast< uint32_t >( compiler.get_declared_struct_size( bufferType ) );
            if ( IsReservedDescriptorSet( descriptorSet ) ) {
                if ( IsReservedResourceName( name ) == false ) {
                    ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                    name );
                }

                continue;
            }

            ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
                static_cast< int32_t >( descriptorSet ),
                outReflectionInfo );
            UniformBuffer uniformBuffer;
            uniformBuffer.Id = StringId32( alias );
            uniformBuffer.BindingPoint = binding;
            uniformBuffer.Size = size;
            uniformBuffer.Stage = ShaderStage::All;

            shaderDescriptorSet.UniformBuffers[ binding ] = uniformBuffer;
        }
    }

    for ( const spirv_cross::Resource& resource : resources.storage_buffers ) {
        const SmallVector< BufferRange >& activeBuffers = compiler.get_active_buffer_ranges( resource.id );
        // Discard unused buffers from headers
        if ( activeBuffers.empty() == false ) {
            const String& name = resource.name;
            const String& alias = compiler.get_name( resource.id );
            const SPIRType& bufferType = compiler.get_type( resource.base_type_id );
            uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
            uint8_t descriptorSet = numericCast< uint8_t >(
                compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
            uint32_t size = static_cast< uint32_t >( compiler.get_declared_struct_size( bufferType ) );

            if ( IsReservedDescriptorSet( descriptorSet ) ) {
                if ( IsReservedResourceName( name ) == false ) {
                    ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                    name );
                }

                continue;
            }

            ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
                static_cast< int32_t >( descriptorSet ),
                outReflectionInfo );
            StorageBuffer storageBuffer;
            storageBuffer.Id = alias.empty() ? StringId32( name ) : StringId32( alias );
            storageBuffer.BindingPoint = binding;
            storageBuffer.Size = size;
            storageBuffer.Stage = ShaderStage::All;
            shaderDescriptorSet.StorageBuffers[ binding ] = storageBuffer;
        }
    }

    for ( const spirv_cross::Resource& resource : resources.push_constant_buffers ) {
        const String& bufferName = resource.name;
        const SPIRType& bufferType = compiler.get_type( resource.base_type_id );
        uint32_t bufferSize = static_cast< uint32_t >( compiler.get_declared_struct_size( bufferType ) );
        uint32_t memberCount = static_cast< uint32_t >( bufferType.member_types.size() );
        uint32_t bufferOffset = 0;

        if ( outReflectionInfo.pushConstantRanges.empty() == false ) {
            const PushConstantRange& last = outReflectionInfo.pushConstantRanges.back();
            bufferOffset = last.Offset + last.Size;
        }

        PushConstantRange& pushConstantRange = outReflectionInfo.pushConstantRanges.emplace_back();
        pushConstantRange.Stage = shaderStage;
        pushConstantRange.Size = bufferSize - bufferOffset;
        pushConstantRange.Offset = bufferOffset;

        ShaderBuffer& buffer = outReflectionInfo.constantBuffers[ bufferName ];
        buffer.Name = bufferName;
        buffer.Size = bufferSize - bufferOffset;

        for ( uint32_t i = 0; i < memberCount; i++ ) {
            const String& memberName = compiler.get_member_name( bufferType.self, i );

            uint32_t size = static_cast< uint32_t >( compiler.get_declared_struct_member_size( bufferType, i ) );
            uint32_t offset = compiler.type_struct_member_offset( bufferType, i ) - bufferOffset;

            String uniformName = format::format( "{}.{}", bufferName, memberName );
            buffer.Uniforms[ uniformName ] = ShaderUniform( uniformName, size, offset );
        }
    }

    for ( const spirv_cross::Resource& resource : resources.sampled_images ) {
        const String& name = resource.name;
        const SPIRType& baseType = compiler.get_type( resource.base_type_id );
        const SPIRType& type = compiler.get_type( resource.type_id );
        uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
        uint8_t descriptorSet = numericCast< uint8_t >(
            compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
        uint32_t dimension = baseType.image.dim;
        uint32_t arraySize = type.array.empty() ? 1 : type.array[ 0 ];

        if ( IsReservedDescriptorSet( descriptorSet ) ) {
            if ( IsReservedResourceName( name ) == false ) {
                ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                name );
            }

            continue;
        }

        ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
            static_cast< int32_t >( descriptorSet ),
            outReflectionInfo );
        ImageSampler& imageSampler = shaderDescriptorSet.ImageSamplers[ binding ];
        imageSampler.BindingPoint = binding;
        imageSampler.DescriptorSet = descriptorSet;
        imageSampler.Name = name;
        imageSampler.Stage = shaderStage;
        imageSampler.Dimension = dimension;
        imageSampler.ArraySize = arraySize;

        outReflectionInfo.shaderResources[ name ] = ShaderResourceDeclaration( name, descriptorSet, binding, 1 );
    }

    for ( const spirv_cross::Resource& resource : resources.separate_images ) {
        const String& name = resource.name;
        const SPIRType& baseType = compiler.get_type( resource.base_type_id );
        const SPIRType& type = compiler.get_type( resource.type_id );
        uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
        uint8_t descriptorSet = numericCast< uint8_t >(
            compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
        uint32_t dimension = baseType.image.dim;
        uint32_t arraySize = type.array.empty() ? 1 : type.array[ 0 ];

        if ( IsReservedDescriptorSet( descriptorSet ) ) {
            if ( IsReservedResourceName( name ) == false ) {
                ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                name );
            }

            continue;
        }

        ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
            static_cast< int32_t >( descriptorSet ),
            outReflectionInfo );
        ImageSampler& imageSampler = shaderDescriptorSet.SeparateTextures[ binding ];
        imageSampler.BindingPoint = binding;
        imageSampler.DescriptorSet = descriptorSet;
        imageSampler.Name = name;
        imageSampler.Stage = shaderStage;
        imageSampler.Dimension = dimension;
        imageSampler.ArraySize = arraySize;

        outReflectionInfo.shaderResources[ name ] = ShaderResourceDeclaration( name, descriptorSet, binding, 1 );
    }

    for ( const spirv_cross::Resource& resource : resources.separate_samplers ) {
        const String& name = resource.name;
        const SPIRType& baseType = compiler.get_type( resource.base_type_id );
        const SPIRType& type = compiler.get_type( resource.type_id );
        uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
        uint8_t descriptorSet = numericCast< uint8_t >(
            compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
        uint32_t dimension = baseType.image.dim;
        uint32_t arraySize = type.array.empty() ? 1 : type.array[ 0 ];

        if ( IsReservedDescriptorSet( descriptorSet ) ) {
            if ( IsReservedResourceName( name ) == false ) {
                ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                name );
            }

            continue;
        }

        ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
            static_cast< int32_t >( descriptorSet ),
            outReflectionInfo );
        auto& imageSampler = shaderDescriptorSet.SeparateSamplers[ binding ];
        imageSampler.BindingPoint = binding;
        imageSampler.DescriptorSet = descriptorSet;
        imageSampler.Name = name;
        imageSampler.Stage = shaderStage;
        imageSampler.Dimension = dimension;
        imageSampler.ArraySize = arraySize;

        outReflectionInfo.shaderResources[ name ] = ShaderResourceDeclaration( name, descriptorSet, binding, 1 );
    }

    for ( const spirv_cross::Resource& resource : resources.storage_images ) {
        const String& name = resource.name;
        const SPIRType& type = compiler.get_type( resource.type_id );
        uint32_t binding = compiler.get_decoration( resource.id, spv::DecorationBinding );
        uint8_t descriptorSet = numericCast< uint8_t >(
            compiler.get_decoration( resource.id, spv::DecorationDescriptorSet ) );
        uint32_t dimension = type.image.dim;
        uint32_t arraySize = type.array.empty() ? 1 : type.array[ 0 ];

        if ( IsReservedDescriptorSet( descriptorSet ) ) {
            if ( IsReservedResourceName( name ) == false ) {
                ONYX_LOG_ERROR( "Found a resource that is using the a reserved name and descriptor set. Name: {}",
                                name );
            }

            continue;
        }

        ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(
            static_cast< int32_t >( descriptorSet ),
            outReflectionInfo );
        ImageSampler& imageSampler = shaderDescriptorSet.StorageImages[ binding ];
        imageSampler.BindingPoint = binding;
        imageSampler.DescriptorSet = descriptorSet;
        imageSampler.Name = name;
        imageSampler.Dimension = dimension;
        imageSampler.ArraySize = arraySize;
        imageSampler.Stage = shaderStage;

        outReflectionInfo.shaderResources[ name ] = ShaderResourceDeclaration( name, descriptorSet, binding, 1 );
    }

    return true;
}

bool IsReservedDescriptorSet( uint8_t set ) {
    return set == BINDLESS_SET;
}

ShaderDescriptorSet& GetOrCreateShaderDescriptorSet( uint8_t setIndex, ShaderReflectionInfo& reflectionInfo ) {
    auto it = std::find_if( reflectionInfo.shaderDescriptorSets.begin(),
                            reflectionInfo.shaderDescriptorSets.end(),
                            [ & ]( ShaderDescriptorSet& descriptorSet ) { return setIndex == descriptorSet.Set; } );

    if ( it != reflectionInfo.shaderDescriptorSets.end() ) {
        return *it;
    }

    return reflectionInfo.shaderDescriptorSets.emplace_back( setIndex );
}

bool ValidateCode( const GraphicsSystem& graphicsSystem, const String& shaderSourceCode ) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions shaderCOptions = SetupShaderOptions( graphicsSystem );
    shaderCOptions.SetIncluder( SetupShaderIncluder() );
    shaderCOptions.SetForcedVersionProfile( ShaderCoreVersion, shaderc_profile_core );
    const shaderc::PreprocessedSourceCompilationResult& preProcessingResult = compiler.PreprocessGlsl(
        shaderSourceCode,
        shaderc_glsl_default_vertex_shader,
        "validation",
        shaderCOptions );
    if ( preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success ) {
        ONYX_LOG_ERROR( "Validation for shader source failed. \nError: {}", preProcessingResult.GetErrorMessage() );
        return false;
    }

    return true;
}
} // namespace onyx::rhi::ShaderCompiler
