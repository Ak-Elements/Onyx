#include <onyx/graphics/serialize/sdffontserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template <>
    struct Serialization<Graphics::SDFFont>
    {
        static bool Serialize(Serializer& serializer, const Graphics::SDFFont& map)
        {
            return serializer.Write<"metrics">(map.GetMetrics()) &&
                serializer.Write<"glyphs">(map.GetGlyphs());
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::SDFFont& outFont)
        {
            Graphics::SDFFontMetrics& metrics = outFont.GetMetrics();
            if (deserializer.Read<"metrics">(metrics) == false)
            {
                ONYX_LOG_ERROR("Failed loading SDF Font, missing metrics.");
                return false;
            }

            DynamicArray<Graphics::SDFFontGlyphData> deserializedGlyphs;
            deserializer.Read<"glyphs">(deserializedGlyphs);


            HashMap<onyxU32, Graphics::SDFFontGlyphData>& glyphs = outFont.GetGlyphs();

            for (const Graphics::SDFFontGlyphData& glyph : deserializedGlyphs)
            {
                if (glyphs.contains(glyph.KeyCode))
                {
                    ONYX_LOG_ERROR("Failed loading SDF font glyph with keycode {} is already registered.", glyph.KeyCode);
                    return false;
                }

                glyphs[glyph.KeyCode] = glyph;
            }

            return true;
        }
    };

    template <>
    struct Serialization<Graphics::SDFFontMetrics>
    {
        static bool Serialize(Serializer& serializer, const Graphics::SDFFontMetrics& metrics)
        {
            return serializer.Write<"glyphSize">(metrics.GlpyhSize) &&
                serializer.Write<"atlasSize">(metrics.AtlasSize) &&
                serializer.Write<"emSize">(metrics.Size) &&
                serializer.Write<"lineHeight">(metrics.LineHeight) &&
                serializer.Write<"ascender">(metrics.Ascender) &&
                serializer.Write<"descender">(metrics.Descender) &&
                serializer.Write<"underlineY">(metrics.UnderLineY) &&
                serializer.Write<"underlineThickness">(metrics.UnderlineThickness) &&
                serializer.Write<"kerning">(metrics.Kerning);
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::SDFFontMetrics& outMetrics)
        {
            return deserializer.Read<"glyphSize">(outMetrics.GlpyhSize) &&
                deserializer.Read<"atlasSize">(outMetrics.AtlasSize) &&
                deserializer.Read<"emSize">(outMetrics.Size) &&
                deserializer.Read<"lineHeight">(outMetrics.LineHeight) &&
                deserializer.Read<"ascender">(outMetrics.Ascender) &&
                deserializer.Read<"descender">(outMetrics.Descender) &&
                deserializer.Read<"underlineY">(outMetrics.UnderLineY) &&
                deserializer.Read<"underlineThickness">(outMetrics.UnderlineThickness) &&
                deserializer.Read<"kerning">(outMetrics.Kerning);
        }
    };

    template <>
    struct Serialization<Graphics::SDFFontGlyphData>
    {
        static bool Serialize(Serializer& serializer, const Graphics::SDFFontGlyphData& glyph)
        {
            //TODO: Create and actually write out correct sdf font
            return serializer.Write<"unicode">(glyph.KeyCode) &&
                serializer.Write<"advance">(glyph.Advance) &&
                serializer.Write<"planeBounds">(glyph.PlaneBounds) &&
                serializer.Write<"atlasBounds">(glyph.AtlasBounds);
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::SDFFontGlyphData& outGlyph)
        {
            bool success = deserializer.Read<"unicode">(outGlyph.KeyCode) &&
                deserializer.Read<"advance">(outGlyph.Advance);

            if (success == false)
            {
                return false;
            }

            deserializer.Read<"planeBounds">(outGlyph.PlaneBounds);
            deserializer.Read<"atlasBounds">(outGlyph.AtlasBounds);

            return true;
        }
    };

}

namespace Onyx::Graphics
{
    bool SDFFontSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer, IEngine& /*engine*/) const
    {
        const SDFFont& font = asset.As<SDFFont>();

        return serializer.Write<"texture">(font.GetTextureId().Get()) &&
            serializer.Write(font);
    }

    bool SDFFontSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const
    {
        Assets::AssetSystem& assetSystem = engine.GetSystem<Assets::AssetSystem>();

        SDFFont& font = asset.As<SDFFont>();

        onyxU64 textureAssetId;
        if (deserializer.Read<"texture">(textureAssetId) == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF Font(Path:{}), missing texture asset id.", meta.Path);
            return false;
        }

        if (deserializer.Read(font) == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF Font(Path:{}), font data invalid.", meta.Path);
            return false;
        }

        Reference<TextureAsset> fontTexture;
        assetSystem.GetAsset(textureAssetId, fontTexture);
        font.SetTextureId(Assets::AssetId(textureAssetId));
        font.SetTexture(fontTexture);

        return true;
    }
}
