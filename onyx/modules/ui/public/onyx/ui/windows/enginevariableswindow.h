#pragma once

#include <onyx/ui/imguiwindow.h>

namespace onyx::ui {
class EngineVariablesWindow : public ImGuiWindow {
  public:
    static constexpr char EngineVariablePathSeperator = '/';

  private:
    struct EngineVariableGraphNode {
        String Label;
        DynamicArray< Variant< EngineVariableGraphNode, IEngineVariable* > > Children;
    };

  public:
    static constexpr StringView WindowId = "EngineVariables";
    static constexpr StringView WindowCategory = "Debug";

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ImGuiSystem& imguiSystem ) override;

    void renderGraphNode( const EngineVariableGraphNode& graphNode ) const;

    void rebuildVariableGraph();

    EngineVariableGraphNode& getOrCreateParent( StringView nodePath );

  private:
    // building a graph of nodes based on their id and sepeartors '/'
    String m_search;
    bool m_searchBarIsFocused = true;

    EngineVariableGraphNode m_root;
};
} // namespace onyx::ui
