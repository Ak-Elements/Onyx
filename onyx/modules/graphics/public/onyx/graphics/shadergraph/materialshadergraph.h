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

        ShaderEffectHandle& GetShaderEffect() { return m_ShaderEffect; }
        const ShaderEffectHandle& GetShaderEffect() const { return m_ShaderEffect; }

    private:
        ShadingModel m_ShadingModel = ShadingModel::Lit;

        ShaderEffectHandle m_ShaderEffect;
    };
}
