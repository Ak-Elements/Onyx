#pragma once 

#include <onyx/ui/imguiwindow.h>

namespace Onyx::Ui
{
    class EngineVariablesWindow : public ImGuiWindow
    {
    public:
        static constexpr char ENGINE_VARIABLE_PATH_SEPERATOR = '/';
    private:
        struct EngineVariableGraphNode
        {
            String Label;
            DynamicArray<Variant<EngineVariableGraphNode, IEngineVariable*>> Children;
        };  

    public:
        static constexpr StringView WindowId = "EngineVariables";
        static constexpr StringView WindowCategory = "Debug";

        StringView GetWindowId() override { return WindowId; }

    private:
        void OnOpen() override;
        void OnRender(ImGuiSystem& imguiSystem) override;

        void RenderGraphNode(const EngineVariableGraphNode& group) const;

        void RebuildVariableGraph();

        EngineVariableGraphNode& GetOrCreateParent(StringView nodePath);
    private:
        // building a graph of nodes based on their id and sepeartors '/'
        String m_Search;
        bool m_SearchBarIsFocused = true;

        EngineVariableGraphNode m_Root;
    };
}