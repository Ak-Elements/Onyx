#include <editor/panels/sceneeditor/componentspanel.h>

#include <editor/modules/sceneeditor.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entity.h>
#include <onyx/entity/entityregistry.h>

#include <onyx/ui/scopedid.h>
#include <onyx/ui/propertygrid.h>

#include <onyx/localization/localizedstring.h>
#include <imgui_internal.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/imguisystem.h>


namespace Onyx::Editor::SceneEditor
{
    namespace
    {
        // probably needed again once we add a button to add components
        //constexpr Localization::LocalizationId ADD_COMPONENT_LABEL_ID{ "editor.sceneeditor.componentspanel.addcomponent" };
        
        constexpr Localization::LocalizationId SHOW_ALL_COMPONENTS_LABEL_ID{ "editor.windows.sceneeditor.componentspanel.showall" };
        constexpr Localization::LocalizationId SEARCH_BAR_HINT_LABEL_ID{ "editor.generic.search" };
    }

    ComponentsPanel::ComponentsPanel(Localization::LocalizationModule& localizationModule)
        : m_ShowAllLabel(localizationModule.Localize<"Editor">(SHOW_ALL_COMPONENTS_LABEL_ID))
        , m_SearchHintLabel(localizationModule.Localize<"Editor">(SEARCH_BAR_HINT_LABEL_ID))
        , m_LocalizationModule(&localizationModule)
    {
        
    }


    void ComponentsPanel::Render(GameCore::Scene& scene)
    {
        ImGui::Checkbox(m_ShowAllLabel.Get().data(), &m_ShowAll);

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

                        Localization::LocalizationId localizationId(typeId);
                        if (Ui::ContextMenuHeader(localizationId, ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen))
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

        if (ImGui::BeginPopupContextWindow("AddComponentModal", ImGuiPopupFlags_MouseButtonRight))
        {
            static String s_SearchString;
            static bool s_HasFocus = false;

            s_HasFocus |= ImGui::GetCurrentWindow()->Appearing;

            if (ImGui::GetCurrentWindow()->Appearing)
                s_SearchString.clear();

            Ui::DrawSearchBar(s_SearchString, m_SearchHintLabel.Get().data(), s_HasFocus);

            Entity::EntityRegistry& registry = scene.GetRegistry();
            bool hasMenuItem = false;
            const Entity::ComponentRegistry& componentRegistry = registry.GetComponentRegistry();
            for (auto&& [componentId, meta] : componentRegistry.GetComponentMeta())
            {
                Localization::LocalizationId localizationId(componentId);
                Localization::LocalizedString componentLocalizedName = m_LocalizationModule->Localize<"Editor">(localizationId);

                StringView componentName = componentLocalizedName.Get();
                if (IgnoreCaseFind(componentName, s_SearchString) == std::string::npos)
                    continue;

                hasMenuItem = true;

                if (ImGui::MenuItem(componentName.data()))
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
