#include <editor/panels/sceneeditor/componentspanel.h>

#include <editor/modules/sceneeditor.h>
#include <editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entity.h>
#include <onyx/entity/entityregistry.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/imguistyle.h>
#include <onyx/ui/propertygrid.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace Onyx::Editor::SceneEditor
{
    //ComponentsPanel::ComponentsPanel(SceneEditorWindow& sceneEditorModule)
    //    //: scene(scene)
    //{

    //    windowClass = sceneEditorModule.GetWindowClass();
    //    for (auto&& [id, type] : entt::resolve())
    //    {
    //        using namespace entt::literals;
    //        const entt::meta_prop& showInEditorProperty = type.prop("ShowInEditor"_hs);
    //        
    //        if (showInEditorProperty.value().cast<bool>())
    //        {
    //            const StringView& typeName = type.info().name();
    //            StringView::difference_type index = typeName.find_last_of(':') + 1;
    //            m_Components[id] = typeName.substr(index);
    //        }
    //           
    //    }
    //}

    //void ComponentsPanel::OnClose()
    //{
    //    //entitiesPanel.RemoveOnSelectedEntityChangedHandler(this, &ComponentsPanel::OnSelectedEntityChanged);
    //}

    void ComponentsPanel::Render(GameCore::Scene& /*scene*/)
    {
        
        //Application::ScopedImGuiColor style (ImGuiCol_WindowBg, 0x66C0C0C0);
        //ImGui::SetNextWindowClass(windowClass);
        //ImGui::SetNextWindowSize(ImVec2(200, 200));
        //ImGui::Begin(GetWindowId().data(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        //ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        ImGui::Checkbox("Show all", &m_ShowAll);

        /*if ()
        {

            DrawSelectedEntityComponents();
            DrawCreateComponentContextMenu();
        }*/

        //ImGui::End();
    }

    /*void ComponentsPanel::OnSelectedEntityChanged(Entity::EntityId entityId)
    {
        m_SelectedEntity = entityId;
    }*/


    void ComponentsPanel::DrawSelectedEntityComponents()
    {
        //Entity::EntityRegistry& registry = scene->GetRegistry();

        //for (auto&& componentStorageIt : registry.GetStorage())
        //{
        //    // if the component storage contains the entity we know that the entity has this component
        //    if (entt::basic_sparse_set<Entity::EntityId>& componentStorage = componentStorageIt.second; componentStorage.contains(m_SelectedEntity))
        //    {
        //        using namespace entt::literals;
        //        entt::meta_type metaClass = entt::resolve(componentStorageIt.first);

        //        if (!metaClass)
        //            continue;

        //        if (m_ShowAll == false)
        //        {
        //            const entt::meta_prop& showInEditorProperty = metaClass.prop("ShowInEditor"_hs);
        //            if (showInEditorProperty.value().cast<bool>() == false)
        //                continue;
        //        }

        //        Ui::ScopedImGuiId id(metaClass.id());
        //        Ui::ScopedImGuiStyle style
        //        {
        //            { ImGuiStyleVar_FrameBorderSize, 0.0f },
        //            { ImGuiStyleVar_ItemSpacing, ImVec2(0.0, 0.0f) },
        //            { ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0, 0.0f) }
        //        };

        //        const StringView& typeName = metaClass.info().name();
        //        StringView::difference_type index = typeName.find_last_of(':') + 1;
        //        String name(typeName.substr(index));

        //        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen))
        //        {
        //            ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);

        //            // manually clear style
        //            style.Reset();

        //            Ui::PropertyGrid::BeginPropertyGrid("Properties", 80.0f);
        //            entt::meta_any componentHandle = metaClass.from_void(componentStorage.value(m_SelectedEntity));
        //            componentHandle.invoke("DrawImGuiEditor"_hs);

        //            Ui::PropertyGrid::EndPropertyGrid();

        //            ImGui::EndChild();
        //        }
        //        else
        //        {
        //            style.Reset();
        //            ImGui::Spacing();
        //        }
        //    }
        //}
    }

    void ComponentsPanel::DrawCreateComponentContextMenu()
    {
        //if (ImGui::BeginPopupContextWindow("AddComponent", ImGuiPopupFlags_MouseButtonRight))
        //{
        //    static String s_SearchString;
        //    static bool s_HasFocus = false;

        //    s_HasFocus |= ImGui::GetCurrentWindow()->Appearing;

        //    if (ImGui::GetCurrentWindow()->Appearing)
        //        s_SearchString.clear();

        //    bool hasChanged = Ui::DrawSearchBar(s_SearchString, "Search...", s_HasFocus);
        //    if (hasChanged)
        //    {
        //        //if (s_SearchString.empty() && (m_CreateNodeData.PinId.IsValid() == false))
        //        //    m_EditorContext->ClearNodeListFilter();
        //        //else
        //        //    FilterNodeListContextMenu(s_SearchString);
        //    }

        //    Entity::EntityRegistry& registry = scene->GetRegistry();
        //    bool hasMenuItem = false;
        //    for (auto&& [componentId, name] : m_Components)
        //    {
        //        if (name.find(s_SearchString) == std::string::npos)
        //            continue;

        //        hasMenuItem = true;
        //        if (ImGui::MenuItem(name.data()))
        //        {
        //            registry.AddComponent(m_SelectedEntity, componentId, {});
        //            ImGui::CloseCurrentPopup();
        //        }
        //    }

        //    if (hasMenuItem == false)
        //        ImGui::Dummy(ImVec2(100, 10));

        //    ImGui::EndPopup();
        //}
    }
}
