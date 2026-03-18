#include <onyx/ui/controls/treeview.h>

#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>

namespace onyx::ui
{
    namespace
    {
        struct StackInfo
        {
            const TreeItem& Item;
            onyxU32 Depth = 0;
        };
    }

    bool RenderTreeItem(Stack<StackInfo>& itemStack, const StackInfo& currentItem, TreeViewFlags flags)
    {
        if (currentItem.Item.Children.empty())
        {
            if (ImGui::MenuItem(currentItem.Item.Label.data(), nullptr, false, true))
            {
                currentItem.Item.OnSelected();
                return true;
            }
        }
        else
        {
            ScopedImGuiId childId(currentItem.Item.Label);
            if (enums::HasAnyFlags(flags, (TreeViewFlags::ForceOpenAll | TreeViewFlags::ForceCloseAll)))
            {
                ImGui::SetNextItemOpen(enums::HasAnyFlags(flags, TreeViewFlags::ForceOpenAll));
            }

            if (ContextMenuHeader(currentItem.Item.Label, ImGuiTreeNodeFlags_None))
            {
                for (const auto& childNode : (currentItem.Item.Children | std::views::values))
                {
                    itemStack.push({ childNode, currentItem.Depth + 1 });
                }
            }
        }

        return false;
    }

    bool RenderTreeView(StringView id, const TreeItem& root)
    {
        return RenderTreeView(id, root, TreeViewFlags::None);
    }

    bool RenderTreeView(StringView id, const TreeItem& root, TreeViewFlags flags)
    {
        ScopedImGuiId treeViewId(id);
        
        Stack<StackInfo> itemStack;
        itemStack.push({ root, 0 });
        while (itemStack.empty() == false)
        {
            const StackInfo currentItem = itemStack.top();
            itemStack.pop();

            const bool hasLabel = currentItem.Item.Label.empty() == false;
            if (hasLabel == false)
            {
                for (const auto& childNode : (currentItem.Item.Children | std::views::values))
                {
                    itemStack.push({ childNode, currentItem.Depth });
                }
                continue;
            }

            if (currentItem.Depth == 0)
            {
                if (RenderTreeItem(itemStack, currentItem, flags))
                    return true;
            }
            else
            {
                ScopedImGuiIndent indent(static_cast<onyxF32>(currentItem.Depth) * ImGui::GetStyle().IndentSpacing);
                if (RenderTreeItem(itemStack, currentItem, flags))
                    return true;
            }
        }

        return false;
    }
}
