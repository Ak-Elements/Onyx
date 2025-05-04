#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics
{
    class MaterialShaderGraph : public ShaderGraph
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::MaterialShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        ShadingModel GetShadingModel() const { return shadingModel; }
        void SetShadingModel(ShadingModel model) { shadingModel = model; }

    private:
        ShadingModel shadingModel = ShadingModel::Lit;
    };
}
