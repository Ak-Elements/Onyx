#include <onyx/gamecore/serialize/sdffontserializer.h>

#include <onyx/gamecore/assets/sdffont.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::GameCore
{
    SDFFontSerializer::SDFFontSerializer(Assets::AssetSystem& assetSystem)
        : AssetSerializer(assetSystem)
    {
    }

    bool SDFFontSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool SDFFontSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        const SDFFont& font = asset.As<SDFFont>();

        FileSystem::JsonValue jsonRoot;
        jsonRoot.Set("texture", font.GetTexture()->GetId().Get());

        // metrics
        FileSystem::JsonValue metricsJsonObj;
        const SDFFontMetrics& metrics = font.GetMetrics();

        metricsJsonObj.Set("glyphSize", metrics.GlpyhSize);
        
        Array<onyxU32, 2> atlasSize{ metrics.AtlasSize[0], metrics.AtlasSize[1] };
        metricsJsonObj.Set("atlasSize", atlasSize);

        metricsJsonObj.Set("size", metrics.Size);

        metricsJsonObj.Set("lineHeight", metrics.LineHeight);
        metricsJsonObj.Set("ascender", metrics.Ascender);
        metricsJsonObj.Set("descender", metrics.Descender);
        metricsJsonObj.Set("underLineY", metrics.UnderLineY);
        metricsJsonObj.Set("underlineThickness", metrics.UnderlineThickness);
        metricsJsonObj.Set("kerning", metrics.Kerning);

        jsonRoot.Add(metricsJsonObj);

        // glyphs
        FileSystem::JsonValue glyphsJson;
        FileSystem::JsonValue glyphJsonObj;

        for (auto&& [keyCode, glyph] : font.GetGlyphs())
        {
            glyphJsonObj.Set("unicode", keyCode);
            glyphJsonObj.Set("advance", glyph.Advance);

            Array<onyxF32, 2> position{ glyph.PlaneBounds.m_Position[0], glyph.PlaneBounds.m_Position[1] };
            glyphJsonObj.Set("planePosition", position);

            Array<onyxF32, 2> bounds{ glyph.PlaneBounds.m_Size[0], glyph.PlaneBounds.m_Size[1] };
            glyphJsonObj.Set("planeBounds", bounds);

            position = { glyph.AtlasBounds.m_Position[0], glyph.AtlasBounds.m_Position[1] };
            glyphJsonObj.Set("atlasPosition", position);

            bounds = { glyph.AtlasBounds.m_Size[0], glyph.AtlasBounds.m_Size[1] };
            glyphJsonObj.Set("atlasBounds", bounds);

            glyphsJson.Add(glyphJsonObj);
        }

        const String& jsonString = jsonRoot.Json.dump(4);
        using namespace FileSystem;
        OnyxFile graphOutFile(filePath);
        FileStream stream = graphOutFile.OpenStream(OpenMode::Write | OpenMode::Text);
        stream.WriteRaw(jsonString.data(), jsonString.size());

        return true;
    }

    bool SDFFontSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool SDFFontSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }

    bool SDFFontSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        SDFFont& font = asset.As<SDFFont>();

        using namespace FileSystem;
        OnyxFile graphFile(filePath);
        const JsonValue& sdfFontJsonRoot = graphFile.LoadJson();

        onyxU64 textureAssetId;
        if (sdfFontJsonRoot.Get("texture", textureAssetId) == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF Font(Path:{}), missing texture asset id.", filePath);
            return false;
        }
        
        // metrics
        JsonValue metricsJsonObj;
        if (sdfFontJsonRoot.Get("metrics", metricsJsonObj) == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF Font(Path:{}), missing metrics.", filePath);
            return false;
        }

        SDFFontMetrics& metrics = font.GetMetrics();

        bool isValid = true;

        isValid &= metricsJsonObj.Get("glyphSize", metrics.GlpyhSize);

        Array<onyxU32, 2> atlasSize;
        isValid &= metricsJsonObj.Get("atlasSize", atlasSize);
        metrics.AtlasSize = { atlasSize[0], atlasSize[1] };

        isValid &= metricsJsonObj.Get("emSize", metrics.Size);

        isValid &= metricsJsonObj.Get("lineHeight", metrics.LineHeight);
        isValid &= metricsJsonObj.Get("ascender", metrics.Ascender);
        isValid &= metricsJsonObj.Get("descender", metrics.Descender);
        isValid &= metricsJsonObj.Get("underlineY", metrics.UnderLineY);
        isValid &= metricsJsonObj.Get("underlineThickness", metrics.UnderlineThickness);
        isValid &= metricsJsonObj.Get("kerning", metrics.Kerning);

        if (isValid == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF Font(Path:{}), invalid metrics.", filePath);
            return false;
        }

        // glyphs
        JsonValue glyphsJson;
        if (sdfFontJsonRoot.Get("glyphs", glyphsJson) == false)
        {
            ONYX_LOG_ERROR("Failed loading SDF font(Path:{}), missing glyphs.", filePath);
            return false;
        }

        HashMap<onyxU32, SDFFontGlyphData>& glyphs = font.GetGlyphs();

        Graphics::TextureOrigin textureOrigin = Graphics::TextureOrigin::BottomLeft;

        JsonValue tmpJson;
        for (const nlohmann::ordered_json& glyphRawJson : glyphsJson.Json)
        {
            JsonValue glyphJsonObj{ glyphRawJson };

            onyxU32 keyCode;
            glyphJsonObj.Get("unicode", keyCode);
            if (glyphs.contains(keyCode))
            {
                ONYX_LOG_ERROR("Failed loading SDF font(Path:{}) because the key with keycode {} is already registered.", filePath, keyCode);
                return false;
            }

            SDFFontGlyphData& glyph = glyphs[keyCode];
            glyphJsonObj.Get("advance", glyph.Advance);

            // Temp to make it work with exported msdf texture from the tool
            bool hasAllBounds;
            if (glyphJsonObj.Get("planeBounds", tmpJson))
            {
                hasAllBounds = true;

                onyxF32 planeBoundLeft = 0.0f;
                onyxF32 planeBoundRight = 0.0f;
                onyxF32 planeBoundTop = 0.0f;
                onyxF32 planeBoundBottom = 0.0f;

                hasAllBounds &= tmpJson.Get("left", planeBoundLeft);
                hasAllBounds &= tmpJson.Get("right", planeBoundRight);
                hasAllBounds &= tmpJson.Get("top", planeBoundTop);
                hasAllBounds &= tmpJson.Get("bottom", planeBoundBottom);

                glyph.PlaneBounds = { planeBoundLeft, planeBoundRight, planeBoundTop, planeBoundBottom };

                if (hasAllBounds == false)
                {
                    ONYX_LOG_ERROR("Font meta data json is missing plane bounds expected 4.");
                    return false;
                }
            }

            if (glyphJsonObj.Get("atlasBounds", tmpJson))
            {
                hasAllBounds = true;

                onyxF32 atlasBoundLeft = 0.0f;
                onyxF32 atlasBoundRight = 0.0f;
                onyxF32 atlasBoundTop = 0.0f;
                onyxF32 atlasBoundBottom = 0.0f;

                hasAllBounds &= tmpJson.Get("left", atlasBoundLeft);
                hasAllBounds &= tmpJson.Get("right", atlasBoundRight);
                hasAllBounds &= tmpJson.Get("top", atlasBoundTop);
                hasAllBounds &= tmpJson.Get("bottom", atlasBoundBottom);

                atlasBoundLeft /= numeric_cast<onyxF32>(metrics.AtlasSize[0]);
                atlasBoundRight /= numeric_cast<onyxF32>(metrics.AtlasSize[0]);

                if (textureOrigin == Graphics::TextureOrigin::BottomLeft)
                {
                    atlasBoundTop = (numeric_cast<onyxF32>(metrics.AtlasSize[1]) - atlasBoundTop);
                    atlasBoundBottom = (numeric_cast<onyxF32>(metrics.AtlasSize[1]) - atlasBoundBottom);
                }

                atlasBoundTop /= numeric_cast<onyxF32>(metrics.AtlasSize[1]);
                atlasBoundBottom /= numeric_cast<onyxF32>(metrics.AtlasSize[1]);

                glyph.AtlasBounds = { atlasBoundLeft, atlasBoundRight, atlasBoundTop, atlasBoundBottom };
                if (hasAllBounds == false)
                {
                    ONYX_LOG_ERROR("Font meta data json is missing atlas bounds expected 4.");
                    return false;
                }
            }

            // TODO: This should be the way forward
            //std::array<onyxF32, 2> position;
            //glyphJsonObj.Get("planePosition", position);
            //glyph.PlaneBounds.m_Position = { position[0], position[1] };

            //std::array<onyxF32, 2> bounds;
            //glyphJsonObj.Get("planeBounds", bounds);
            //glyph.PlaneBounds.m_Size = { bounds[0], bounds[1] };

            //glyphJsonObj.Get("atlasPosition", position);
            //glyph.AtlasBounds.m_Position = { position[0], position[1] };

            //glyphJsonObj.Get("atlasBounds", bounds);
            //glyph.AtlasBounds.m_Size = { bounds[0], bounds[1] };
        }

        Reference<Graphics::TextureAsset> fontTexture;
        m_AssetSystem.GetAsset(textureAssetId, fontTexture);
        font.SetTexture(fontTexture);

        return true;
    }

    bool SDFFontSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }
}
