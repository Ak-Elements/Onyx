#pragma once


#include <onyx/onyx_types.h>
#include <onyx/string/format.h>
#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
    struct ShaderVariable
    {
        String Name;
        ShaderDataType Type;
        onyxU32 Offset = 0;
    };

    struct ShaderVariableHash
    {
        using is_transparent = void;
        size_t operator()(const ShaderVariable& variable) const { return std::hash<String>{}(variable.Name); }
        size_t operator()(const String& name) const { return std::hash<String>{}(name); }
    };

    struct ShaderTexture
    {
        onyxU64 Id;
    };

    class ShaderGenerator
    {
    public:
        ShaderGenerator() = default;
        virtual ~ShaderGenerator() = default;

        template <typename T>
        static String GenerateShaderValue(const T& value)
        {
            if constexpr (is_specialization_of_v<Vector4, T>)
            {
                return String(Format::Format("vec4({}, {}, {}, {})", value[0], value[1], value[2], value[3]));
            }
            else if constexpr (is_specialization_of_v<Vector3, T>)
            {
                return String(Format::Format("vec3({}, {}, {})", value[0], value[1], value[2]));
            }
            else if constexpr (is_specialization_of_v<Vector2, T>)
            {
                return String(Format::Format("vec2({}, {})", value[0], value[1]));
            }
            else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
            {
                return std::to_string(value);
            }
            else
                return "";
        }

        template <typename T>
        static String GetTypeAsShaderTypeString()
        {
            if constexpr (is_specialization_of_v<Vector4, T>)
            {
                return "vec4";
            }
            else if constexpr (is_specialization_of_v<Vector3, T>)
            {
                return "vec3";
            }
            else if constexpr (is_specialization_of_v<Vector2, T>)
            {
                return "vec2";
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                return "float";
            }
            else if constexpr (std::is_integral_v<T>)
            {
                return std::is_signed_v<T> ? "int" : "uint";
            }
            else
                return "";
        }

        onyxS32 AddTexture(onyxU64 textureId)
        {
            onyxS32 index = GetTextureIndex(textureId);
            if (index != INVALID_INDEX_32)
            {
                return index;
            }

            index = static_cast<onyxS32>(m_Textures.size());
            m_Textures.emplace_back(textureId);
            return index;
        }

        onyxS32 GetTextureIndex(onyxU64 textureId)
        {
            auto it = std::ranges::find_if(m_Textures, [&](const ShaderTexture& texture) { return texture.Id == textureId; });
            if (it == m_Textures.end())
            {
                return INVALID_INDEX_32;
            }

            return static_cast<onyxS32>(std::distance(m_Textures.begin(), it));
        }

        void SetStage(ShaderStage stage) { m_CurrentStage = stage; }
        ShaderStage GetStage() const { return m_CurrentStage; }

        void AppendCode(StringView code);

        bool HasPushConstant(StringView name) const;
        bool HasPushConstant(ShaderStage stage, StringView name) const;

        void AddPushConstant(StringView name, ShaderDataType type);
        void AddPushConstant(ShaderStage stage, StringView name, ShaderDataType type);
        void AddPushConstant(ShaderStage stage, StringView name, ShaderDataType type, onyxU32 offset);

        void AddInclude(String include);
        void AddInclude(ShaderStage stage, String include);

        // TODO: Do not submit and fix shader generator isntead of hacking it like that
        virtual String GenerateShader();

    private:
        void GenerateVertexShader();
        void GenerateFragmentShader();

        void GeneratePushConstants(String& stageCode);
        void GenerateIncludes(String& stageCode);

        virtual void DoGenerateFragmentMain() {}

    // TODO: Do not submit and fix shader generator isntead of hacking it like that
    protected:
        DynamicArray<ShaderTexture> m_Textures;

        InplaceArray<DynamicArray<ShaderVariable>, MAX_SHADER_STAGES> m_PushConstants;

        DynamicArray<ShaderVariable> m_VertexInputs;
        DynamicArray<ShaderVariable> m_VertexOutputs;

        ShaderStage m_CurrentStage = ShaderStage::Invalid;
        InplaceArray<HashSet<String>, MAX_SHADER_STAGES> m_ShaderStagesIncludes;
        InplaceArray<String, MAX_SHADER_STAGES> m_ShaderStagesCode;
    };

    class PBRShaderGenerator : public ShaderGenerator
    {
    public:
        PBRShaderGenerator();

    protected:
        void DoGenerateFragmentMain() override;
    };
}
