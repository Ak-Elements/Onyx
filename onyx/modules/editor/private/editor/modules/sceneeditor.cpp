#include <editor/modules/sceneeditor.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entity.h>
#include <onyx/gamecore/components/cameracomponent.h>
#include <onyx/gamecore/components/freecameracomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/components/transientcomponent.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/gamecore/serialize/sceneserializer.h>

#include <editor/panels/sceneeditor/componentspanel.h>
#include <editor/panels/sceneeditor/entitiespanel.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>
#include <editor/windows/editormainwindow.h>

namespace Onyx::Editor
{
    namespace 
    {
        onyxU32 local_WindowId = 0;
    }

    SceneEditorWindow::SceneEditorWindow(GameCore::GameCoreSystem& gameCore, Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, Input::InputActionSystem& inputActionSystem)
        : gameCore(gameCore)
        , api(graphicsApi)
        , inputActionSystem(inputActionSystem)
        , assetSystem(&assetSystem)
        , windowId(local_WindowId++)
    {
        windowClass = new ImGuiWindowClass();
        windowClass->DockingAllowUnclassed = false;
    }

    SceneEditorWindow::~SceneEditorWindow() = default;

    void SceneEditorWindow::OnOpen()
    {
        SetName(Format::Format("Scene Editor###SceneEditor{}", windowId));
        windowClass->ClassId = ImGui::GetID("scene");

        sceneViewPanelId = Format::Format("###SceneViewPanel{}", windowId);
        entitiesPanelId = Format::Format("###EntitiesPanel{}", windowId);
        componentsPanelId = Format::Format("###ComponentsPanel{}", windowId);

        ImGuiID dockspaceID = ImGui::GetID(Format::Format("SceneEditorDockspace{}", windowId));
        dockspace = Ui::Dockspace::Create({
          {
              Ui::DockSplitDirection::Right, 0.8f, "", entitiesPanelId
          },
          {
              Ui::DockSplitDirection::Right, 0.2f, componentsPanelId, sceneViewPanelId
          }
            });
        dockspace.SetId(dockspaceID);
        dockspace.SetWindowClass(*windowClass);

        
        inputActionSystem.SetCurrentInputActionMap(Hash::FNV1aHash32("sceneeditor"));

        if (scene.IsValid() == false)
        {
            StringView startupLevel;// = "project:/scenes/TestScene/scene.oscene";

            if (startupLevel.empty())
            {
                scene = assetSystem->Create<GameCore::Scene>();
                OnSceneLoaded(scene);
            }
            else
            {
                LoadScene(startupLevel);
            }
        }
    }

    void SceneEditorWindow::OnClose()
    {
        inputActionSystem.Disconnect(this);
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

        Optional<EditorMainWindow*> mainWindowOptional = system.GetWindow<EditorMainWindow>();
        if (mainWindowOptional.has_value())
        {
            EditorMainWindow& mainWindow = *mainWindowOptional.value();
            ImGui::SetNextWindowDockID(mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver);
        }
        
        if (Begin())
        {
            dockspace.Render();

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
            dockspace.Render();
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
    
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene"))
            {
                FileSystem::Filepath path;
                if (FileSystem::FileDialog::SaveFileDialog(path, "Scene", GameCore::SceneSerializer::Extensions))
                {
                    assetSystem->SaveAssetAs(path, scene);
                }
            }

            if (ImGui::MenuItem("Open Scene"))
            {
                isLoading = true;
                if (scene.IsValid())
                {
                    scene->GetOnLoadedEvent().Disconnect(this);
                    inputActionSystem.Disconnect(this);
                }

                FileSystem::Filepath path;
                if (FileSystem::FileDialog::OpenFileDialog(path, "Scene", GameCore::SceneSerializer::Extensions))
                {
                    LoadScene(Assets::AssetId(path));
                }
            }

            ImGui::EndMenu();
        }

        EndMenuBar();
    }

    void SceneEditorWindow::RenderSceneViewport()
    {
        // TODO TEMP: expose final pin to the outside
        const Graphics::TextureHandle& finalSceneTexture  = api.GetRenderGraph()->GetFinalTexture();

        const Graphics::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
        ImVec2 sceneTextureExtents = { static_cast<onyxF32>(sceneTextureProperties.m_Size[0]), static_cast<onyxF32>(sceneTextureProperties.m_Size[1]) };

        ImGui::SetNextWindowClass(windowClass);
        //ImGui::SetNextWindowDockID(dockspace);
        if (ImGui::Begin(Format::Format("Scene{}", sceneViewPanelId)))
        {
            ImGui::Image(finalSceneTexture.Texture->GetIndex(), sceneTextureExtents);

            RenderImGuizmo(Vector2f(static_cast<onyxF32>(sceneTextureProperties.m_Size[0]), static_cast<onyxF32>(sceneTextureProperties.m_Size[1])));
        }

        ImGui::End();
    }

    void SceneEditorWindow::RenderEntitiesPanel()
    {
        ImGui::SetNextWindowClass(windowClass);
        if (ImGui::Begin(Format::Format("Entities{}", entitiesPanelId)))
        {
            entitiesPanel.Render(*scene);
        }

        ImGui::End();
    }

    void SceneEditorWindow::RenderComponentsPanel()
    {
        ImGui::SetNextWindowClass(windowClass);
        if (ImGui::Begin(Format::Format("Components{}", componentsPanelId), nullptr, ImGuiWindowFlags_HorizontalScrollbar))
        {
            componentsPanel.Render(*scene);
        }
        ImGui::End();
    }

    void SceneEditorWindow::RenderImGuizmo(const Vector2f& viewportExtents)
    {
        Entity::EntityRegistry& registry = scene->GetRegistry();
        auto selectedEntitesView = registry.GetView<SelectedComponent>();

        hasSelectedEntity = false;
        if (selectedEntitesView.empty() == false)
        {
            Entity::EntityId selectedEntity = *selectedEntitesView.begin();
            // draw gizmos
            hasSelectedEntity = selectedEntity != entt::null;
            if (hasSelectedEntity)
            {
                GameCore::TransformComponent& transformComponent = registry.GetComponent<GameCore::TransformComponent>(selectedEntity);

                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();

                const ImVec2& windowPosition = ImGui::GetWindowPos();
                //const ImVec2& windowSize = ImGui::GetWindowSize();
                ImGuizmo::SetRect(windowPosition.x, windowPosition.y, viewportExtents[0], viewportExtents[1]);

                GameCore::CameraComponent& editorCamera = registry.GetComponent<GameCore::CameraComponent>(editorCameraEntity);

                const Matrix4<onyxF32>& projectionMatrix = editorCamera.Camera.GetProjectionMatrix();
                const Matrix4<onyxF32>& viewMatrix = editorCamera.Camera.GetViewMatrix();

                Matrix4<onyxF32> transformMatrix = transformComponent.GetTransform();

                ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                if (currentGizmo == GizmoType::Rotate)
                    operation = ImGuizmo::ROTATE;
                else if (currentGizmo == GizmoType::Scale)
                    operation = ImGuizmo::SCALE;
                
                if (ImGuizmo::Manipulate(&(viewMatrix[0][0]), &(projectionMatrix[0][0]), operation, ImGuizmo::LOCAL, &(transformMatrix[0][0])))
                {
                    Vector3f translation, rotation, scale;
                    Rotor3<onyxF32> rotationRotor;
                    transformMatrix.Decompose(translation, rotationRotor, scale);

                    switch (currentGizmo)
                    {
                        case GizmoType::Translate:
                        {
                            transformComponent.SetTranslation(translation);
                            break;
                        }
                        case GizmoType::Rotate:
                        {
                            constexpr onyxF32 PI = std::numbers::pi_v<onyxF32>;
                            constexpr onyxF32 TWO_PI = 2.0f * PI;
                            Vector3f originalRotation = transformComponent.GetRotationEuler();

                            originalRotation[0] = std::fmod(originalRotation[0] + PI, TWO_PI) - PI;
                            originalRotation[1] = std::fmod(originalRotation[1] + PI, TWO_PI) - PI;
                            originalRotation[2] = std::fmod(originalRotation[2] + PI, TWO_PI) - PI;

                            Vector3f deltaRotation = rotationRotor.ToEulerAngles() - originalRotation;

                            if (IsZero(deltaRotation[0], 0.001f))
                                deltaRotation[0] = 0.0f;
                            if (IsZero(deltaRotation[1], 0.001f))
                                deltaRotation[1] = 0.0f;
                            if (IsZero(deltaRotation[2], 0.001f))
                                deltaRotation[2] = 0.0f;

                            transformComponent.Rotate(deltaRotation);
                            break;
                        }
                        case GizmoType::Scale:
                        {
                            transformComponent.SetScale(scale);
                            break;
                        }
                    }
                }
            }
        }
    }

    void SceneEditorWindow::OnGizmoModeAction(const Input::InputActionEvent& inputActionContext)
    {
        if (hasSelectedEntity == false)
            return;
        
        constexpr onyxU64 GIZMO_TRANSLATE_ACTION_ID = Hash::FNV1aHash64("GizmoTranslate");
        constexpr onyxU64 GIZMO_ROTATE_ACTION_ID = Hash::FNV1aHash64("GizmoRotate");

        if (inputActionContext.GetId() == GIZMO_TRANSLATE_ACTION_ID)
        {
            currentGizmo = GizmoType::Translate;
            return;
        }
        else if (inputActionContext.GetId() == GIZMO_ROTATE_ACTION_ID)
        {
            currentGizmo = GizmoType::Rotate;
            return;
        }
        
        currentGizmo = GizmoType::Scale;
    }

    void SceneEditorWindow::OnCameraMoveInput(const Input::InputActionEvent& inputActionContext)
    {
        const Vector3f& direction = inputActionContext.GetData<Vector3f>();
        
        Entity::EntityRegistry& registry = scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(editorCameraEntity);
        cameraRuntimeComponent.InputDirection = direction;
    }

    void SceneEditorWindow::OnCameraRotationInput(const Input::InputActionEvent& inputActionContext)
    {
        const Vector2f& rotationDelta = inputActionContext.GetData<Vector2f>();

        Entity::EntityRegistry& registry = scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(editorCameraEntity);
        cameraRuntimeComponent.InputRotation = { rotationDelta[0] * 0.003f, rotationDelta[1] * 0.003f, 0.0f };
    }

    void SceneEditorWindow::OnCameraSpeedInput(const Input::InputActionEvent& inputActionContext)
    {
        onyxF32 speedValue = inputActionContext.GetData<onyxF32>();

        Entity::EntityRegistry& registry = scene->GetRegistry();
        GameCore::FreeCameraControllerComponent& cameraControllerComponent = registry.GetComponent<GameCore::FreeCameraControllerComponent>(editorCameraEntity);
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(editorCameraEntity);

        cameraRuntimeComponent.Velocity += speedValue * cameraControllerComponent.VelocityIncrementFactor * cameraControllerComponent.BaseVelocity;
    }

    void SceneEditorWindow::OnCameraSpeedUp(const Input::InputActionEvent& inputActionContext)
    {
        bool isSpeedUp = inputActionContext.GetData<bool>();

        Entity::EntityRegistry& registry = scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(editorCameraEntity);

        if (isSpeedUp)
            cameraRuntimeComponent.Velocity *= 10.0f;
        else
            cameraRuntimeComponent.Velocity *= 0.1f;
    }

    void SceneEditorWindow::OnCameraSlowDown(const Input::InputActionEvent& inputActionContext)
    {
        bool isSlowdown = inputActionContext.GetData<bool>();

        Entity::EntityRegistry& registry = scene->GetRegistry();
        GameCore::FreeCameraRuntimeComponent& cameraRuntimeComponent = registry.GetComponent<GameCore::FreeCameraRuntimeComponent>(editorCameraEntity);

        if (isSlowdown)
            cameraRuntimeComponent.Velocity *= 0.1f;
        else
            cameraRuntimeComponent.Velocity *= 10.0f;
    }

    void SceneEditorWindow::LoadScene(Assets::AssetId sceneAssetId)
    {
        Reference<GameCore::Scene> newScene;
        assetSystem->GetAssetUnmanaged(sceneAssetId, newScene);
        newScene->GetOnLoadedEvent().Connect<&SceneEditorWindow::OnSceneLoaded>(this);
    }

    void SceneEditorWindow::OnSceneLoaded(const Reference<GameCore::Scene>& sceneAsset)
    {
        scene = sceneAsset;
        gameCore.SetScene(scene);

        Entity::EntityRegistry& registry = scene->GetRegistry();
        editorCameraEntity = registry.CreateEntity();

        registry.AddComponent<GameCore::TransientComponent>(editorCameraEntity);
        GameCore::TransformComponent& transform = registry.AddComponent<GameCore::TransformComponent>(editorCameraEntity);
        transform.SetTranslation(Vector3f{ 0.0f, 5.0f, 20.0f });
        transform.SetRotation(Vector3f(0, 0, 0));
        GameCore::CameraComponent& camera = registry.AddComponent<GameCore::CameraComponent>(editorCameraEntity);

        camera.Camera.SetPerspective(45.0f, 0.1f, 1000.0f);
        camera.Camera.SetViewportExtents(api.GetSwapchainExtent());
        registry.AddComponent<GameCore::FreeCameraControllerComponent>(editorCameraEntity);

        api.SetCamera(camera.Camera);

        inputActionSystem.OnInput<&SceneEditorWindow::OnCameraMoveInput>(Hash::FNV1aHash64("CameraMovement"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnCameraRotationInput>(Hash::FNV1aHash64("CameraRotation"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnCameraSpeedInput>(Hash::FNV1aHash64("CameraSpeed"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnCameraSlowDown>(Hash::FNV1aHash64("CameraSlowDown"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnCameraSpeedUp>(Hash::FNV1aHash64("CameraSpeedUp"), this);

        inputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>(Hash::FNV1aHash64("GizmoTranslate"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>(Hash::FNV1aHash64("GizmoRotate"), this);
        inputActionSystem.OnInput<&SceneEditorWindow::OnGizmoModeAction>(Hash::FNV1aHash64("GizmoScale"), this);
    }
}
