#include <editor/panels/sceneeditor/componentspanel.h>

#include <editor/modules/sceneeditor.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entity.h>
#include <onyx/entity/entityregistry.h>

#include <onyx/ui/imguistyle.h>
#include <onyx/ui/propertygrid.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>

namespace Onyx::Editor::SceneEditor
{

    void ComponentsPanel::Render(GameCore::Scene& scene)
    {
        ImGui::Checkbox("Show all", &m_ShowAll);

        DrawSelectedEntityComponents(scene);
        DrawCreateComponentContextMenu(scene);
    }

    void ComponentsPanel::DrawSelectedEntityComponents(GameCore::Scene& scene)
    {
        Entity::EntityRegistry& registry = scene.GetRegistry();
        auto selectedEntities = registry.GetView<SelectedComponent>();

        for (Entity::EntityId selectedEntity : selectedEntities)
        {
            for (auto&& componentStorageIt : registry.GetStorage())
            {
                // if the component storage contains the entity we know that the entity has this component
                if (entt::basic_sparse_set<Entity::EntityId>& componentStorage = componentStorageIt.second; componentStorage.contains(selectedEntity))
                {
                    if (const Entity::IComponentMeta* componentMeta = registry.GetComponentMeta(componentStorageIt.first).value_or(nullptr))
                    {
                        if ((m_ShowAll == false) && (componentMeta->ShowInEditor() == false))
                        {
                            continue;
                        }

                        const StringId32 typeId = componentMeta->GetTypeId();
                        Ui::ScopedImGuiId id(typeId.GetId());
                        Ui::ScopedImGuiStyle style
                        {
                            { ImGuiStyleVar_FrameBorderSize, 0.0f },
                            { ImGuiStyleVar_ItemSpacing, ImVec2(0.0, 0.0f) },
                            { ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0, 0.0f) }
                        };

                        const StringView& typeName = typeId.GetString();
                        StringView::difference_type index = typeName.find_last_of(':') + 1;
                        String name(typeName.substr(index));

                       if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen))
                       {
                           ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);

                           // manually clear style
                           style.Reset();

                           Ui::PropertyGrid::BeginPropertyGrid("Properties", 80.0f);
                           componentMeta->DrawPropertyGridEditor(componentStorage.value(selectedEntity));

                           //TODO: Needed for now to not auto extend if component is empty
                           ImGui::Dummy(ImVec2(1, 1));
                           
                           Ui::PropertyGrid::EndPropertyGrid();
                           ImGui::EndChild();
                       }
                       else
                       {
                           style.Reset();
                           ImGui::Spacing();
                       }
                    }

                   
                }
            }
        }
    }

    void ComponentsPanel::DrawCreateComponentContextMenu(GameCore::Scene& scene)
    {
        auto selectedEntities = scene.GetRegistry().GetView<SelectedComponent>();

        if (ImGui::BeginPopupContextWindow("AddComponent", ImGuiPopupFlags_MouseButtonRight))
        {
            static String s_SearchString;
            static bool s_HasFocus = false;

            s_HasFocus |= ImGui::GetCurrentWindow()->Appearing;

            if (ImGui::GetCurrentWindow()->Appearing)
                s_SearchString.clear();

            Ui::DrawSearchBar(s_SearchString, "Search...", s_HasFocus);

            Entity::EntityRegistry& registry = scene.GetRegistry();
            bool hasMenuItem = false;
            const Entity::ComponentRegistry& componentRegistry = registry.GetComponentRegistry();
            for (auto&& [componentId, meta] : componentRegistry.GetComponentMeta())
            {
                const StringView& typeName = componentId.GetString();
                StringView::difference_type index = typeName.find_last_of(':') + 1;
                StringView name(typeName.substr(index));

                if (IgnoreCaseFind(name, s_SearchString) == std::string::npos)
                    continue;

                hasMenuItem = true;

                if (ImGui::MenuItem(name.data()))
                {
                    for (Entity::EntityId selectedEntity : selectedEntities)
                    {
                        ONYX_UNUSED(selectedEntity);
                        std::any newComponent = meta->Create();
                        registry.AddComponent(selectedEntity, componentId, newComponent);
                    }

                    ImGui::CloseCurrentPopup();
                }
            }

            if (hasMenuItem == false)
                ImGui::Dummy(ImVec2(100, 10));

            ImGui::EndPopup();
        }
    }
}
