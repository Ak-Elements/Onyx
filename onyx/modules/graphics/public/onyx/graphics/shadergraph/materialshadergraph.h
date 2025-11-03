#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics
{
    class MaterialShaderGraph : public ShaderGraph
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::MaterialShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        ShadingModel GetShadingModel() const { return m_ShadingModel; }
        void SetShadingModel(ShadingModel model) { m_ShadingModel = model; }

        ShaderInstanceHandle& GetShader() { return m_Shader; }
        const ShaderInstanceHandle& GetShader() const { return m_Shader; }

        bool IsValid() const { return m_Shader.IsValid(); }

    private:
        ShadingModel m_ShadingModel = ShadingModel::Lit;

        ShaderInstanceHandle m_Shader;
    };
}
