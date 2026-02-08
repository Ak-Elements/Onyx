#include <onyx/ui/imguisystem.h>

#if ONYX_USE_IMGUI

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/input/inputsystem.h>
#include <onyx/input/inputevent.h>
#include <onyx/ui/imguinotify.h>
#include <onyx/ui/imguiwindow.h>

#include <onyx/ui/propertygrid.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/ui/windows/enginevariableswindow.h>
#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/propertygrid/inspectors/enginevariableinspector.h>
#include <onyx/platform/platformsystem.h>

#include <ImGuizmo.h>
#include <implot.h>
#include <implot3d.h>

#if ONYX_IS_WINDOWS
#include <windows.h>
#endif

namespace Onyx::Ui
{
	ImGuiContext g_UiContext;

	namespace Internal
	{
		bool loc_ReloadLayout = false;
		bool loc_SaveLayout = false;

	    ImGuiKey ConvertToImGuiKey(Input::Key key)
	    {
			switch (key)
			{
				using enum Input::Key;
			    case Tab: return ImGuiKey_Tab;
			    case Left: return ImGuiKey_LeftArrow;
			    case Right: return ImGuiKey_RightArrow;
			    case Up: return ImGuiKey_UpArrow;
			    case Down: return ImGuiKey_DownArrow;
			    case PageUp: return ImGuiKey_PageUp;
			    case PageDown: return ImGuiKey_PageDown;
			    case Home: return ImGuiKey_Home;
			    case End: return ImGuiKey_End;
			    case Insert: return ImGuiKey_Insert;
			    case Delete: return ImGuiKey_Delete;
			    case Backspace: return ImGuiKey_Backspace;
			    case Space: return ImGuiKey_Space;
			    case Enter: return ImGuiKey_Enter;
			    case Escape: return ImGuiKey_Escape;
			    case Apostrophe: return ImGuiKey_Apostrophe;
			    case Comma: return ImGuiKey_Comma;
			    case Minus: return ImGuiKey_Minus;
			    case Period: return ImGuiKey_Period;
			    case Slash: return ImGuiKey_Slash;
			    case Semicolon: return ImGuiKey_Semicolon;
			    case Equals: return ImGuiKey_Equal;
			    case LeftBracket: return ImGuiKey_LeftBracket;
			    case BackSlash: // intentional fallthrough
			    case NonUsBackSlash:
			        return ImGuiKey_Backslash;
			    case RightBracket: return ImGuiKey_RightBracket;
			    case Grave: return ImGuiKey_GraveAccent;
			    case CapsLock: return ImGuiKey_CapsLock;
			    case ScrollLock: return ImGuiKey_ScrollLock;
			    case NumPad_Lock_Clear: return ImGuiKey_NumLock;
			    case PrintScreen: return ImGuiKey_PrintScreen;
			    case Pause: return ImGuiKey_Pause;
			    case NumPad_0: return ImGuiKey_Keypad0;
			    case NumPad_1: return ImGuiKey_Keypad1;
			    case NumPad_2: return ImGuiKey_Keypad2;
			    case NumPad_3: return ImGuiKey_Keypad3;
			    case NumPad_4: return ImGuiKey_Keypad4;
			    case NumPad_5: return ImGuiKey_Keypad5;
			    case NumPad_6: return ImGuiKey_Keypad6;
			    case NumPad_7: return ImGuiKey_Keypad7;
			    case NumPad_8: return ImGuiKey_Keypad8;
			    case NumPad_9: return ImGuiKey_Keypad9;
			    case NumPad_Period: return ImGuiKey_KeypadDecimal;
			    case NumPad_Divide: return ImGuiKey_KeypadDivide;
			    case NumPad_Multiply: return ImGuiKey_KeypadMultiply;
			    case NumPad_Minus: return ImGuiKey_KeypadSubtract;
			    case NumPad_Plus: return ImGuiKey_KeypadAdd;
			    case NumPad_Enter: return ImGuiKey_KeypadEnter;
			    case NumPad_Equals: return ImGuiKey_KeypadEqual;
			    case Left_Ctrl: return ImGuiKey_LeftCtrl;
			    case Left_Shift: return ImGuiKey_LeftShift;
			    case Left_Alt: return ImGuiKey_LeftAlt;
			    case Left_System: return ImGuiKey_LeftSuper;
			    case Right_Ctrl: return ImGuiKey_RightCtrl;
			    case Right_Shift: return ImGuiKey_RightShift;
			    case Right_Alt: return ImGuiKey_RightAlt;
			    case Right_System: return ImGuiKey_RightSuper;
			    case Menu: return ImGuiKey_Menu;
			    case Key0: return ImGuiKey_0;
			    case Key1: return ImGuiKey_1;
			    case Key2: return ImGuiKey_2;
			    case Key3: return ImGuiKey_3;
			    case Key4: return ImGuiKey_4;
			    case Key5: return ImGuiKey_5;
			    case Key6: return ImGuiKey_6;
			    case Key7: return ImGuiKey_7;
			    case Key8: return ImGuiKey_8;
			    case Key9: return ImGuiKey_9;
			    case A: return ImGuiKey_A;
			    case B: return ImGuiKey_B;
			    case C: return ImGuiKey_C;
			    case D: return ImGuiKey_D;
			    case E: return ImGuiKey_E;
			    case F: return ImGuiKey_F;
			    case G: return ImGuiKey_G;
			    case H: return ImGuiKey_H;
			    case I: return ImGuiKey_I;
			    case J: return ImGuiKey_J;
			    case K: return ImGuiKey_K;
			    case L: return ImGuiKey_L;
			    case M: return ImGuiKey_M;
			    case N: return ImGuiKey_N;
			    case O: return ImGuiKey_O;
			    case P: return ImGuiKey_P;
			    case Q: return ImGuiKey_Q;
			    case R: return ImGuiKey_R;
			    case S: return ImGuiKey_S;
			    case T: return ImGuiKey_T;
			    case U: return ImGuiKey_U;
			    case V: return ImGuiKey_V;
			    case W: return ImGuiKey_W;
			    case X: return ImGuiKey_X;
			    case Y: return ImGuiKey_Y;
			    case Z: return ImGuiKey_Z;
			    case F1: return ImGuiKey_F1;
			    case F2: return ImGuiKey_F2;
			    case F3: return ImGuiKey_F3;
			    case F4: return ImGuiKey_F4;
			    case F5: return ImGuiKey_F5;
			    case F6: return ImGuiKey_F6;
			    case F7: return ImGuiKey_F7;
			    case F8: return ImGuiKey_F8;
			    case F9: return ImGuiKey_F9;
			    case F10: return ImGuiKey_F10;
			    case F11: return ImGuiKey_F11;
			    case F12: return ImGuiKey_F12;
                case Invalid:
			    case NonUsHash:
                default:
				    break;
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

		void SetupCatpuccin()
	    {
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = style.Colors;

			colors[ImGuiCol_Text] = ImVec4(0.91f, 0.89f, 0.84f, 1.00f); // text (#cdd6f4)
			colors[ImGuiCol_TextDisabled] = ImVec4(0.63f, 0.61f, 0.65f, 1.00f); // overlay0 (#7f849c)
			colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.13f, 0.20f, 1.00f); // base (#1e1e2e)
			colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.13f, 0.20f, 1.00f); // base
			colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.15f, 0.24f, 1.00f); // mantle (#181825)
			colors[ImGuiCol_Border] = ImVec4(0.37f, 0.36f, 0.49f, 0.65f); // surface1 (#45475a)
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

			colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.22f, 0.33f, 1.00f); // surface0 (#313244)
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.29f, 0.28f, 0.40f, 1.00f); // surface1 (#45475a)
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.41f, 0.55f, 1.00f); // surface2 (#585b70)

			colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.15f, 0.24f, 1.00f); // mantle
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.18f, 0.27f, 1.00f); // crust (#11111b)
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.15f, 0.24f, 0.75f);

			colors[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.15f, 0.24f, 1.00f); // mantle
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.15f, 0.24f, 1.00f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.37f, 0.36f, 0.49f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.41f, 0.55f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.55f, 0.67f, 1.00f);

			colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f); // mauve (#cba6f7)
			colors[ImGuiCol_SliderGrab] = ImVec4(0.82f, 0.65f, 0.45f, 1.00f); // yellow (#f9e2af)
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.80f, 0.54f, 1.00f);

			colors[ImGuiCol_Button] = ImVec4(0.29f, 0.28f, 0.40f, 1.00f); // surface1
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.41f, 0.55f, 1.00f); // surface2
			colors[ImGuiCol_ButtonActive] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f); // mauve

			colors[ImGuiCol_Header] = ImVec4(0.29f, 0.28f, 0.40f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.53f, 0.79f, 0.80f); // mauve
			colors[ImGuiCol_HeaderActive] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f);

			colors[ImGuiCol_Separator] = ImVec4(0.37f, 0.36f, 0.49f, 0.65f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.90f, 0.53f, 0.79f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f);

			colors[ImGuiCol_ResizeGrip] = ImVec4(0.90f, 0.53f, 0.79f, 0.25f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.90f, 0.53f, 0.79f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.90f, 0.53f, 0.79f, 0.95f);

			colors[ImGuiCol_Tab] = ImVec4(0.29f, 0.28f, 0.40f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.90f, 0.53f, 0.79f, 0.80f);
			colors[ImGuiCol_TabActive] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.19f, 0.18f, 0.27f, 1.00f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.29f, 0.28f, 0.40f, 1.00f);

			colors[ImGuiCol_DockingPreview] = ImVec4(0.90f, 0.53f, 0.79f, 0.70f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.14f, 0.13f, 0.20f, 1.00f);

			colors[ImGuiCol_PlotLines] = ImVec4(0.82f, 0.65f, 0.45f, 1.00f); // yellow
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f); // mauve
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.82f, 0.65f, 0.45f, 1.00f); // yellow
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f); // mauve

			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.18f, 0.27f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.23f, 0.22f, 0.33f, 1.00f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.22f, 0.33f, 0.50f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.14f, 0.13f, 0.20f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.14f, 0.13f, 0.20f, 0.06f);

			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.90f, 0.53f, 0.79f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(0.95f, 0.80f, 0.54f, 1.00f); // yellow

			colors[ImGuiCol_NavHighlight] = ImVec4(0.90f, 0.53f, 0.79f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.13f, 0.20f, 0.50f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.13f, 0.20f, 0.50f);

			// Optional tweaks for rounding/shadows
			style.FrameRounding = 6.0f;
			style.GrabRounding = 4.0f;
			style.WindowRounding = 8.0f;
	    }

		void SetCatppuccinMochaBlue()
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = style.Colors;

			// Base text & backgrounds
			colors[ImGuiCol_Text] = ImVec4(0.804f, 0.839f, 0.957f, 1.00f); // Text (#cdd6f4)
			colors[ImGuiCol_TextDisabled] = ImVec4(0.498f, 0.518f, 0.611f, 1.00f); // Overlay1 (#7f849c)
			colors[ImGuiCol_WindowBg] = ImVec4(0.118f, 0.118f, 0.180f, 1.00f); // Base (#1e1e2e)
			colors[ImGuiCol_ChildBg] = colors[ImGuiCol_WindowBg];
			colors[ImGuiCol_PopupBg] = ImVec4(0.094f, 0.094f, 0.145f, 1.00f); // Mantle (#181825)
			colors[ImGuiCol_Border] = ImVec4(0.271f, 0.278f, 0.353f, 0.65f); // Surface1 (#45475a)
			colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

			// Frames
			colors[ImGuiCol_FrameBg] = ImVec4(0.192f, 0.196f, 0.267f, 1.00f); // Surface0 (#313244)
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f); // Surface1
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.345f, 0.357f, 0.439f, 1.00f); // Surface2 (#585b70)

			// Titles
			colors[ImGuiCol_TitleBg] = ImVec4(0.094f, 0.094f, 0.145f, 1.00f); // Mantle
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.067f, 0.067f, 0.106f, 1.00f); // Crust (#11111b)
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.094f, 0.094f, 0.145f, 0.75f);

			// Scrollbar
			colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_PopupBg];
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.345f, 0.357f, 0.439f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.498f, 0.518f, 0.611f, 1.00f);

			// Accent = Periwinkle (Blue-shifted Mauve, HSV [220�, 0.32, 0.969])
			ImVec4 periwinkle = ImVec4(0.667f, 0.710f, 0.969f, 1.00f); // approx RGB from HSV

			colors[ImGuiCol_CheckMark] = periwinkle;
			colors[ImGuiCol_SliderGrab] = ImVec4(0.976f, 0.886f, 0.686f, 1.00f); // Yellow (#f9e2af)
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.980f, 0.702f, 0.529f, 1.00f); // Peach (#fab387)

			colors[ImGuiCol_Button] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f); // Surface1
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.345f, 0.357f, 0.439f, 1.00f); // Surface2
			colors[ImGuiCol_ButtonActive] = periwinkle;

			colors[ImGuiCol_Header] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = periwinkle;
			colors[ImGuiCol_HeaderActive] = periwinkle;

			colors[ImGuiCol_Separator] = ImVec4(0.271f, 0.278f, 0.353f, 0.65f);
			colors[ImGuiCol_SeparatorHovered] = periwinkle;
			colors[ImGuiCol_SeparatorActive] = periwinkle;

			colors[ImGuiCol_ResizeGrip] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.25f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = periwinkle;

			colors[ImGuiCol_Tab] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.533f, 0.569f, 0.773f, 1.0f);
			colors[ImGuiCol_TabActive] = ImVec4(0.467f, 0.498f, 0.678f, 1.0f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.067f, 0.067f, 0.106f, 1.00f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f);

			colors[ImGuiCol_DockingPreview] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.70f);
			colors[ImGuiCol_DockingEmptyBg] = colors[ImGuiCol_WindowBg];

			colors[ImGuiCol_PlotLines] = ImVec4(0.976f, 0.886f, 0.686f, 1.00f); // Yellow
			colors[ImGuiCol_PlotLinesHovered] = periwinkle;
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.976f, 0.886f, 0.686f, 1.00f); // Yellow
			colors[ImGuiCol_PlotHistogramHovered] = periwinkle;

			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.067f, 0.067f, 0.106f, 1.00f); // Crust
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.192f, 0.196f, 0.267f, 1.00f); // Surface0
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.192f, 0.196f, 0.267f, 0.50f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.06f);

			colors[ImGuiCol_TextSelectedBg] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(0.976f, 0.886f, 0.686f, 1.00f); // Yellow

			colors[ImGuiCol_NavHighlight] = periwinkle;
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 1, 1, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.118f, 0.118f, 0.180f, 0.50f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.118f, 0.118f, 0.180f, 0.50f);

			// Optional style tweaks
			style.FrameRounding = 6.0f;
			style.GrabRounding = 4.0f;
			style.WindowRounding = 8.0f;
		}

		void SetCatppuccinMochaGraphite()
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = style.Colors;

			// Base text & backgrounds
			colors[ImGuiCol_Text] = ImVec4(0.902f, 0.910f, 0.922f, 1.00f); // Text (#e6e8eb)
			colors[ImGuiCol_TextDisabled] = ImVec4(0.588f, 0.596f, 0.627f, 1.00f); // Subtext0 (#9698a0)
			colors[ImGuiCol_WindowBg] = ImVec4(0.118f, 0.118f, 0.137f, 1.00f); // Base (#1e1e23)
			colors[ImGuiCol_ChildBg] = colors[ImGuiCol_WindowBg];
			colors[ImGuiCol_PopupBg] = ImVec4(0.098f, 0.098f, 0.114f, 1.00f); // Mantle (#19191d)
			colors[ImGuiCol_Border] = ImVec4(0.271f, 0.278f, 0.314f, 0.65f); // Surface1 (#454851)
			colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

			// Frames
			colors[ImGuiCol_FrameBg] = ImVec4(0.204f, 0.208f, 0.231f, 1.00f); // Surface0 (#34363b)
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.271f, 0.278f, 0.314f, 1.00f); // Surface1
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.345f, 0.357f, 0.392f, 1.00f); // Surface2 (#585b64)

			// Titles
			colors[ImGuiCol_TitleBg] = ImVec4(0.098f, 0.098f, 0.114f, 1.00f); // Mantle
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.067f, 0.067f, 0.078f, 1.00f); // Crust (#111114)
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.098f, 0.098f, 0.114f, 0.75f);

			// Scrollbar
			colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_PopupBg];
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.271f, 0.278f, 0.314f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.345f, 0.357f, 0.392f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.498f, 0.518f, 0.588f, 1.00f);

			// Accent = Periwinkle blue
			ImVec4 periwinkle = ImVec4(0.667f, 0.710f, 0.969f, 1.00f); // #aab5f7

			colors[ImGuiCol_CheckMark] = periwinkle;
			colors[ImGuiCol_SliderGrab] = ImVec4(0.820f, 0.875f, 0.957f, 1.00f); // Pale blue (#d1dff4)
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.667f, 0.710f, 0.969f, 1.00f); // Periwinkle

			// Buttons
			colors[ImGuiCol_Button] = ImVec4(0.271f, 0.278f, 0.314f, 1.00f); // Surface1
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.345f, 0.357f, 0.392f, 1.00f); // Surface2
			colors[ImGuiCol_ButtonActive] = periwinkle;

			// Headers
			colors[ImGuiCol_Header] = ImVec4(0.271f, 0.278f, 0.353f, 1.00f); // Surface1
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.533f, 0.569f, 0.773f, 1.0f);  // lighter periwinkle
			colors[ImGuiCol_HeaderActive] = ImVec4(0.467f, 0.498f, 0.678f, 1.0f);  // darker periwinkle

			// Separators
			colors[ImGuiCol_Separator] = ImVec4(0.271f, 0.278f, 0.314f, 0.65f);
			colors[ImGuiCol_SeparatorHovered] = periwinkle;
			colors[ImGuiCol_SeparatorActive] = periwinkle;

			// Resize grips
			colors[ImGuiCol_ResizeGrip] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.25f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = periwinkle;

			// Tabs
			colors[ImGuiCol_Tab] = ImVec4(0.271f, 0.278f, 0.314f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.533f, 0.569f, 0.773f, 1.0f);
			colors[ImGuiCol_TabActive] = ImVec4(0.365f, 0.392f, 0.553f, 1.0f);  // darker periwinkle
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.067f, 0.067f, 0.078f, 1.00f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.271f, 0.278f, 0.314f, 1.00f);

			// Docking
			colors[ImGuiCol_DockingPreview] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.70f);
			colors[ImGuiCol_DockingEmptyBg] = colors[ImGuiCol_WindowBg];

			// Plots
			colors[ImGuiCol_PlotLines] = ImVec4(0.667f, 0.710f, 0.969f, 1.00f); // Periwinkle
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.820f, 0.875f, 0.957f, 1.00f); // Pale blue
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.667f, 0.710f, 0.969f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.820f, 0.875f, 0.957f, 1.00f);

			// Tables
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.067f, 0.067f, 0.078f, 1.00f); // Crust
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.204f, 0.208f, 0.231f, 1.00f); // Surface0
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.204f, 0.208f, 0.231f, 0.50f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.06f);

			// Misc
			colors[ImGuiCol_TextSelectedBg] = ImVec4(periwinkle.x, periwinkle.y, periwinkle.z, 0.35f);
			colors[ImGuiCol_DragDropTarget] = periwinkle;

			colors[ImGuiCol_NavHighlight] = periwinkle;
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 1, 1, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.118f, 0.118f, 0.137f, 0.50f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.118f, 0.118f, 0.137f, 0.50f);

			style.WindowRounding = 4.0f;
			style.FrameRounding = 2.5f;
			style.FrameBorderSize = 0.0f;
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

	
    ImGuiSystem::ImGuiSystem(IEngine& engine,
        Assets::AssetSystem& assetSystem,
        Input::InputSystem& inputSystem,
        Localization::LocalizationModule& localizationSystem,
        Graphics::GraphicsSystem& graphicsSystem,
        Platform::PlatformSystem& platformSystem
    )
        : m_Engine(&engine)
        , m_PlatformSystem(&platformSystem)
        , m_InputSystem(&inputSystem)
    {
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImPlot3D::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		Internal::SetCatppuccinMochaGraphite();// ImGui::StyleColorsDark();

		FilePath settingsPath = FileSystem::Path::GetFullPath("tmp:imgui.ini");
		if (FileSystem::Path::Exists(settingsPath) == false)
		{
			settingsPath = FileSystem::Path::GetFullPath("engine:/layouts/default.ini");
		}

		ImGui::LoadIniSettingsFromDisk(settingsPath.string().data());

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		constexpr StringId64 fontHash("fonts/Roboto-Regular.ttf");

		FilePath fontPath = FileSystem::Path::GetFullPath("engine:/fonts/Roboto-Regular.ttf");
		auto [it, _] = m_Fonts.emplace(fontHash, io.Fonts->AddFontFromFileTTF(fontPath.string().data(), 16.0f, &fontConfig));
		m_Fonts.emplace(fontHash, io.Fonts->AddFontFromFileTTF(fontPath.string().data(), 36.0f, &fontConfig));

		io.FontDefault = it->second;
		InitRenderBuffers(graphicsSystem);

		inputSystem.OnMouseAxisChange().Connect<&ImGuiSystem::OnMouseAxisChange>(this);
		inputSystem.OnMouseButton().Connect<&ImGuiSystem::OnMouseButton>(this);
		inputSystem.OnMousePositionChange().Connect<&ImGuiSystem::OnMousePositionChange>(this);
		inputSystem.OnKey().Connect<&ImGuiSystem::OnKey>(this);

		g_UiContext.AssetSystem = &assetSystem;
		g_UiContext.InputSystem = &inputSystem;
        g_UiContext.LocalizationSystem = &localizationSystem;

		Graphics::PipelineProperties pipelineProperties;
		pipelineProperties.Shader = Assets::AssetId("engine:/shaders/imgui.oshader");
		pipelineProperties.BlendStates.Emplace(
			Graphics::BlendState
			{
				.SourceColor = Graphics::Blend::SrcAlpha,
				.DestinationColor = Graphics::Blend::OneMinusSrcAlpha,
				.ColorOperation = Graphics::BlendOperation::Add,
				.SourceAlpha = Graphics::Blend::Invalid,
				.DestinationAlpha = Graphics::Blend::Invalid,
				.AlphaOperation = Graphics::BlendOperation::Add,
				.IsBlendEnabled = true
			}
		);

        Graphics::RenderPassSettings renderPassSettings;
        Graphics::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.Emplace();
        Graphics::RenderPassSettings::Attachment attachment{};

		attachment.m_Format = Enums::ToIntegral(Graphics::TextureFormat::BGRA_UNORM8);
		attachment.m_LoadOp = Enums::ToIntegral(Graphics::RenderPassSettings::LoadOp::DontCare);
		renderPassSettings.m_Attachments.Add(attachment);

		subpass.m_AttachmentAccesses.Emplace(Graphics::RenderPassSettings::AttachmentAccess::RenderTarget);
		pipelineProperties.RenderPass = graphicsSystem.GetOrCreateRenderPass(renderPassSettings);

		m_ImguiShader = graphicsSystem.CreateShaderInstance(pipelineProperties.Shader, pipelineProperties);
		
        Platform::Window& mainWindow = m_PlatformSystem->GetMainWindow();
        mainWindow.OnResize().Connect<&ImGuiSystem::OnWindowResize>(this);
        
        RegisterWindow<EngineVariablesWindow>();
    }

	ImGuiSystem::~ImGuiSystem()
	{
		const FilePath settingsPath = FileSystem::Path::GetTempDirectory() / "imgui.ini";
		ImGui::SaveIniSettingsToDisk(settingsPath.string().data());

		//m_Window->RemoveOnResizeHandler(this, &ImGuiSystem::OnWindowResize);
		m_InputSystem->OnMouseAxisChange().Disconnect(this);
		m_InputSystem->OnMouseButton().Disconnect(this);
		m_InputSystem->OnMousePositionChange().Disconnect(this);
		m_InputSystem->OnKey().Disconnect(this);
		
		ImPlot::DestroyContext();
		ImPlot3D::DestroyContext();
		ImGui::DestroyContext();
	}

    void ImGuiSystem::Update(Graphics::GraphicsSystem& system, DeltaGameTime deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO();	

		g_UiContext.GraphicsSystem = &system;
		io.DeltaTime = std::max(numeric_cast<onyxF32>(deltaTime.DeltaMilliseconds) * 0.001f, 0.001f);

		//// this is an index based loop on purpose as windows might be added during rendering by other windows
		const onyxU32 windowsCount = numeric_cast<onyxU32>(m_Windows.size());
		for (onyxU32 i = 0; i < windowsCount; ++i)
		{
			const UniquePtr<ImGuiWindow>& imguiWindow = m_Windows[i];
			imguiWindow->Render(*this);
		}

		g_UiContext.GraphicsSystem = nullptr;
    }

    void ImGuiSystem::OnBeginFrame(const Graphics::FrameContext& /*frameContext*/)
    {
		ImGuiIO& io = ImGui::GetIO();	
		// TODO: Fix
		auto& mainWindow = m_PlatformSystem->GetMainWindow();
		auto framebufferSize = mainWindow.GetFrameBufferSize();
		io.DisplaySize = ImVec2(static_cast<onyxF32>(framebufferSize.X), static_cast<onyxF32>(framebufferSize.Y));

		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		if (Internal::loc_ReloadLayout)
		{
			Internal::loc_ReloadLayout = false;
			/*FilePath settingsPath = FileSystem::Path::GetDataDirectory() / "layouts/default_2.ini";
			ImGui::LoadIniSettingsFromDisk(settingsPath.string().data());*/
		}
		
		if (Internal::loc_SaveLayout)
		{
			Internal::loc_SaveLayout = false;
			FilePath savePath;
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
#endif

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
    }

	void ImGuiSystem::OnRenderFrame(const Graphics::FrameContext& frameContext)
    {
		if (m_ImguiShader.IsValid() == false)
			return;

		ImGui::Render();

		UpdateDrawBuffers(frameContext);
        
        Graphics::CommandBuffer& commandBuffer = frameContext.Api->GetCommandBuffer(frameContext.FrameIndex, true);

		const Reference<Graphics::Pipeline>& pipeline = m_ImguiShader->GetPipeline();
		const Graphics::PipelineProperties& properties = pipeline->GetProperties();

        Graphics::FramebufferSettings framebufferSettings; 
		framebufferSettings.m_RenderPass = properties.RenderPass;

		const Vector2s32& swapchainExtent = frameContext.Api->GetSwapchainExtent();
		framebufferSettings.m_Width = swapchainExtent.X;
		framebufferSettings.m_Height = swapchainExtent.Y;

		Graphics::TextureHandle swapchainImage = frameContext.Api->GetAcquiredSwapChainImage();
		framebufferSettings.m_ColorTargets.Add(swapchainImage.Texture);

		Graphics::FramebufferHandle frameBuffer = frameContext.Api->GetOrCreateFramebuffer(framebufferSettings);

		// TODO: Do a proper barrier here for the rendergraph to be finished and the imgui pass to start
		commandBuffer.TransitionLayout(swapchainImage, Graphics::Context::Graphics, Graphics::Access::ColorAttachmentWrite, Graphics::ImageLayout::AttachmentOptimal);
		commandBuffer.GlobalBarrier(Enums::ToIntegral(Graphics::Access::ColorAttachmentWrite), 0x00000400ULL, Enums::ToIntegral(Graphics::Access::None), 0x00000001ULL);
		commandBuffer.BeginRenderPass(properties.RenderPass, frameBuffer);
		commandBuffer.SetViewport();
		commandBuffer.BindShaderEffect(m_ImguiShader);

		ImDrawData* imDrawData = ImGui::GetDrawData();

		onyxS32 fb_width = numeric_cast<onyxS32>(imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x);
		onyxS32 fb_height = numeric_cast<onyxS32>(imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
		{
			return;
		}

		onyxS32 vertexOffset = 0;
		onyxS32 indexOffset = 0;

		const onyxU8 frameIndex = frameContext.FrameIndex;
		const Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[frameIndex];
		const Graphics::BufferHandle& indexBuffer = m_IndexBuffers[frameIndex];
		commandBuffer.BindVertexBuffer(vertexBuffer, 0, 0);
		commandBuffer.BindIndexBuffer(indexBuffer, 0, Graphics::IndexType::uint16);

		struct PushConstants
		{
			Vector2f32 scale;
			Vector2f32 translate;
		} constants;

		constants.scale = { 2.0f / imDrawData->DisplaySize.x, -2.0f / imDrawData->DisplaySize.y };
		constants.translate = { -1.0f - imDrawData->DisplayPos.x * constants.scale[0], 1.0f - imDrawData->DisplayPos.y * constants.scale[1] };
		commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, constants);

		// Will project scissor/clipping rectangles into framebuffer space
		ImVec2 clip_off = imDrawData->DisplayPos; // (0,0) unless using multi-viewports
		ImVec2 clip_scale = imDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		for (onyxS32 i = 0; i < imDrawData->CmdListsCount; i++)
		{
			const ImDrawList* cmd_list = imDrawData->CmdLists[i];
			for (onyxS32 j = 0; j < cmd_list->CmdBuffer.Size; j++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
				if (pcmd->UserCallback != nullptr)
				{
					// User callback, registered via ImDrawList::AddCallback()
					// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
					//if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					//    commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants), &constants);
					//else
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					ImTextureID textureId = pcmd->TextureId;

					ImVec4 clip_rect;
					clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
					clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
					clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
					clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

					if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
					{
						// Negative offsets are illegal for vkCmdSetScissor
						if (clip_rect.x < 0.0f)
							clip_rect.x = 0.0f;
						if (clip_rect.y < 0.0f)
							clip_rect.y = 0.0f;

						// Apply scissor/clipping rectangle
						Rect2s16 scissor;
						scissor.Position[0] = (onyxS16)(clip_rect.x);
						scissor.Position[1] = (onyxS16)(clip_rect.y);
						scissor.Extents[0] = (onyxS16)(std::abs(clip_rect.z - clip_rect.x));
						scissor.Extents[1] = (onyxS16)(std::abs(clip_rect.w - clip_rect.y));
						//vkCmdSetScissor(command_buffer, 0, 1, &scissor);

						commandBuffer.SetScissor(scissor);
						commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, pcmd->ElemCount, 1,
							pcmd->IdxOffset + indexOffset, pcmd->VtxOffset + vertexOffset, numeric_cast<onyxU32>(textureId));
					}
				}
			}

			vertexOffset += cmd_list->VtxBuffer.Size;
			indexOffset += cmd_list->IdxBuffer.Size;

			Rect2s16 scissor;
			scissor.Position = { 0, 0 };
			scissor.Extents = { (onyxS16)fb_width, (onyxS16)fb_height };
			commandBuffer.SetScissor(scissor);
		}

		commandBuffer.EndRenderPass();
    }

    void ImGuiSystem::OnEndFrame(const Graphics::FrameContext& /*frameContext*/)
    {
#if ONYX_IS_WINDOWS
		//if (ImGui::GetMouseCursor() != ImGuiMouseCursor_None)
		//{
		//	LPTSTR win32Cursor = IDC_ARROW;
		//	
		//	switch (ImGui::GetMouseCursor())
		//	{
		//		case ImGuiMouseCursor_Arrow:        win32Cursor = IDC_ARROW; break;
		//		case ImGuiMouseCursor_TextInput:    win32Cursor = IDC_IBEAM; break;
		//		case ImGuiMouseCursor_ResizeAll:    win32Cursor = IDC_SIZEALL; break;
		//		case ImGuiMouseCursor_ResizeEW:     win32Cursor = IDC_SIZEWE; break;
		//		case ImGuiMouseCursor_ResizeNS:     win32Cursor = IDC_SIZENS; break;
		//		case ImGuiMouseCursor_ResizeNESW:   win32Cursor = IDC_SIZENESW; break;
		//		case ImGuiMouseCursor_ResizeNWSE:   win32Cursor = IDC_SIZENWSE; break;
		//		case ImGuiMouseCursor_Hand:         win32Cursor = IDC_HAND; break;
		//	}
		//	
		//	m_Window->SetCursor(::LoadCursor(NULL, win32Cursor));
		//}
#endif
		
		ImGuiIO& io = ImGui::GetIO();

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui::EndFrame();
    }

    Optional<ImGuiWindow*> ImGuiSystem::GetWindow(StringId32 windowId)
    {
        auto it = std::ranges::find_if(m_Windows, [&](const UniquePtr<ImGuiWindow>& window)
        {
            return StringId32(window->GetWindowId()) == windowId;
        });

        if (it == m_Windows.end())
        {
            return {};
        }

        return it->get();
    }

    void ImGuiSystem::InitRenderBuffers(Graphics::GraphicsSystem& graphicsSystem)
    {
		ImGuiIO& io = ImGui::GetIO();

		//// Create font texture
		unsigned char* fontData;
		int texWidth, texHeight;

		io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
		onyxU32 uploadSize = texWidth * texHeight * 4 * sizeof(char);

		//// Create target image for copy
		Graphics::TextureStorageProperties storageProps;
		storageProps.m_Format = Graphics::TextureFormat::RGBA_UNORM8;
		storageProps.m_Type = Graphics::TextureType::Texture2D;
		storageProps.m_Size = { texWidth, texHeight, 1 };
		storageProps.m_MaxMipLevel = 1;
		storageProps.m_ArraySize = 0;
		storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
		storageProps.m_CpuAccess = Graphics::CPUAccess::None;
		storageProps.m_GpuAccess = Graphics::GPUAccess::Read;
		storageProps.m_IsTexture = true;
		storageProps.m_DebugName = "ImGui Font Texture Storage";

		Graphics::TextureProperties textureProps;
		textureProps.m_Format = Graphics::TextureFormat::RGBA_UNORM8;
		textureProps.m_AllowCubeMapLoads = false;
		textureProps.m_DebugName = "ImGui Font Texture";

		Span<onyxU8> fontTexData{ fontData, uploadSize };

		graphicsSystem.CreateTexture(m_FontImage, storageProps, textureProps, fontTexData);
		io.Fonts->TexID = m_FontImage.Texture->GetIndex();

		Graphics::BufferProperties vertexBufferProps;
		vertexBufferProps.m_Size = 400000 * sizeof(ImDrawVert);
		vertexBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Vertex);
		vertexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
		vertexBufferProps.m_DebugName = "ImGui Vertices";

		Graphics::BufferProperties indexBufferProps;
		indexBufferProps.m_Size = 200000 * sizeof(ImDrawIdx);
		indexBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Index);
		indexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
		indexBufferProps.m_DebugName = "ImGui Indices";

		for (onyxU8 i = 0; i < Graphics::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[i];
			graphicsSystem.CreateBuffer(vertexBuffer, vertexBufferProps);
			m_VertexCounts.Add(400000);

			Graphics::BufferHandle& indexBuffer = m_IndexBuffers[i];
			graphicsSystem.CreateBuffer(indexBuffer, indexBufferProps);
			m_IndexCounts.Add(200000);
		}
    }

	void ImGuiSystem::UpdateDrawBuffers(const Graphics::FrameContext& frameContext)
    {	
		ImDrawData* imDrawData = ImGui::GetDrawData();

		if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
		{
			return;
		}

		onyxS32 fb_width = numeric_cast<onyxS32>(imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x);
		onyxS32 fb_height = numeric_cast<onyxS32>(imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		// Note: Alignment is done inside buffer creation
		onyxU32 vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		onyxU32 indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		// Update buffers only if vertex or index count has been changed compared to current buffer size
		if ((vertexBufferSize == 0) || (indexBufferSize == 0))
		{
			return /*false*/;
		}

		// Vertex buffer
		const onyxU8 frameIndex = frameContext.FrameIndex;
		Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[frameIndex];
		if ((vertexBuffer.Buffer.IsValid() == false) || (vertexBuffer.Buffer->IsMapped() == false) || (m_VertexCounts[frameIndex] < imDrawData->TotalVtxCount))
		{
			Graphics::BufferProperties vertexBufferProps = vertexBuffer.Buffer->GetProperties();
			vertexBufferProps.m_Size = vertexBufferSize;

			frameContext.Api->CreateBuffer(vertexBuffer, vertexBufferProps);
			m_VertexCounts[frameIndex] = imDrawData->TotalVtxCount;
		}

		// Index buffer
		Graphics::BufferHandle& indexBuffer = m_IndexBuffers[frameIndex];
		if ((indexBuffer.Buffer.IsValid() == false) || (indexBuffer.Buffer->IsMapped() == false) || (m_IndexCounts[frameIndex] < imDrawData->TotalIdxCount))
		{
			Graphics::BufferProperties indexBufferProps = indexBuffer.Buffer->GetProperties();
			indexBufferProps.m_Size = indexBufferSize;

			frameContext.Api->CreateBuffer(indexBuffer, indexBufferProps);
			m_IndexCounts[frameIndex] = imDrawData->TotalIdxCount;
		}

		// Upload data
		onyxS32 vertexCopyOffset = 0;
		onyxS32 indexCopyOffset = 0;

		for (int n = 0; n < imDrawData->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = imDrawData->CmdLists[n];
			vertexBuffer.Buffer->SetData(vertexCopyOffset, cmd_list->VtxBuffer.Data,
				cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			indexBuffer.Buffer->SetData(indexCopyOffset, cmd_list->IdxBuffer.Data,
				cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

			vertexCopyOffset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
			indexCopyOffset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
		}
    }

    void ImGuiSystem::OnWindowResize(Vector2s32 size)
	{
        ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(numeric_cast<onyxF32>(size.X), numeric_cast<onyxF32>(size.Y));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}

	void ImGuiSystem::OnMouseAxisChange(const Input::MouseAxisEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(0, event.Value);
	}

	void ImGuiSystem::OnMouseButton(const Input::MouseButtonEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(static_cast<onyxS32>(event.Button) - 1, event.State == Input::ButtonState::Down);
	}

	void ImGuiSystem::OnMousePositionChange(const Input::MousePositionEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(numeric_cast<onyxF32>(event.Position[0]), numeric_cast<onyxF32>(event.Position[1]));
	}

	void ImGuiSystem::OnKey(const Input::KeyboardEvent& keyboardEvent)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		bool isDown = keyboardEvent.State == Input::ButtonState::Down;
		if (IsModifierKey(keyboardEvent.Key))
		{
			switch (keyboardEvent.Key)
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
			    default:
				    ONYX_ASSERT(false, "Invalid modifier key");
				    break;
			}
		}
		
		io.AddKeyEvent(Internal::ConvertToImGuiKey(keyboardEvent.Key), isDown);
		
		if (isDown && (keyboardEvent.Char != 0))
			io.AddInputCharacterUTF16(keyboardEvent.Char);
		
	}

	void ImGuiSystem::OnControllerAxisChange(const Input::GameControllerAxisEvent& /*event*/)
	{
	}

	void ImGuiSystem::OnControllerButton(const Input::GameControllerButtonEvent& /*event*/)
	{

	}
}

#endif