#include <onyx/graphics/serialize/sdffontserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
template <>
struct Serialization< graphics::SDFFont > {
    static bool serialize( Serializer& serializer, const graphics::SDFFont& map ) {
        return serializer.write< "metrics" >( map.GetMetrics() ) && serializer.write< "glyphs" >( map.GetGlyphs() );
    }

    static bool deserialize( const Deserializer& deserializer, graphics::SDFFont& outFont ) {
        graphics::SDFFontMetrics& metrics = outFont.GetMetrics();
        if ( deserializer.read< "metrics" >( metrics ) == false ) {
            ONYX_LOG_ERROR( "Failed loading SDF Font, missing metrics." );
            return false;
        }

        DynamicArray< graphics::SDFFontGlyphData > deserializedGlyphs;
        deserializer.read< "glyphs" >( deserializedGlyphs );

        HashMap< uint32_t, graphics::SDFFontGlyphData >& glyphs = outFont.GetGlyphs();

        for ( const graphics::SDFFontGlyphData& glyph : deserializedGlyphs ) {
            if ( glyphs.contains( glyph.KeyCode ) ) {
                ONYX_LOG_ERROR( "Failed loading SDF font glyph with keycode {} is already registered.", glyph.KeyCode );
                return false;
            }

            glyphs[ glyph.KeyCode ] = glyph;
        }

        return true;
    }
};

template <>
struct Serialization< graphics::SDFFontMetrics > {
    static bool serialize( Serializer& serializer, const graphics::SDFFontMetrics& metrics ) {
        return serializer.write< "glyphSize" >( metrics.GlpyhSize ) &&
               serializer.write< "atlasSize" >( metrics.AtlasSize ) && serializer.write< "emSize" >( metrics.Size ) &&
               serializer.write< "lineHeight" >( metrics.LineHeight ) &&
               serializer.write< "ascender" >( metrics.Ascender ) &&
               serializer.write< "descender" >( metrics.Descender ) &&
               serializer.write< "underlineY" >( metrics.UnderLineY ) &&
               serializer.write< "underlineThickness" >( metrics.UnderlineThickness ) &&
               serializer.write< "kerning" >( metrics.Kerning );
    }

    static bool deserialize( const Deserializer& deserializer, graphics::SDFFontMetrics& outMetrics ) {
        return deserializer.read< "glyphSize" >( outMetrics.GlpyhSize ) &&
               deserializer.read< "atlasSize" >( outMetrics.AtlasSize ) &&
               deserializer.read< "emSize" >( outMetrics.Size ) &&
               deserializer.read< "lineHeight" >( outMetrics.LineHeight ) &&
               deserializer.read< "ascender" >( outMetrics.Ascender ) &&
               deserializer.read< "descender" >( outMetrics.Descender ) &&
               deserializer.read< "underlineY" >( outMetrics.UnderLineY ) &&
               deserializer.read< "underlineThickness" >( outMetrics.UnderlineThickness ) &&
               deserializer.read< "kerning" >( outMetrics.Kerning );
    }
};

template <>
struct Serialization< graphics::SDFFontGlyphData > {
    static bool serialize( Serializer& serializer, const graphics::SDFFontGlyphData& glyph ) {
        // TODO: Create and actually write out correct sdf font
        return serializer.write< "unicode" >( glyph.KeyCode ) && serializer.write< "advance" >( glyph.Advance ) &&
               serializer.write< "planeBounds" >( glyph.PlaneBounds ) &&
               serializer.write< "atlasBounds" >( glyph.AtlasBounds );
    }

    static bool deserialize( const Deserializer& deserializer, graphics::SDFFontGlyphData& outGlyph ) {
        bool success = deserializer.read< "unicode" >( outGlyph.KeyCode ) &&
                       deserializer.read< "advance" >( outGlyph.Advance );

        if ( success == false ) {
            return false;
        }

        deserializer.read< "planeBounds" >( outGlyph.PlaneBounds );
        deserializer.read< "atlasBounds" >( outGlyph.AtlasBounds );

        return true;
    }
};

} // namespace onyx

namespace onyx::graphics {
bool SDFFontSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                   const assets::AssetMetaData& /*meta*/,
                                   Serializer& serializer,
                                   const IEngine& /*engine*/ ) const {
    const SDFFont& font = asset.as< SDFFont >();

    return serializer.write< "texture" >( asset.getId().get() ) && serializer.write( font );
}

bool SDFFontSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                     const assets::AssetMetaData& meta,
                                     const Deserializer& deserializer,
                                     IEngine& engine ) const {
    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();

    SDFFont& font = asset.as< SDFFont >();

    uint64_t textureAssetId;
    if ( deserializer.read< "texture" >( textureAssetId ) == false ) {
        ONYX_LOG_ERROR( "Failed loading SDF Font(Path:{}), missing texture asset id.", meta.Path );
        return false;
    }

    if ( deserializer.read( font ) == false ) {
        ONYX_LOG_ERROR( "Failed loading SDF Font(Path:{}), font data invalid.", meta.Path );
        return false;
    }

    assets::AssetHandle< TextureAsset > fontTexture;
    assetSystem.getAsset( assets::AssetId( textureAssetId ), fontTexture );
    font.SetTexture( fontTexture );

    return true;
}
} // namespace onyx::graphics
