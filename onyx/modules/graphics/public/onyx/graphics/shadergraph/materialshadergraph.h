#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::graphics {
class MaterialShaderGraph : public ShaderGraph {
  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::MaterialShaderGraph" };
    static StringId32 getTypeId() { return TypeId; }

    ShadingModel getShadingModel() const { return m_shadingModel; }
    void setShadingModel( ShadingModel model ) { m_shadingModel = model; }

    rhi::ShaderInstanceHandle& getShader() { return m_shader; }
    const rhi::ShaderInstanceHandle& getShader() const { return m_shader; }

    bool isValid() const { return m_shader.isValid(); }

  private:
    ShadingModel m_shadingModel = ShadingModel::Lit;

    rhi::ShaderInstanceHandle m_shader;
};
} // namespace onyx::graphics
