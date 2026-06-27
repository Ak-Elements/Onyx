#include <onyx/graphics/serialize/meshserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/assets/meshasset.h>
#include <onyx/stream/stringstream.h>

namespace onyx::graphics {
bool MeshSerializer::serialize( [[maybe_unused]] const assets::AssetHandle< assets::AssetInterface >& asset,
                                [[maybe_unused]] const assets::AssetMetaData& meta,
                                [[maybe_unused]] Serializer& serializer,
                                [[maybe_unused]] const IEngine& engine ) const {
    return false;
}

bool MeshSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                  const assets::AssetMetaData& meta,
                                  const Deserializer& /*deserializer*/,
                                  IEngine& engine ) const {
    MeshAsset& meshAsset = asset.as< MeshAsset >();

    DynamicArray< rhi::Vertex >& vertices = meshAsset.getVertices();
    DynamicArray< uint32_t >& indices = meshAsset.getIndices();

    file_system::OnyxFile meshSource(
        file_system::path::replaceExtension( file_system::path::getFullPath( meta.Path ), "obj" ) );
    file_system::FileStream stream = meshSource.OpenStream( file_system::OpenMode::Text | file_system::OpenMode::Read );

    String meshSourceContent;
    stream.readAll( meshSourceContent );
    StringStream stringStream( meshSourceContent );
    //  uint32_t length = stringStream.GetLength();
    //  ONYX_UNUSED(length);
    StringView line;

    DynamicArray< uint32_t > vertexIndices, uvIndices, normalIndices;
    DynamicArray< Vector3f32 > tempVertices;
    DynamicArray< Vector3f32 > tempNormals;
    DynamicArray< Vector2f32 > tempTexCoords;

    // TODO fix mesh asset import
    while( stringStream.isEof() == false ) {
        stringStream.readLine( line );

        if( line.starts_with( "vn" ) ) {
            line.remove_prefix( 4 );
            float32 coords[ 3 ];
            DynamicArray< String > parts = split( line, " " );
            int32_t i = -1;
            for( const String& token : parts ) {
                std::from_chars( token.data(), token.data() + token.size(), coords[ ++i ] );
            }

            tempNormals.emplace_back( coords[ 0 ], coords[ 1 ], coords[ 2 ] );
        } else if( line.starts_with( "vt" ) ) {
            line.remove_prefix( 4 );
            float32 coords[ 2 ];
            DynamicArray< String > parts = split( line, " " );
            int32_t i = -1;
            for( const String& token : parts ) {
                std::from_chars( token.data(), token.data() + token.size(), coords[ ++i ] );
            }

            tempTexCoords.emplace_back( coords[ 0 ], coords[ 1 ] );
        }
        if( line.starts_with( "v" ) ) {
            line.remove_prefix( 2 );
            float32 coords[ 3 ];
            DynamicArray< String > parts = split( line, " " );
            int32_t i = -1;
            for( const String& token : parts ) {
                std::from_chars( token.data(), token.data() + token.size(), coords[ ++i ] );
            }

            tempVertices.emplace_back( coords[ 0 ], coords[ 1 ], coords[ 2 ] );
        } else if( line.starts_with( "f" ) ) {
            line.remove_prefix( 2 );
            uint32_t localIndices[ 12 ];

            DynamicArray< String > parts = split( line, " /" );
            int32_t i = -1;
            for( const String& token : parts ) {
                std::from_chars( token.data(), token.data() + token.size(), localIndices[ ++i ] );
            }

            vertexIndices.push_back( localIndices[ 0 ] );
            uvIndices.push_back( localIndices[ 1 ] );
            normalIndices.push_back( localIndices[ 2 ] );

            vertexIndices.push_back( localIndices[ 3 ] );
            uvIndices.push_back( localIndices[ 4 ] );
            normalIndices.push_back( localIndices[ 5 ] );

            vertexIndices.push_back( localIndices[ 6 ] );
            uvIndices.push_back( localIndices[ 7 ] );
            normalIndices.push_back( localIndices[ 8 ] );

            if( parts.size() > 9 ) {
                vertexIndices.push_back( localIndices[ 9 ] );
                uvIndices.push_back( localIndices[ 10 ] );
                normalIndices.push_back( localIndices[ 11 ] );

                vertexIndices.push_back( localIndices[ 0 ] );
                uvIndices.push_back( localIndices[ 1 ] );
                normalIndices.push_back( localIndices[ 2 ] );

                vertexIndices.push_back( localIndices[ 6 ] );
                uvIndices.push_back( localIndices[ 7 ] );
                normalIndices.push_back( localIndices[ 8 ] );
            }
        }
    }

    for( uint32_t i = 0; i < vertexIndices.size(); i++ ) {
        uint32_t vertexIndex = vertexIndices[ i ];
        uint32_t texCoordIndex = uvIndices[ i ];
        uint32_t normalIndex = normalIndices[ i ];

        Vector3f32 vertex = tempVertices[ vertexIndex - 1 ];
        auto it = std::find_if( vertices.begin(), vertices.end(), [ & ]( const rhi::Vertex& v ) {
            return Vector3f32( v.PositionX, v.PositionY, v.PositionZ ) == vertex;
        } );

        if( it != vertices.end() ) {
            indices.push_back( static_cast< uint32_t >( std::distance( vertices.begin(), it ) ) );
        } else {
            indices.push_back( static_cast< uint32_t >( vertices.size() ) );

            Vector3f32 normal = tempNormals[ normalIndex - 1 ];
            Vector2f32 texCoord = tempTexCoords[ texCoordIndex - 1 ];
            vertices.emplace_back( vertex.X, vertex.Y, vertex.Z, texCoord.X, normal.X, normal.Y, normal.Z, texCoord.Y );
        }
    }

    rhi::GraphicsSystem& graphics = engine.getSystem< rhi::GraphicsSystem >();

    rhi::BufferProperties vertexBufferProps;
    vertexBufferProps.m_Size = static_cast< uint32_t >( vertices.size() ) * sizeof( rhi::Vertex );
    vertexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Vertex );
    vertexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
    vertexBufferProps.m_DebugName = "static mesh vertices";

    graphics.createBuffer( meshAsset.m_vertexBuffer, vertexBufferProps );
    meshAsset.m_vertexBuffer.Buffer->SetData( 0, vertices.data(), static_cast< uint32_t >( vertexBufferProps.m_Size ) );

    rhi::BufferProperties indexBufferProps;
    indexBufferProps.m_Size = static_cast< uint32_t >( indices.size() ) * sizeof( uint32_t );
    indexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Index );
    indexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
    indexBufferProps.m_DebugName = "static mesh Indices";

    graphics.createBuffer( meshAsset.m_indexBuffer, indexBufferProps );
    meshAsset.m_indexBuffer.Buffer->SetData( 0, indices.data(), static_cast< uint32_t >( indexBufferProps.m_Size ) );

    return true;
}
} // namespace onyx::graphics
