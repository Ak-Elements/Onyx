#pragma once

#include <onyx/graphics/shadergraph/nodes/math/fractalsimplexnoisenode.h>
#include <onyx/ui/propertygrid/propertyinspector.h>

namespace onyx::ui {
template <>
struct PropertyInspector< onyx::graphics::shader_graph_nodes::FractalSimplexNoise2DNode > {
    using Type = typename onyx::graphics::shader_graph_nodes::FractalSimplexNoise2DNode;
    static bool draw( Type& node, bool /*forceShow*/ );
};
} // namespace onyx::ui
