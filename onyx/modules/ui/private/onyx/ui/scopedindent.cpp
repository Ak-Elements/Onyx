#include <onyx/ui/scopedindent.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace Onyx::Ui
{
    ScopedImGuiIndent::ScopedImGuiIndent()
    {
        ImGui::Indent(m_Indent);
    }

    ScopedImGuiIndent::ScopedImGuiIndent(onyxF32 indent): m_Indent(indent)
    {
        ImGui::Indent(m_Indent);
    }

    ScopedImGuiIndent::~ScopedImGuiIndent()
    {
        ImGui::Unindent(m_Indent);
    }
}
#endif