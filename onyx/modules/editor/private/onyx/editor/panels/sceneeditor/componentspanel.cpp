#include <onyx/editor/panels/sceneeditor/componentspanel.h>

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/commands/scene/addcomponentcommand.h>
#include <onyx/editor/commands/scene/modifycomponentcommand.h>
#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/gamecore/gamecore.h>
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
#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/controls/treeview.h>


namespace Onyx::Editor::SceneEditor
{
    void ComponentsPanel::OnOpen()
    {
    }

    void ComponentsPanel::OnClose()
    {
    }

    void ComponentsPanel::OnRender(Ui::ImGuiSystem& /*imguiSystem*/)
    {
        SetName(String(GetWindowId()));
        Begin();
        
        ImGui::Checkbox(Localization::Editor::ComponentsPanel::ShowAll.Get().data(), &m_ShowAll);

        GameCore::GameCoreSystem& gameCoreSystem = GetEngineSystem<GameCore::GameCoreSystem>();
        const Localization::LocalizationModule& localizationModule = GetEngineSystem<Localization::LocalizationModule>();        
        
        SceneEditorWindow& parent = *GetParent<SceneEditorWindow>().value();
        GameCore::Scene& scene = parent.GetScene();

        Entity::EntityRegistry& registry = scene.GetRegistry();

        DrawSelectedEntityComponents(registry,parent.GetSceneId(), gameCoreSystem, localizationModule);

        auto selectedEntities = registry.GetView<SelectedComponent>();
        if (selectedEntities.empty() == false)
        {
            DrawCreateComponentContextMenu(registry, parent.GetSceneId(), gameCoreSystem, localizationModule);
        }

        End();
    }

    void ComponentsPanel::DrawSelectedEntityComponents(Entity::EntityRegistry& registry, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem, const Localization::LocalizationModule& localizationModule)
    {
        const Entity::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();
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
                        if (Ui::PropertyInspectors::IsTypeRegistered(componentMeta->GetRuntimeTypeId()) == false)
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
                        Localization::LocalizedString componentName = localizationModule.GetLocalized(localizationId);
                        if (Ui::ContextMenuHeader(componentName, ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY );

                            // manually clear style
                            style.Reset();

                            Ui::PropertyGrid::BeginPropertyGrid("Properties", 80.0f);

                            void* componentPtr = componentStorage.value(selectedEntity);
                            
                            bool hasModified = Ui::PropertyInspectors::Draw(componentMeta->GetRuntimeTypeId(), componentPtr, m_ShowAll);
                            
                            if (hasModified)
                            {
                                DynamicArray<onyxU32> componentData;
                                componentMeta->Copy(componentPtr, componentData);
                                m_CommandGraph->Push(ModifyComponentCommand(selectedEntity, componentTypeId, std::move(componentData), sceneId, gameCoreSystem));
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

    void ComponentsPanel::DrawCreateComponentContextMenu(Entity::EntityRegistry& registry, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem, const Localization::LocalizationModule& localizationModule)
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
                Ui::TreeItem root = BuildComponentTree(s_SearchString, registry, sceneId, gameCoreSystem, localizationModule);
                Ui::RenderTreeView("componentMenu", root, flags);
            }

            ImGui::EndChild();

            ImGui::EndPopup();
        }
    }

    Ui::TreeItem ComponentsPanel::BuildComponentTree(StringView searchString, Entity::EntityRegistry& registry, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem, const Localization::LocalizationModule& localizationModule) const
    {
        Ui::TreeItem root;
        const Entity::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();
        for (auto&& [componentTypeId, componentMeta] : componentFactory.GetComponentMeta())
        {
            if ( componentMeta->IsTransient() || (componentMeta->IsCodeOnly() ) )
            {
                continue;
            }

            Localization::LocalizationId localizationId(componentTypeId);
            Localization::LocalizedString localizedString = localizationModule.GetLocalized(localizationId);
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
                            auto selectedEntities = registry.GetView<SelectedComponent>();
                            auto size = selectedEntities.size();
                            ONYX_UNUSED(size);
                            for (Entity::EntityId selectedEntity : selectedEntities)
                            {
                                m_CommandGraph->Push(AddComponentCommand(selectedEntity, componentTypeId, sceneId, gameCoreSystem));
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
