#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics
{
    class MaterialShaderGraph : public ShaderGraph
    {
    public:
        ShadingModel GetShadingModel() const { return shadingModel; }
        void SetShadingModel(ShadingModel model) { shadingModel = model; }

    private:
        ShadingModel shadingModel = ShadingModel::Lit;
    };
}
