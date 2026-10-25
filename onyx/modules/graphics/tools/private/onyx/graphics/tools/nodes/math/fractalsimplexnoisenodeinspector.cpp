#include <onyx/graphics/tools/nodes/math/fractalsimplexnoisenodeinspector.h>

#include <onyx/ui/propertygrid/propertygrid.h>

namespace onyx::ui {
/*static*/ bool PropertyInspector< onyx::graphics::shader_graph_nodes::FractalSimplexNoise2DNode >::draw(
    Type& node,
    bool /*forceShow*/ ) {
    auto drawOctaveInPropertyGrid = []( StringView id, Type::Octave& octave ) {
        // ImGui::BeginVertical( "##octave" );
        // ImGui::Dummy( ImVec2{ 1.0f, ImGui::GetTextLineHeight() } );
        bool modified = ui::property_grid::drawProperty( "Frequency", octave.Frequency );
        modified |= ui::property_grid::drawProperty( "Amplitude", octave.Amplitude );
        // ImGui::EndVertical();
        return modified;
    };

    bool isModified = ui::property_grid::drawProperty( "Octaves", node.m_octaves, drawOctaveInPropertyGrid );
    return isModified;
}
} // namespace onyx::ui
