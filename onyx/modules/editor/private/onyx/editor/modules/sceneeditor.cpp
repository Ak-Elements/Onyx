#include <onyx/editor/modules/sceneeditor.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entity.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/components/transientcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/localization/localization.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/gamecore/serialize/sceneserializer.h>

#include <onyx/editor/panels/sceneeditor/componentspanel.h>
#include <onyx/editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/editor/windows/editormainwindow.h>
#include <onyx/editor/editor_localization.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

namespace Onyx::Editor
{
    namespace 
    {
        onyxU32 local_WindowId = 0;
    }

    SceneEditorWindow::SceneEditorWindow(GameCore::GameCoreSystem& gameCore, Assets::AssetSystem& assetSystem, Localization::LocalizationModule& localizationModule, Graphics::GraphicsSystem& graphicsSystem, InputActions::InputActionSystem& inputActionSystem)
        : m_GameCore(gameCore)
        , m_GraphicsSystem(graphicsSystem)
        , m_InputActionSystem(inputActionSystem)
        , m_AssetSystem(&assetSystem)
        , m_ComponentsPanel(localizationModule)
        , m_TerrainPanel(inputActionSystem, graphicsSystem, gameCore)
        , m_WindowId(local_WindowId++)
    {
        m_WindowClass = new ImGuiWindowClass();
        m_WindowClass->DockingAllowUnclassed = false;
    }

    SceneEditorWindow::~SceneEditorWindow() = default;

    void SceneEditorWindow::OnOpen()
    {
        SetName(Format::Format("{}###SceneEditor{}", Localization::Editor::SceneEditor::Title, m_WindowId));
        m_WindowClass->ClassId = ImGui::GetID("scene");

        m_SceneViewPanelId = Format::Format("###SceneViewPanel{}", m_WindowId);
        m_EntitiesPanelId = Format::Format("###EntitiesPanel{}", m_WindowId);
        m_ComponentsPanelId = Format::Format("###ComponentsPanel{}", m_WindowId);

        ImGuiID dockspaceID = ImGui::GetID(Format::Format("SceneEditorDockspace{}", m_WindowId));

        // TODO: should those be somewhere defined as default sizes? And should be based on window size not main view port
        float windowWidth = ImGui::GetMainViewport()->Size.x;
        float compPanelRatio = 512.0f / windowWidth;
        float entitiesPanelRatio = 256.0f / windowWidth;
        
        m_Dockspace = Ui::Dockspace::Create({
          {
              Ui::DockSplitDirection::Right, 1.0f - entitiesPanelRatio, "", m_EntitiesPanelId
          },
          {
              Ui::DockSplitDirection::Right, compPanelRatio, m_ComponentsPanelId, m_SceneViewPanelId
          }
            });
        m_Dockspace.SetId(dockspaceID);
        m_Dockspace.SetWindowClass(*m_WindowClass);

        m_InputActionSystem.SetCurrentInputActionMap(StringId32("sceneeditor"));

        if (m_Scene.IsValid() == false)
        {
            StringView startupLevel = "";

            if (startupLevel.empty())
            {
                m_Scene = m_AssetSystem->Create<GameCore::Scene>();
                m_AssetSystem->GetAsset("engine:/rendergraphs/default.orendergraph", m_Scene->GetRenderGraphRef());
                OnSceneLoaded(m_Scene);
            }
            else
            {
                LoadScene(startupLevel);
            }
        }

    }

    void SceneEditorWindow::OnClose()
    {
        m_InputActionSystem.Disconnect(this);
    }

    void SceneEditorWindow::OnRender(Ui::ImGuiSystem& system)
    {
        if (IsLoading())
            return;

        if (IsDocked())
        {
            SetWindowFlags(ImGuiWindowFlags_None);
        }
        else
        {
            SetWindowFlags(ImGuiWindowFlags_MenuBar);
        }

        m_GameCore.SetScene(m_Scene);

        Optional<EditorMainWindow*> mainWindowOptional = system.GetWindow<EditorMainWindow>();
        if (mainWindowOptional.has_value())
        {
            EditorMainWindow& mainWindow = *mainWindowOptional.value();
            ImGui::SetNextWindowDockID(mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver);
        }
        
        if (Begin())
        {
            m_Dockspace.Render();

            RenderMenuBar();

            if (ImGui::IsWindowAppearing())
            {
                ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
            }

            if (IsLoading())
                return;

            RenderSceneViewport();
            RenderEntitiesPanel();
            RenderComponentsPanel();
        }
        else
        {
            m_Dockspace.Render();
        }

        End();

            //ImGui::End();

        //const Graphics::TextureStorageProperties& sceneTextureProperties = sceneTextureHandle.Storage->GetProperties();
        //const ImVec2 sceneTextureExtents{ static_cast<onyxF32>(sceneTextureProperties.m_Size[0]), static_cast<onyxF32>(sceneTextureProperties.m_Size[1]) };

        /*if ((static_cast<onyxS32>(windowSize.x) != sceneTextureProperties.m_Size[0]) || (static_cast<onyxS32>(windowSize.y) != sceneTextureProperties.m_Size[1]))

        {
            Entity::CameraComponent& cameraComponent = m_Registry.AddComponent<Entity::CameraComponent>(editorCameraEntity);
            cameraComponent.Camera.SetViewportSize( { static_cast<onyxS32>(windowSize.x), static_cast<onyxS32>(windowSize.y) } );

            Graphics::TextureStorageProperties newProperties = sceneTextureProperties;
            newProperties.m_Size[0] = static_cast<onyxS32>(windowSize.x);
            newProperties.m_Size[1] = static_cast<onyxS32>(windowSize.y);

            m_Api.CreateTexture(newProperties, sceneTextureHandle.Texture->GetProperties(), sceneTextureHandle);
        }*/
        
    }

    void SceneEditorWindow::RenderMenuBar()
    {
        BeginMenuBar();
    
        if (ImGui::BeginMenu(Format::Format("{}###File", Localization::Generic::File)))
        {
            if (ImGui::MenuItem(Localization::Generic::Open.Get().data()))
            {
                m_IsLoading = true;
                if (m_Scene.IsValid())
                {
                    m_Scene->GetOnLoadedEvent().Disconnect(this);
                    m_InputActionSystem.Disconnect(this);
                }

                FilePath path;
                if (FileSystem::FileDialog::OpenFileDialog(path, "Scene", GameCore::SceneSerializer::Extensions))
                {
                    LoadScene(Assets::AssetId(path));
                }
            }

            if (ImGui::MenuItem(Localization::Generic::Save.Get().data()))
            {
                m_AssetSystem->SaveAsset(m_Scene);
            }

            if (ImGui::MenuItem(Localization::Generic::SaveAs.Get().data()))
            {
                FilePath path;
                if (FileSystem::FileDialog::SaveFileDialog(path, "Scene", GameCore::SceneSerializer::Extensions))
                {
                    m_AssetSystem->SaveAssetAs(path, m_Scene);
                }
            }
            
            ImGui::EndMenu();
        }

        EndMenuBar();
    }

    void SceneEditorWindow::RenderSceneViewport()
    {
        // TODO TEMP: expose final pin to the outside
        if (m_Scene->HasRenderGraph() == false)
            return;

        const Graphics::TextureHandle finalSceneTexture = m_Scene->GetRenderGraph().GetFinalTexture();
        //if (finalSceneTexture.IsValid() == false)
        //{
        //    return;
        //}

        const Graphics::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
        ImVec2 sceneTextureExtents = { static_cast<onyxF32>(sceneTextureProperties.m_Size[0]), static_cast<onyxF32>(sceneTextureProperties.m_Size[1]) };

        ImGui::SetNextWindowClass(m_WindowClass);
        //ImGui::SetNextWindowDockID(dockspace);
        m_TerrainPanel.SetSceneViewPanelId(ImGui::GetID(m_SceneViewPanelId.c_str()));
        if (ImGui::Begin(Format::Format("{}{}", Localization::Editor::SceneEditor::SceneViewport, m_SceneViewPanelId)))
        {
            m_ViewportBounds.Position = { static_cast<onyxS16>(ImGui::GetCursorPos().x), static_cast<onyxS16>(ImGui::GetCursorPos().y) };
            m_ViewportBounds.Extents = { static_cast<onyxS16>(sceneTextureProperties.m_Size[0]), static_cast<onyxS16>(sceneTextureProperties.m_Size[1]) };// = Vector2s16{ static_cast<onyxS16>(ImGui::GetContentRegionAvail().x), static_cast<onyxS16>(ImGui::GetContentRegionAvail().y) } - m_ViewportBounds.Position;

            ImGui::SetNextItemAllowOverlap();
            ImGui::Image(finalSceneTexture.Texture->GetIndex(), sceneTextureExtents);

            RenderImGuizmo(Vector2f32(static_cast<onyxF32>(sceneTextureProperties.m_Size[0]), static_cast<onyxF32>(sceneTextureProperties.m_Size[1])));
        }

        RenderTerrainPanel();

        ImGui::End();
    }

    void SceneEditorWindow::RenderEntitiesPanel()
    {
        ImGui::SetNextWindowClass(m_WindowClass);
        if (ImGui::Begin(Format::Format("{}{}", Localization::Editor::EntitiesPanel::Title, m_EntitiesPanelId)))
        {
            m_EntitiesPanel.Render(*m_Scene);
        }

        ImGui::End();
    }

    void SceneEditorWindow::RenderComponentsPanel()
    {
        ImGui::SetNextWindowClass(m_WindowClass);
        if (ImGui::Begin(Format::Format("{}{}", Localization::Editor::ComponentsPanel::Title, m_ComponentsPanelId), nullptr, ImGuiWindowFlags_HorizontalScrollbar))
        {
            m_ComponentsPanel.Render(m_GameCore.GetComponentFactory(), *m_Scene);
        }
        ImGui::End();
    }

    void SceneEditorWindow::RenderTerrainPanel()
    {
        m_TerrainPanel.Render(*m_Scene);
    }

    void SceneEditorWindow::RenderImGuizmo(const Vector2f32& viewportExtents)
    {
        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        auto selectedEntitesView = registry.GetView<SelectedComponent>();

        m_HasSelectedEntity = false;
        if (selectedEntitesView.empty() == false)
        {
            Entity::EntityId selectedEntity = *selectedEntitesView.begin();
            // draw gizmos
            m_HasSelectedEntity = selectedEntity != entt::null;
            if (m_HasSelectedEntity)
            {
                GameCore::TransformComponent& transformComponent = registry.GetComponent<GameCore::TransformComponent>(selectedEntity);

                ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();

                const ImVec2& windowPosition = ImGui::GetWindowPos();
                //const ImVec2& windowSize = ImGui::GetWindowSize();
                ImGuizmo::SetRect(windowPosition.x, windowPosition.y, viewportExtents[0], viewportExtents[1]);

                GameCore::CameraComponent& editorCamera = registry.GetComponent<GameCore::CameraComponent>(m_EditorCameraEntity);

                const Matrix4<onyxF32>& projectionMatrix = editorCamera.Camera.GetProjectionMatrix();
                const Matrix4<onyxF32>& viewMatrix = editorCamera.Camera.GetViewMatrix();

                Matrix4<onyxF32> transformMatrix = GameCore::WorldTransform::GetTransform(transformComponent);

                ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                if (m_CurrentGizmo == GizmoType::Rotate)
                    operation = ImGuizmo::ROTATE;
                else if (m_CurrentGizmo == GizmoType::Scale)
                    operation = ImGuizmo::SCALE;
                
                if (ImGuizmo::Manipulate(&(viewMatrix[0][0]), &(projectionMatrix[0][0]), operation, ImGuizmo::LOCAL, &(transformMatrix[0][0])))
                {
                    Vector3f32 translation, scale;
                    Rotor3<onyxF32> rotationRotor;
                    transformMatrix.Decompose(translation, rotationRotor, scale);

                    switch (m_CurrentGizmo)
                    {
                        case GizmoType::Translate:
                        {
                            transformComponent.Translation = translation;
                            break;
                        }
                        case GizmoType::Rotate:
                        {
                            constexpr onyxF32 PI = std::numbers::pi_v<onyxF32>;
                            constexpr onyxF32 TWO_PI = 2.0f * PI;
                            Vector3f32 originalRotation = transformComponent.RotationEuler;

                            originalRotation[0] = std::fmod(originalRotation[0] + PI, TWO_PI) - PI;
                            originalRotation[1] = std::fmod(originalRotation[1] + PI, TWO_PI) - PI;
                            originalRotation[2] = std::fmod(originalRotation[2] + PI, TWO_PI) - PI;

                            Vector3f32 deltaRotation = rotationRotor.ToEulerAngles() - originalRotation;

                            if (IsZero(deltaRotation[0], 0.001f))
                                deltaRotation[0] = 0.0f;
                            if (IsZero(deltaRotation[1], 0.001f))
                                deltaRotation[1] = 0.0f;
                            if (IsZero(deltaRotation[2], 0.001f))
                                deltaRotation[2] = 0.0f;
                            
                            GameCore::WorldTransform::Rotate(transformComponent, deltaRotation);
                            break;
                        }
                        case GizmoType::Scale:
                        {
                            transformComponent.Scale = scale;
                            break;
                        }
                    }
                }
            }
        }
    }

    void SceneEditorWindow::OnGizmoModeAction(const InputActions::InputActionEvent& inputActionContext)
    {
        if (m_HasSelectedEntity == false)
            return;
        
        constexpr StringId64 GIZMO_TRANSLATE_ACTION_ID("GizmoTranslate");
        constexpr StringId64 GIZMO_ROTATE_ACTION_ID("GizmoRotate");
        
        if (inputActionContext.GetId() == GIZMO_TRANSLATE_ACTION_ID)
        {
            m_CurrentGizmo = GizmoType::Translate;
            return;
        }
        else if (inputActionContext.GetId() == GIZMO_ROTATE_ACTION_ID)
        {
            m_CurrentGizmo = GizmoType::Rotate;
            return;
        }
        
        m_CurrentGizmo = GizmoType::Scale;
    }

    void SceneEditorWindow::OnCameraMoveInput(const InputActions::InputActionEvent& inputActionContext)
    {
        const Vector3f32& direction = inputActionContext.GetData<Vector3f32>();
        
        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);
        cameraRuntimeComponent.InputDirection = direction;
    }

    void SceneEditorWindow::OnCameraRotationInput(const InputActions::InputActionEvent& inputActionContext)
    {
        const Vector2f32& rotationDelta = inputActionContext.GetData<Vector2f32>();

        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);
        cameraRuntimeComponent.InputRotation = { rotationDelta[0] * 0.003f, rotationDelta[1] * 0.003f, 0.0f };
    }

    void SceneEditorWindow::OnCameraSpeedInput(const InputActions::InputActionEvent& inputActionContext)
    {
        onyxF32 speedValue = inputActionContext.GetData<onyxF32>();

        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        GameCore::FreeCameraControllerComponent& cameraControllerComponent = registry.GetComponent<GameCore::FreeCameraControllerComponent>(m_EditorCameraEntity);
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);

        cameraRuntimeComponent.Velocity += speedValue * cameraControllerComponent.VelocityIncrementFactor * cameraControllerComponent.BaseVelocity;
    }

    void SceneEditorWindow::OnCameraSpeedUp(const InputActions::InputActionEvent& inputActionContext)
    {
        bool isSpeedUp = inputActionContext.GetData<bool>();

        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);

        if (isSpeedUp)
            cameraRuntimeComponent.Velocity *= 10.0f;
        else
            cameraRuntimeComponent.Velocity *= 0.1f;
    }

    void SceneEditorWindow::OnCameraSlowDown(const InputActions::InputActionEvent& inputActionContext)
    {
        bool isSlowdown = inputActionContext.GetData<bool>();

        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);

        if (isSlowdown)
            cameraRuntimeComponent.Velocity *= 0.1f;
        else
            cameraRuntimeComponent.Velocity *= 10.0f;
    }

    void SceneEditorWindow::LoadScene(Assets::AssetId sceneAssetId)
    {
        Reference<GameCore::Scene> newScene;
        m_AssetSystem->GetAssetUnmanaged(sceneAssetId, newScene);
        newScene->GetOnLoadedEvent().Connect<&SceneEditorWindow::OnSceneLoaded>(this);
    }

    void SceneEditorWindow::OnSceneLoaded(const Reference<GameCore::Scene>& sceneAsset)
    {
        m_Scene = sceneAsset;
        
        Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        m_EditorCameraEntity = registry.CreateEntity();

        registry.AddComponent<GameCore::TransientComponent>(m_EditorCameraEntity);
        GameCore::TransformComponent& transform = registry.AddComponent<GameCore::TransformComponent>(m_EditorCameraEntity);
        transform.Translation = Vector3f32{ 0.0f, 100.0f, 1000.0f };
        transform.RotationEuler = Vector3f32(0, 0, 0);
        GameCore::CameraComponent& camera = registry.AddComponent<GameCore::CameraComponent>(m_EditorCameraEntity);

        camera.Camera.SetPerspective(45.0f, 0.1f, 65536);
        camera.Camera.SetViewportExtents(m_GraphicsSystem.GetSwapchainExtent());
        const GameCore::FreeCameraControllerComponent& freeCameraController = registry.AddComponent<GameCore::FreeCameraControllerComponent>(m_EditorCameraEntity);
        GameCore::FreeCameraRuntimeComponent& runtimeComponent = registry.AddComponent<GameCore::FreeCameraRuntimeComponent>(m_EditorCameraEntity);
        runtimeComponent.Velocity = freeCameraController.BaseVelocity;

        m_GraphicsSystem.SetCamera(camera.Camera);

        m_InputActionSystem.OnInput<&SceneEditorWindow::OnCameraMoveInput>("CameraMovement"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnCameraRotationInput>("CameraRotation"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnCameraSpeedInput>("CameraSpeed"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnCameraSlowDown>("CameraSlowDown"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnCameraSpeedUp>("CameraSpeedUp"_id64, this);

        m_InputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>("GizmoTranslate"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>("GizmoRotate"_id64, this);
        m_InputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>("GizmoScale"_id64, this);
    }
}
