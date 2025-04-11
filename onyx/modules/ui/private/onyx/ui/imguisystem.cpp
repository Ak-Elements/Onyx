#include <onyx/ui/imguisystem.h>

#if ONYX_USE_IMGUI

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/window.h>

#include <onyx/input/inputsystem.h>
#include <onyx/input/inputevent.h>
#include <onyx/ui/imguinotify.h>
#include <onyx/ui/imguiwindow.h>

#include <onyx/ui/propertygrid.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>

#include <ImGuizmo.h>
#include <implot.h>
#include <implot3d.h>

#include <windows.h>

namespace Onyx::Ui
{
	Reference<Graphics::TextureAsset> ImGuiSystem::FolderClosedAsset = {};
	Reference<Graphics::TextureAsset> ImGuiSystem::FolderOpenAsset = {};
	Reference<Graphics::TextureAsset> ImGuiSystem::FolderSelectedClosedAsset = {};
	Reference<Graphics::TextureAsset> ImGuiSystem::FolderSelectedOpenAsset = {};

	namespace Internal
	{
		bool loc_ReloadLayout = false;
		bool loc_SaveLayout = false;

	    ImGuiKey ConvertToImGuiKey(Input::Key key)
	    {
			switch (key)
			{
			case Input::Key::Tab: return ImGuiKey_Tab;
			case Input::Key::Left: return ImGuiKey_LeftArrow;
			case Input::Key::Right: return ImGuiKey_RightArrow;
			case Input::Key::Up: return ImGuiKey_UpArrow;
			case Input::Key::Down: return ImGuiKey_DownArrow;
			case Input::Key::PageUp: return ImGuiKey_PageUp;
			case Input::Key::PageDown: return ImGuiKey_PageDown;
			case Input::Key::Home: return ImGuiKey_Home;
			case Input::Key::End: return ImGuiKey_End;
			case Input::Key::Insert: return ImGuiKey_Insert;
			case Input::Key::Delete: return ImGuiKey_Delete;
			case Input::Key::Backspace: return ImGuiKey_Backspace;
			case Input::Key::Space: return ImGuiKey_Space;
			case Input::Key::Enter: return ImGuiKey_Enter;
			case Input::Key::Escape: return ImGuiKey_Escape;
			case Input::Key::Apostrophe: return ImGuiKey_Apostrophe;
			case Input::Key::Comma: return ImGuiKey_Comma;
			case Input::Key::Minus: return ImGuiKey_Minus;
			case Input::Key::Period: return ImGuiKey_Period;
			case Input::Key::Slash: return ImGuiKey_Slash;
			case Input::Key::Semicolon: return ImGuiKey_Semicolon;
			case Input::Key::Equals: return ImGuiKey_Equal;
			case Input::Key::LeftBracket: return ImGuiKey_LeftBracket;
			case Input::Key::BackSlash: return ImGuiKey_Backslash;
			case Input::Key::RightBracket: return ImGuiKey_RightBracket;
			case Input::Key::Grave: return ImGuiKey_GraveAccent;
			case Input::Key::CapsLock: return ImGuiKey_CapsLock;
			case Input::Key::ScrollLock: return ImGuiKey_ScrollLock;
			case Input::Key::NumPad_Lock_Clear: return ImGuiKey_NumLock;
			case Input::Key::PrintScreen: return ImGuiKey_PrintScreen;
			case Input::Key::Pause: return ImGuiKey_Pause;
			case Input::Key::NumPad_0: return ImGuiKey_Keypad0;
			case Input::Key::NumPad_1: return ImGuiKey_Keypad1;
			case Input::Key::NumPad_2: return ImGuiKey_Keypad2;
			case Input::Key::NumPad_3: return ImGuiKey_Keypad3;
			case Input::Key::NumPad_4: return ImGuiKey_Keypad4;
			case Input::Key::NumPad_5: return ImGuiKey_Keypad5;
			case Input::Key::NumPad_6: return ImGuiKey_Keypad6;
			case Input::Key::NumPad_7: return ImGuiKey_Keypad7;
			case Input::Key::NumPad_8: return ImGuiKey_Keypad8;
			case Input::Key::NumPad_9: return ImGuiKey_Keypad9;
			case Input::Key::NumPad_Period: return ImGuiKey_KeypadDecimal;
			case Input::Key::NumPad_Divide: return ImGuiKey_KeypadDivide;
			case Input::Key::NumPad_Multiply: return ImGuiKey_KeypadMultiply;
			case Input::Key::NumPad_Minus: return ImGuiKey_KeypadSubtract;
			case Input::Key::NumPad_Plus: return ImGuiKey_KeypadAdd;
			case Input::Key::NumPad_Enter: return ImGuiKey_KeypadEnter;
			case Input::Key::NumPad_Equals: return ImGuiKey_KeypadEqual;
			case Input::Key::Left_Ctrl: return ImGuiKey_LeftCtrl;
			case Input::Key::Left_Shift: return ImGuiKey_LeftShift;
			case Input::Key::Left_Alt: return ImGuiKey_LeftAlt;
			case Input::Key::Left_System: return ImGuiKey_LeftSuper;
			case Input::Key::Right_Ctrl: return ImGuiKey_RightCtrl;
			case Input::Key::Right_Shift: return ImGuiKey_RightShift;
			case Input::Key::Right_Alt: return ImGuiKey_RightAlt;
			case Input::Key::Right_System: return ImGuiKey_RightSuper;
			case Input::Key::Menu: return ImGuiKey_Menu;
			case Input::Key::Key_0: return ImGuiKey_0;
			case Input::Key::Key_1: return ImGuiKey_1;
			case Input::Key::Key_2: return ImGuiKey_2;
			case Input::Key::Key_3: return ImGuiKey_3;
			case Input::Key::Key_4: return ImGuiKey_4;
			case Input::Key::Key_5: return ImGuiKey_5;
			case Input::Key::Key_6: return ImGuiKey_6;
			case Input::Key::Key_7: return ImGuiKey_7;
			case Input::Key::Key_8: return ImGuiKey_8;
			case Input::Key::Key_9: return ImGuiKey_9;
			case Input::Key::A: return ImGuiKey_A;
			case Input::Key::B: return ImGuiKey_B;
			case Input::Key::C: return ImGuiKey_C;
			case Input::Key::D: return ImGuiKey_D;
			case Input::Key::E: return ImGuiKey_E;
			case Input::Key::F: return ImGuiKey_F;
			case Input::Key::G: return ImGuiKey_G;
			case Input::Key::H: return ImGuiKey_H;
			case Input::Key::I: return ImGuiKey_I;
			case Input::Key::J: return ImGuiKey_J;
			case Input::Key::K: return ImGuiKey_K;
			case Input::Key::L: return ImGuiKey_L;
			case Input::Key::M: return ImGuiKey_M;
			case Input::Key::N: return ImGuiKey_N;
			case Input::Key::O: return ImGuiKey_O;
			case Input::Key::P: return ImGuiKey_P;
			case Input::Key::Q: return ImGuiKey_Q;
			case Input::Key::R: return ImGuiKey_R;
			case Input::Key::S: return ImGuiKey_S;
			case Input::Key::T: return ImGuiKey_T;
			case Input::Key::U: return ImGuiKey_U;
			case Input::Key::V: return ImGuiKey_V;
			case Input::Key::W: return ImGuiKey_W;
			case Input::Key::X: return ImGuiKey_X;
			case Input::Key::Y: return ImGuiKey_Y;
			case Input::Key::Z: return ImGuiKey_Z;
			case Input::Key::F1: return ImGuiKey_F1;
			case Input::Key::F2: return ImGuiKey_F2;
			case Input::Key::F3: return ImGuiKey_F3;
			case Input::Key::F4: return ImGuiKey_F4;
			case Input::Key::F5: return ImGuiKey_F5;
			case Input::Key::F6: return ImGuiKey_F6;
			case Input::Key::F7: return ImGuiKey_F7;
			case Input::Key::F8: return ImGuiKey_F8;
			case Input::Key::F9: return ImGuiKey_F9;
			case Input::Key::F10: return ImGuiKey_F10;
			case Input::Key::F11: return ImGuiKey_F11;
			case Input::Key::F12: return ImGuiKey_F12;
			}
			return ImGuiKey_None;
	    }

		void SetupImGuiDarkMaterialStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();

			ImGui::NotificationSettings.Position = ImGuiToastPos::TopRight;
			ImGui::NotificationSettings.Direction = ImGuiToastDirection::Down;
			ImGui::NotificationSettings.Order = ImGuiToastOrder::FIFO;

			ImVec4* colors = style.Colors;

			// Main background and window background
			colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

			// Border
			colors[ImGuiCol_Border] = ImVec4(0.28f, 0.28f, 0.28f, 0.50f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

			// Frame background
			colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

			// Title
			colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

			// Menu
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

			// Scrollbar
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

			// Check mark
			colors[ImGuiCol_CheckMark] = ImVec4(0.23f, 0.77f, 0.42f, 1.00f);

			// Slider
			colors[ImGuiCol_SliderGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

			// Buttons
			colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);

			// Header
			colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);

			// Separator
			colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

			// Resize grip
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

			// Tab
			colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

			// Plot
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

			// Text
			colors[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

			// Tables
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);

			// Drag and drop
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.60f, 0.00f, 0.90f);

			// Nav highlight
			colors[ImGuiCol_NavHighlight] = ImColor(0xFFFF7929).Value;

			// Misc
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

			style.FrameRounding = 2.5f;
			style.FrameBorderSize = 1.0f;
			style.GrabRounding = 4.0f;

	        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.WindowPadding = ImVec2(8.0f, 8.0f);
			style.FramePadding = ImVec2(4.0f, 4.0f);
			style.ItemSpacing = ImVec2(8.0f, 8.0f);
			style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
			style.IndentSpacing = 20.0f;
			style.ScrollbarSize = 15.0f;
			style.GrabMinSize = 10.0f;
			style.TabBorderSize = 1.0f;
		}
	}

	ImGuiSystem::ImGuiSystem() = default;
	ImGuiSystem::~ImGuiSystem() = default;

    void ImGuiSystem::Init(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem, Graphics::Window& _window)
    {
		window = &_window;

        ImGui::CreateContext();
		ImPlot::CreateContext();
		ImPlot3D::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		Internal::SetupImGuiDarkMaterialStyle();// ImGui::StyleColorsDark();

		FileSystem::Filepath settingsPath = FileSystem::Path::GetFullPath("tmp:/imgui.ini");
		if (FileSystem::Path::Exists(settingsPath) == false)
		{
			settingsPath = FileSystem::Path::GetFullPath("engine:/layouts/default.ini");
		}

		ImGui::LoadIniSettingsFromDisk(settingsPath.string().data());

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		onyxU64 fontHash = Hash::FNV1aHash64("fonts/Roboto-Regular.ttf");

        FileSystem::Filepath fontPath = FileSystem::Path::GetFullPath("engine:/fonts/Roboto-Regular.ttf");
		auto [it,_] = fonts.emplace(fontHash, io.Fonts->AddFontFromFileTTF(fontPath.string().data(), 16.0f, &fontConfig));
		fonts.emplace(fontHash, io.Fonts->AddFontFromFileTTF(fontPath.string().data(), 36.0f, &fontConfig));

		io.FontDefault = it->second;

        _window.AddOnResizeHandler(this, &ImGuiSystem::OnWindowResize);
		OnWindowResize(_window.GetWidth(), _window.GetHeight());
		
		inputSystem.AddOnInputHandler(this, &ImGuiSystem::OnInputEvent);

#if ONYX_IS_EDITOR
		PropertyGrid::SetAssetSystem(assetSystem);

		Assets::AssetId closedId("textures/editor/icons/contentbrowser/folder_closed.png");
		assetSystem.GetAsset(closedId, ImGuiSystem::FolderClosedAsset);

		Assets::AssetId openId("textures/editor/icons/contentbrowser/folder_open.png");
		assetSystem.GetAsset(openId, ImGuiSystem::FolderOpenAsset);

		Assets::AssetId closedSelectedId("textures/editor/icons/contentbrowser/folder_closed_selected.png");
		assetSystem.GetAsset(closedSelectedId, ImGuiSystem::FolderSelectedClosedAsset);

		Assets::AssetId openSelectedId("textures/editor/icons/contentbrowser/folder_open_selected.png");
		assetSystem.GetAsset(openSelectedId, ImGuiSystem::FolderSelectedOpenAsset);
#else
		ONYX_UNUSED(assetSystem);
#endif

#if ONYX_IS_WINDOWS
#else
		static_assert(false);
#endif
    }

    void ImGuiSystem::Shutdown(Input::InputSystem& inputSystem, Graphics::Window& _window)
    {
		const FileSystem::Filepath settingsPath = FileSystem::Path::GetTempDirectory() / "imgui.ini";
		ImGui::SaveIniSettingsToDisk(settingsPath.string().data());

		_window.RemoveOnResizeHandler(this, &ImGuiSystem::OnWindowResize);
		inputSystem.RemoveOnInputHandler(this, &ImGuiSystem::OnInputEvent);

		ImPlot::DestroyContext();
		ImPlot3D::DestroyContext();
		ImGui::DestroyContext();
    }

    void ImGuiSystem::Update(onyxU64 deltaTime)
    {
#if ONYX_IS_WINDOWS

		ImGuiIO& io = ImGui::GetIO();
	
		// TODO: Add single producer single consumer queue that supports popping the entire queue at once
		InplaceFunction<void(ImGuiIO&)> eventFunctor;
		while (queuedInputs.Pop(eventFunctor))
		{
			eventFunctor(io);
		}

		io.DeltaTime = std::max(numeric_cast<onyxF32>(deltaTime * 0.001f), 0.001f);
#endif



		// this is an index based loop on purpose as windows might be added during rendering by other windows
		const onyxU32 windowsCount = numeric_cast<onyxU32>(windows.size());
		for (onyxU32 i = 0; i < windowsCount; ++i)
		{
			const UniquePtr<ImGuiWindow>& imguiWindow = windows[i];
			imguiWindow->Render(*this);
		}
    }

    void ImGuiSystem::OnBeginFrame(Graphics::FrameContext&)
    {
		if (Internal::loc_ReloadLayout)
		{
			Internal::loc_ReloadLayout = false;
			/*FileSystem::Filepath settingsPath = FileSystem::Path::GetDataDirectory() / "layouts/default_2.ini";
			ImGui::LoadIniSettingsFromDisk(settingsPath.string().data());*/
		}

		if (Internal::loc_SaveLayout)
		{
			Internal::loc_SaveLayout = false;
			FileSystem::Filepath savePath;
			FileSystem::FileDialog saveDialog;
			DynamicArray<StringView> extensions{ "ini" };
			if (saveDialog.SaveFileDialog(savePath, "Ini File", extensions))
			{
				ImGui::SaveIniSettingsToDisk(savePath.string().data());
			}
		}

#if ONYX_USE_SDL2
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_Window.GetWindowHandle());
#elif ONYX_IS_WINDOWS
		// TODO:
#else
		static_assert(false);
#endif
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
    }

    void ImGuiSystem::OnEndFrame()
    {
		if (ImGui::GetMouseCursor() != ImGuiMouseCursor_None)
		{
			LPTSTR win32Cursor = IDC_ARROW;

			switch (ImGui::GetMouseCursor())
			{
			case ImGuiMouseCursor_Arrow:        win32Cursor = IDC_ARROW; break;
			case ImGuiMouseCursor_TextInput:    win32Cursor = IDC_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll:    win32Cursor = IDC_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW:     win32Cursor = IDC_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS:     win32Cursor = IDC_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW:   win32Cursor = IDC_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE:   win32Cursor = IDC_SIZENWSE; break;
			case ImGuiMouseCursor_Hand:         win32Cursor = IDC_HAND; break;
			}

			window->SetCursor(::LoadCursor(NULL, win32Cursor));
		}

		ImGuiIO& io = ImGui::GetIO();
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
    }

    Optional<ImGuiWindow*> ImGuiSystem::GetWindow(StringView windowName)
    {
        auto it = std::ranges::find_if(windows, [&](const UniquePtr<ImGuiWindow>& window)
        {
            return window->GetWindowId() == windowName;
        });

        if (it == windows.end())
        {
            return {};
        }

        return it->get();
    }

	void ImGuiSystem::OnWindowResize(onyxU32 width, onyxU32 height)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}

    void ImGuiSystem::OnInputEvent(const Input::InputEvent* event)
    {
		if (event->IsMouseEvent())
		{
			const Input::MouseEvent* mouseEvent = static_cast<const Input::MouseEvent*>(event);

			InplaceFunction<void(ImGuiIO&)> queuedInputEvent = [event = *mouseEvent](ImGuiIO& io)
				{
					if ((event.m_Id == Input::InputEventId::MouseButtonUp) || (event.m_Id == Input::InputEventId::MouseButtonDown))
						io.AddMouseButtonEvent(static_cast<onyxS32>(event.m_Button) - 1, event.m_Id == Input::InputEventId::MouseButtonDown);
					else if (event.m_Id == Input::InputEventId::MousePositionChanged)
						io.AddMousePosEvent(event.m_Position[0], event.m_Position[1]);
					else if (event.m_Id == Input::InputEventId::MouseWheel)
						io.AddMouseWheelEvent(0, event.m_Scroll);
				};

			queuedInputs.Push(std::move(queuedInputEvent));
			
			//return io.WantCaptureMouse; TODO 
		}
		else if (event->IsKeyboardEvent())
		{
			const Input::KeyboardEvent* keyboardEvent = static_cast<const Input::KeyboardEvent*>(event);

			InplaceFunction<void(ImGuiIO&)> queuedInputEvent = [event = *keyboardEvent](ImGuiIO& io)
			{
				if ((event.m_Id == Input::InputEventId::KeyUp) || (event.m_Id == Input::InputEventId::KeyDown))
				{
					bool isDown = event.m_Id == Input::InputEventId::KeyDown;
					if (IsModifierKey(event.m_Key))
					{
						switch (event.m_Key)
						{
						    case Input::Key::Left_Ctrl:
						    case Input::Key::Right_Ctrl:
						    {
								io.AddKeyEvent(ImGuiKey_ModCtrl, isDown);
								break;
						    }
							case Input::Key::Left_Shift:
							case Input::Key::Right_Shift:
							{
								io.AddKeyEvent(ImGuiKey_ModShift, isDown);
								break;
							}
							case Input::Key::Left_Alt:
							case Input::Key::Right_Alt:
							{
								io.AddKeyEvent(ImGuiKey_ModAlt, isDown);
								break;
							}
							case Input::Key::Left_System:
							case Input::Key::Right_System:
							{
								io.AddKeyEvent(ImGuiKey_ModSuper, isDown);
								break;
							}
						}
					}
					
					io.AddKeyEvent(Internal::ConvertToImGuiKey(event.m_Key), isDown);
					
				}
				else if (event.m_Id == Input::InputEventId::KeyCharacter)
					io.AddInputCharacterUTF16(event.m_Char);
			};

			queuedInputs.Push(std::move(queuedInputEvent));
		}
		else if (event->IsGamepadButtonEvent())
		{
		    // TODO
		}

		//return false; TODO
    }
}

#endif