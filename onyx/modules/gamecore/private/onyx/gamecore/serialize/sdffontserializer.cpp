#include <onyx/gamecore/serialize/sdffontserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/gamecore/assets/sdffont.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template <>
    struct Serialization<GameCore::SDFFont>
    {
        static bool Serialize(Serializer& serializer, const GameCore::SDFFont& map)
        {
            return serializer.Write<"metrics">(map.GetMetrics()) &&
                serializer.Write<"glyphs">(map.GetGlyphs());
        }

        static bool Deserialize(const Deserializer& deserializer, GameCore::SDFFont& outFont)
        {
            GameCore::SDFFontMetrics& metrics = outFont.GetMetrics();
            if (deserializer.Read<"metrics">(metrics) == false)
            {
                ONYX_LOG_ERROR("Failed loading SDF Font, missing metrics.");
                return false;
            }

            DynamicArray<GameCore::SDFFontGlyphData> deserializedGlyphs;
            deserializer.Read<"glyphs">(deserializedGlyphs);


            HashMap<onyxU32, GameCore::SDFFontGlyphData>& glyphs = outFont.GetGlyphs();

            for (const GameCore::SDFFontGlyphData& glyph : deserializedGlyphs)
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
    struct Serialization<GameCore::SDFFontMetrics>
    {
        static bool Serialize(Serializer& serializer, const GameCore::SDFFontMetrics& metrics)
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

        static bool Deserialize(const Deserializer& deserializer, GameCore::SDFFontMetrics& outMetrics)
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
    struct Serialization<GameCore::SDFFontGlyphData>
    {
        static bool Serialize(Serializer& serializer, const GameCore::SDFFontGlyphData& glyph)
        {
            //TODO: Create and actually write out correct sdf font
            return serializer.Write<"unicode">(glyph.KeyCode) &&
                serializer.Write<"advance">(glyph.Advance);//&&
                //serializer.Write<"planeBounds">(glyph.PlaneBounds) &&
                //serializer.Write<"atlasBounds">(glyph.AtlasBounds);
        }

        static bool Deserialize(const Deserializer& deserializer, GameCore::SDFFontGlyphData& outGlyph)
        {
            bool success = deserializer.Read<"unicode">(outGlyph.KeyCode) &&
                deserializer.Read<"advance">(outGlyph.Advance);

            if (success == false)
            {
                return false;
            }

            //TODO: those bounds should be read/written in the correct format but for now we rely on the output of the msdf font tool

            //success = deserializer.Read<"planeBounds">([&](const Deserializer& scopedDeserializer)
            //{
            //    onyxF32 planeBoundLeft = 0.0f;
            //    onyxF32 planeBoundRight = 0.0f;
            //    onyxF32 planeBoundTop = 0.0f;
            //    onyxF32 planeBoundBottom = 0.0f;

            //    bool success = scopedDeserializer.Read<"left">(planeBoundLeft);
            //    success &= scopedDeserializer.Read<"right">( planeBoundRight);
            //    success &= scopedDeserializer.Read<"top">( planeBoundTop);
            //    success &= scopedDeserializer.Read<"bottom">( planeBoundBottom);

            //    outGlyph.PlaneBounds = { planeBoundLeft, planeBoundRight, planeBoundTop, planeBoundBottom };
            //    return success;
            //});

            //if (success == false)
            //{
            //    return false;
            //}

            //success = deserializer.Read<"atlasBounds">([&](const Deserializer& scopedDeserializer)
            //{
            //    onyxF32 atlasBoundLeft = 0.0f;
            //    onyxF32 atlasBoundRight = 0.0f;
            //    onyxF32 atlasBoundTop = 0.0f;
            //    onyxF32 atlasBoundBottom = 0.0f;

            //    bool success = scopedDeserializer.Read<"left">(atlasBoundLeft);
            //    success &= scopedDeserializer.Read<"right">(atlasBoundRight);
            //    success &= scopedDeserializer.Read<"top">(atlasBoundTop);
            //    success &= scopedDeserializer.Read<"bottom">(atlasBoundBottom);

            //    //TODO: hardcoded until we do the proper export of the font asset
            //    const onyxF32 atlasSize = 2980;
            //    const onyxF32 atlasSizeY = 2980;

            //    atlasBoundLeft /= numeric_cast<onyxF32>(atlasSize);
            //    atlasBoundRight /= numeric_cast<onyxF32>(atlasSize);

            //    Graphics::TextureOrigin textureOrigin = Graphics::TextureOrigin::BottomLeft;
            //    if (textureOrigin == Graphics::TextureOrigin::BottomLeft)
            //    {
            //        atlasBoundTop = (numeric_cast<onyxF32>(atlasSizeY) - atlasBoundTop);
            //        atlasBoundBottom = (numeric_cast<onyxF32>(atlasSizeY) - atlasBoundBottom);
            //    }

            //    atlasBoundTop /= numeric_cast<onyxF32>(atlasSizeY);
            //    atlasBoundBottom /= numeric_cast<onyxF32>(atlasSizeY);

            //    outGlyph.AtlasBounds = { atlasBoundLeft, atlasBoundRight, atlasBoundTop, atlasBoundBottom };
            //    return success;
            //});

            //if (success == false)
            //{
            //    return false;
            //}
            return true;
        }
    };

}

namespace Onyx::GameCore
{
    SDFFontSerializer::SDFFontSerializer(Assets::AssetSystem& assetSystem)
        : AssetSerializer(assetSystem)
    {
    }

    bool SDFFontSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer) const
    {
        const SDFFont& font = asset.As<SDFFont>();

        return serializer.Write<"texture">(font.GetTexture()->GetId().Get()) &&
            serializer.Write(font);
    }

    bool SDFFontSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const
    {
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

        Reference<Graphics::TextureAsset> fontTexture;
        m_AssetSystem.GetAsset(textureAssetId, fontTexture);
        font.SetTexture(fontTexture);

        return true;
    }
}
