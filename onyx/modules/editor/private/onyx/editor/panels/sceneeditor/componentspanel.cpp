#include <onyx/editor/panels/sceneeditor/componentspanel.h>

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/modules/sceneeditor.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entity.h>
#include <onyx/entity/entityregistry.h>

#include <onyx/ui/scopedid.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/localization/localizationmodule.h>

#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/ui/controls/treeview.h>

namespace Onyx::Editor::SceneEditor
{
    ComponentsPanel::ComponentsPanel(Localization::LocalizationModule& localizationModule)
        : m_LocalizationModule(&localizationModule)
    {
    }

    void ComponentsPanel::Render(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene)
    {
        ImGui::Checkbox(Localization::Editor::ComponentsPanel::ShowAll.Get().data(), &m_ShowAll);

        DrawSelectedEntityComponents(componentFactory, scene);

        Entity::EntityRegistry& registry = scene.GetRegistry();
        auto selectedEntities = registry.GetView<SelectedComponent>();
        if (selectedEntities.empty() == false)
        {
            DrawCreateComponentContextMenu(componentFactory, scene);
        }
    }

    void ComponentsPanel::DrawSelectedEntityComponents(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene)
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
                    if (const Entity::IComponentMeta* componentMeta = componentFactory.GetComponentMeta(componentStorageIt.first).value_or(nullptr))
                    {
                        if ((m_ShowAll == false) && (componentMeta->ShowInEditor() == false))
                        {
                            continue;
                        }

                        const StringId32 componentTypeId = componentMeta->GetTypeId();
                        Ui::ScopedImGuiId id(componentTypeId.GetId());
                        Ui::ScopedImGuiStyle style
                        {
                            { ImGuiStyleVar_FrameBorderSize, 0.0f },
                            { ImGuiStyleVar_ItemSpacing, ImVec2(0.0, 0.0f) },
                            { ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0, 0.0f) }
                        };

                        Onyx::Localization::LocalizationId localizationId(componentTypeId);
                        Localization::LocalizedString componentName = m_LocalizationModule->GetLocalized(localizationId);
                        if (Ui::ContextMenuHeader(componentName, ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY );

                            // manually clear style
                            style.Reset();

                            Ui::PropertyGrid::BeginPropertyGrid("Properties", 80.0f);
                            void* componentPtr = componentStorage.value(selectedEntity);

                            bool hasModified = false;
                            if (Entity::ComponentFactory::s_Editor.contains(componentTypeId))
                            {
                                hasModified = Entity::ComponentFactory::s_Editor[componentTypeId](componentPtr, false);
                            }
                            else
                            {
                                hasModified = componentMeta->DrawPropertyGridEditor(componentPtr);
                            }

                            if (hasModified)
                            {
                                // we only need to copy and replace the component if there is a factory associated
                                // else the component is just default constructed without special logic
                                if (componentMeta->HasFactory())
                                {
                                    bool hasCopied = componentFactory.TryCopyComponent(registry, selectedEntity, componentTypeId, componentPtr);
                                    if (hasCopied)
                                    {
                                        ONYX_LOG_WARNING("Failed updating component({}) after edit in property grid on entity {}", componentName, static_cast<onyxU32>(selectedEntity));
                                    }
                                }
                            }

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

    void ComponentsPanel::DrawCreateComponentContextMenu(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene)
    {
        if (ImGui::BeginPopupContextWindow("AddComponentModal", ImGuiPopupFlags_MouseButtonRight))
        {
            static String s_SearchString;
            static bool s_HasFocus = false;

            const bool isAppearing = ImGui::IsWindowAppearing();
            s_HasFocus |= isAppearing;

            if (isAppearing)
            {
                s_SearchString.clear();
            }

            Ui::DrawSearchBar(s_SearchString, Localization::Generic::Search.Get(), s_HasFocus);

            if (ImGui::BeginChild("##ScrollList", ImVec2(350.0f, 350.0f)))
            {
                Ui::TreeViewFlags flags = isAppearing ? Ui::TreeViewFlags::ForceCloseAll : (s_SearchString.empty() ? Ui::TreeViewFlags::None : Ui::TreeViewFlags::ForceOpenAll);
                Ui::TreeItem root = BuildComponentTree(componentFactory, scene, s_SearchString);
                //bool hasNoMenuItem = root.Children.empty();
                Ui::RenderTreeView("componentMenu", root, flags);

                //if (hasNoMenuItem)
                //    ImGui::Dummy(ImVec2(100, 10));
            }

            ImGui::EndChild();

            ImGui::EndPopup();
        }
    }

    Ui::TreeItem ComponentsPanel::BuildComponentTree(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene, StringView searchString) const
    {
        Ui::TreeItem root;
        for (auto&& [componentTypeId, componentMeta] : componentFactory.GetComponentMeta())
        {
            if (componentMeta->IsTransient() || (componentMeta->ShowInEditor() == false))
            {
                continue;
            }

            Localization::LocalizationId localizationId(componentTypeId);
            Localization::LocalizedString localizedString = m_LocalizationModule->GetLocalized(localizationId);
            StringView componentName = localizedString.Get();
            if (IgnoreCaseFind(componentName, searchString) == StringView::npos)
            {
                continue;
            }

            constexpr char delimiter = '/';
            DynamicArray<String> split = Split(componentName, delimiter);

            Ui::TreeItem* currentParent = &root;
            for (onyxU32 i = 0; i < split.size(); ++i)
            {
                const String& currentToken = split[i];
                if (i == split.size() - 1)
                {
                    Ui::TreeItem& menuItem = currentParent->Children[currentToken];
                    menuItem.Label = currentToken;
                    menuItem.OnSelected = [&]()
                        {
                            Entity::EntityRegistry& registry = scene.GetRegistry();
                            auto selectedEntities = registry.GetView<SelectedComponent>();
                            for (Entity::EntityId selectedEntity : selectedEntities)
                            {
                                componentFactory.TryCreateComponent(registry, selectedEntity, componentTypeId);
                            }

                            ImGui::CloseCurrentPopup();
                        };
                    break;
                }

                currentParent = &currentParent->Children[currentToken];
                currentParent->Label = currentToken;
            }
        }

        return root;
    }
}
