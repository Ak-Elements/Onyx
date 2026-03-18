#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::graphics
{
    class MaterialShaderGraph : public ShaderGraph
    {
    public:
        static constexpr StringId32 TypeId{ "onyx::graphics::assets::MaterialShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        ShadingModel GetShadingModel() const { return m_ShadingModel; }
        void SetShadingModel(ShadingModel model) { m_ShadingModel = model; }

        rhi::ShaderInstanceHandle& GetShader() { return m_Shader; }
        const rhi::ShaderInstanceHandle& GetShader() const { return m_Shader; }

        bool IsValid() const { return m_Shader.IsValid(); }

    private:
        ShadingModel m_ShadingModel = ShadingModel::Lit;

        rhi::ShaderInstanceHandle m_Shader;
    };
}
