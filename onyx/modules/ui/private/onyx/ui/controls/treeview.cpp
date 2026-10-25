#include <onyx/ui/controls/treeview.h>

#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>

namespace onyx::ui {
namespace {
struct StackInfo {
    const TreeItem& Item;
    uint32_t Depth = 0;
};
} // namespace

bool renderTreeItem( Stack< StackInfo >& itemStack, const StackInfo& currentItem, TreeViewFlags flags ) {
    if( currentItem.Item.Children.empty() ) {
        if( ImGui::MenuItem( currentItem.Item.Label.data(), nullptr, false, true ) ) {
            currentItem.Item.OnSelected();
            return true;
        }
    } else {
        ScopedImGuiId childId( currentItem.Item.Label );
        if( enums::any( flags, ( TreeViewFlags::ForceOpenAll | TreeViewFlags::ForceCloseAll ) ) ) {
            ImGui::SetNextItemOpen( enums::any( flags, TreeViewFlags::ForceOpenAll ) );
        }

        if( contextMenuHeader( currentItem.Item.Label, ImGuiTreeNodeFlags_None ) ) {
            for( const auto& childNode : ( currentItem.Item.Children | std::views::values ) ) {
                itemStack.push( { childNode, currentItem.Depth + 1 } );
            }
        }
    }

    return false;
}

bool renderTreeView( StringView id, const TreeItem& root ) {
    return renderTreeView( id, root, TreeViewFlags::None );
}

bool renderTreeView( StringView id, const TreeItem& root, TreeViewFlags flags ) {
    ScopedImGuiId treeViewId( id );

    int32_t itemImguiId = 0;
    Stack< StackInfo > itemStack;
    itemStack.push( { root, 0 } );
    while( itemStack.empty() == false ) {
        ScopedImGuiId childId( itemImguiId++ );
        const StackInfo currentItem = itemStack.top();
        itemStack.pop();

        const bool hasLabel = currentItem.Item.Label.empty() == false;
        if( hasLabel == false ) {
            for( const auto& childNode : ( currentItem.Item.Children | std::views::values ) ) {
                itemStack.push( { childNode, currentItem.Depth } );
            }
            continue;
        }

        if( currentItem.Depth == 0 ) {
            if( renderTreeItem( itemStack, currentItem, flags ) )
                return true;
        } else {
            ScopedImGuiIndent indent( static_cast< float32 >( currentItem.Depth ) * ImGui::GetStyle().IndentSpacing );
            if( renderTreeItem( itemStack, currentItem, flags ) )
                return true;
        }
    }

    return false;
}
} // namespace onyx::ui
