#include <onyx/graphics/window/windows/nativewindow.h>

#if ONYX_IS_WINDOWS

#include <onyx/encoding.h>
#include <onyx/log/logger.h>

#include <onyx/filesystem/imagefile.h>
#include <onyx/profiler/profiler.h>

#include <vulkan/vulkan.h>

#include <Windows.h>
#include <dwmapi.h>

namespace
{
//    HHOOK g_hKeyboardHook = nullptr;
//
//    LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
//    {
//        if (nCode < 0 || nCode != HC_ACTION)  // do not process message 
//            return ::CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
//
//        bool bEatKeystroke = false;
//        KBDLLHOOKSTRUCT* p = std::bit_cast<KBDLLHOOKSTRUCT*>(lParam);
//        switch (wParam)
//        {
//        case WM_KEYDOWN:
//        case WM_KEYUP:
//        {
//            bEatKeystroke = ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN));
//            // Note that this will not block the Xbox Game Bar hotkeys (Win+G, Win+Alt+R, etc.)
//            break;
//        }
//        }
//
//        if (bEatKeystroke)
//            return 1;
//        else
//            return ::CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
//    }

    ::HICON CreateIconFromBitmap(const Onyx::Span<Onyx::onyxU8>& imageData, Onyx::Vector2s32 imageSize)
    {
        int i;
        ::HDC dc;
        ::HBITMAP color, mask;
        ::BITMAPV5HEADER bi;
        ::ICONINFO ii;
        unsigned char* target = nullptr;
        unsigned char* source = imageData.data();

        ZeroMemory(&bi, sizeof(bi));
        bi.bV5Size = sizeof(bi);
        bi.bV5Width = imageSize[0];
        bi.bV5Height = -imageSize[1];
        bi.bV5Planes = 1;
        bi.bV5BitCount = 32;
        bi.bV5Compression = BI_BITFIELDS;
        bi.bV5RedMask = 0x00ff0000;
        bi.bV5GreenMask = 0x0000ff00;
        bi.bV5BlueMask = 0x000000ff;
        bi.bV5AlphaMask = 0xff000000;

        dc = ::GetDC(nullptr);
        color = ::CreateDIBSection(dc,
            (BITMAPINFO*)&bi,
            DIB_RGB_COLORS,
            (void**)&target,
            nullptr,
            (DWORD)0);
        ReleaseDC(nullptr, dc);

        if (!color)
        {
            return nullptr;
        }

        mask = ::CreateBitmap(imageSize[0], imageSize[1], 1, 1, NULL);
        if (!mask)
        {
            DeleteObject(color);
            return nullptr;
        }

        for (i = 0; i < imageSize[0] * imageSize[1]; i++)
        {
            target[0] = source[2];
            target[1] = source[1];
            target[2] = source[0];
            target[3] = source[3];
            target += 4;
            source += 4;
        }

        ZeroMemory(&ii, sizeof(ii));
        ii.fIcon = true;
        ii.xHotspot = 0;
        ii.yHotspot = 0;
        ii.hbmMask = mask;
        ii.hbmColor = color;

        ::HICON handle = ::CreateIconIndirect(&ii);

        ::DeleteObject(color);
        ::DeleteObject(mask);

        if (!handle)
        {
            // return error
            return nullptr;
        }

        return handle;
    }
}

namespace Onyx::Graphics
{
    Window::~Window()
    {
        Destroy();
    }

    void Window::Create(const WindowSettings& settings)
    {
        m_Settings = settings;
        m_IsInitialized = false;
        myWakeFromSleepEvent = CreateEvent(nullptr, FALSE, FALSE, "Local\\WindowThread_WakeFromSleepEvent");
        Start();

        m_IsInitialized.wait(false);
    }

    void Window::Destroy()
    {
        Stop(true);
        if (myWakeFromSleepEvent != nullptr)
        {
            ::CloseHandle(myWakeFromSleepEvent);
            myWakeFromSleepEvent = nullptr;
        }
    }

    void Window::Show()
    {
        ::ShowWindow(m_WindowHandle, SW_SHOW);
    }

    void Window::Minimize()
    {
        ::ShowWindow(m_WindowHandle, SW_MINIMIZE);
    }

    void Window::Maximize()
    {
        if (::IsWindowVisible(m_WindowHandle))
            ::ShowWindow(m_WindowHandle, SW_MAXIMIZE);
        else
            FitToMonitor();
    }


    void Window::Hide()
    {
        ::ShowWindow(m_WindowHandle, SW_HIDE);
    }

    void Window::Focus()
    {
        ::BringWindowToTop(m_WindowHandle);
        ::SetForegroundWindow(m_WindowHandle);
        ::SetFocus(m_WindowHandle);
    }

    void Window::RequestWindowAttention()
    {
        ::FlashWindow(m_WindowHandle, true);
    }

    void Window::ToggleCursor(bool enable)
    {
        if (enable)
            ReleaseCursor();
        else
            CaptureCursor();
    }

    void Window::UpdateCursorImage()
    {
        if (m_CursorMode == CursorMode::Normal ||
            m_CursorMode == CursorMode::Captured)
        {
            // add support for custom cursors
            ::SetCursor(m_Cursor);
        }
        else
            ::SetCursor(nullptr);
    }

    onyxS64 Window::OnWindowProc(HWND hWnd, onyxU32 message, onyxU64 wParam, onyxS64 lParam)
    {
        using namespace Onyx;
        Window* pThis; // our "this" pointer will go here
        if (message == WM_NCCREATE)
        {
            // Recover the "this" pointer which was passed as a parameter to CreateWindow(Ex).
            LPCREATESTRUCT lpcs = std::bit_cast<LPCREATESTRUCT>(lParam);
            pThis = static_cast<Window*>(lpcs->lpCreateParams);
            // Put the value in a safe place for future use
            SetWindowLongPtr(hWnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(pThis));
        }
        else
        {
            // Recover the "this" pointer from where our WM_NCCREATE handler
            // stashed it.
            pThis = std::bit_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pThis && pThis->HandleWindowMessages(message, wParam, lParam))
        {
            return 1;
        }

        return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

    void Window::OnStart()
    {
        ONYX_PROFILE_SET_THREAD(WindowThread)
        m_Cursor = ::LoadCursor(nullptr, IDC_ARROW);

        CreateNativeWindow();

        /*g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandle(nullptr), 0);*/

        ONYX_ASSERT(m_WindowHandle, "Could not create window!");
        SetState(WindowState::Default);
        SetWindowMode(m_Settings.Mode);

        ::UpdateWindow(m_WindowHandle);

        m_IsInitialized = true;
        m_IsInitialized.notify_one();
    }

    void Window::OnUpdate()
    {
        MSG msg;
        while (IsRunning())
        {
            ONYX_PROFILE_SECTION(OnUpdate)

            while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessageW(&msg);

                if (msg.message == WM_QUIT)
                {
                    break;
                }
            }

            ::MsgWaitForMultipleObjects(1, &myWakeFromSleepEvent, false, 1000, QS_ALLINPUT);
        }
    }

    void Window::OnStop()
    {
    }

    void Window::CreateNativeWindow()
    {
        HINSTANCE instance = ::GetModuleHandle(nullptr);

        String_U16 windowTitle = Encoding::utf8_to_utf16(m_Settings.Title);
        HICON icon = static_cast<HICON>(m_Settings.Icon);
        WNDCLASSEXW wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wndClass.lpfnWndProc = OnWindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = instance;
        wndClass.hIcon = icon;
        wndClass.hCursor = m_Cursor;
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = (LPCWSTR)windowTitle.data();
        wndClass.hIconSm = icon;

        if (!::RegisterClassExW(&wndClass))
        {
            ONYX_ASSERT(false, "Failed registering window class!");
            return;
        }

        onyxU32 screenWidth = static_cast<onyxU32>(::GetSystemMetrics(SM_CXSCREEN));
        onyxU32 screenHeight = static_cast<onyxU32>(::GetSystemMetrics(SM_CYSCREEN));

        bool isFullscreen = (m_Settings.Mode == WindowMode::Fullscreen) || (m_Settings.Mode == WindowMode::Borderless);

        DWORD dwExStyle = GetExtendedStyle();
        DWORD dwStyle = GetStyle();

        //TODO: build based on monitor index if user selected different display

        if (m_Settings.Position[0] == -1)
        {
            m_Settings.Position[0] = (screenWidth - m_Settings.Size[0]) / 2;
            m_Settings.Position[1] = (screenHeight - m_Settings.Size[1]) / 2;
        }

        RECT windowRect;
        windowRect.left = isFullscreen ? 0 : m_Settings.Position[0];
        windowRect.top = isFullscreen ? 0 : m_Settings.Position[0];
        windowRect.right = isFullscreen ? screenWidth : m_Settings.Size[0];
        windowRect.bottom = isFullscreen ? screenHeight : m_Settings.Size[1];

        ::AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

        m_WindowHandle = ::CreateWindowW(
            (LPCWSTR)windowTitle.data(),
            (LPCWSTR)windowTitle.data(),
            dwStyle,
            windowRect.left,
            windowRect.top,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL, // parent window
            NULL,
            instance,
            this);

       /* if (m_Settings.m_Icon != nullptr)
        {
            SendMessage(m_WindowHandle, WM_SETICON, ICON_BIG, (LPARAM)m_Settings.m_Icon);
            SendMessage(m_WindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)m_Settings.m_SmallIcon);
        }*/
    }

    void Window::FitToMonitor()
    {
        RECT rect;
        DWORD style;
        MONITORINFO mi;

        ::GetMonitorInfoW(MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST), &mi);

        rect = mi.rcWork;

        if (m_Settings.MaxSize[0] != 0 && m_Settings.MaxSize[1] != 0)
        {
            rect.right = std::min(rect.right, rect.left + m_Settings.MaxSize[0]);
            rect.bottom = std::min(rect.bottom, rect.top + m_Settings.MaxSize[1]);
        }

        style = ::GetWindowLongW(m_WindowHandle, GWL_STYLE);
        style |= WS_MAXIMIZE;
        ::SetWindowLongW(m_WindowHandle, GWL_STYLE, style);

        bool hasBorder = false;
        if (hasBorder)
        {
            const DWORD exStyle = ::GetWindowLongW(m_WindowHandle, GWL_EXSTYLE);

            const UINT dpi = GetDpiForWindow(m_WindowHandle);
            ::AdjustWindowRectExForDpi(&rect, style, FALSE, exStyle, dpi);
            ::OffsetRect(&rect, 0, GetSystemMetricsForDpi(SM_CYCAPTION, dpi));

            rect.bottom = std::min(rect.bottom, mi.rcWork.bottom);
        }

        SetWindowPos(m_WindowHandle, HWND_TOP,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);

        m_OnResize(rect.right - rect.left, rect.bottom - rect.top);
    }

    void Window::CaptureCursor()
    {
        RECT clipRect;
        ::GetClientRect(m_WindowHandle, &clipRect);
        ::ClientToScreen(m_WindowHandle, (POINT*)&clipRect.left);
        ::ClientToScreen(m_WindowHandle, (POINT*)&clipRect.right);
        ::ClipCursor(&clipRect);
    }

    void Window::ReleaseCursor()
    {
        ClipCursor(nullptr);
    }

    onyxS32 Window::GetStyle()
    {
        DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        if (m_Settings.Mode == WindowMode::Fullscreen)
        {
            windowStyle |= WS_POPUP;

        }
        else if (m_Settings.Mode == WindowMode::Borderless)
        {
            // borderless fullscreen
            windowStyle |= WS_POPUP;
        }
        else
        {
            // windowed
            bool allowBorder = true;
            bool allowResize = true;
            windowStyle |= allowBorder ? ((allowResize ? WS_OVERLAPPEDWINDOW : (WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX))) : 0;
        }

        return windowStyle;
    }

    onyxS32 Window::GetExtendedStyle()
    {
        DWORD style = WS_EX_APPWINDOW;

        if (m_Settings.Mode == WindowMode::Fullscreen)
            style |= WS_EX_TOPMOST;

        return style;
    }

    bool Window::HandleWindowMessages(onyxU32 messageType, onyxU64 wParam, onyxS64 lParam)
    {
        ONYX_PROFILE_SECTION(HandleWindowMessages)

        // TODO: we need to return an outValue and handled bool as some events require to return 0 to the windows api
        static RECT borderThickness;

        switch (messageType)
        {
            case WM_CLOSE:
            {
                m_OnClose();
                ::DestroyWindow(m_WindowHandle);
                ::PostQuitMessage(0);
                break;
            }
            case WM_CREATE:
            {
                if (m_Settings.HasTitleBar)
                    break;

                
                //find border thickness
                ::SetRectEmpty(&borderThickness);
                if (GetWindowLongPtr(m_WindowHandle, GWL_STYLE) & WS_THICKFRAME)
                {
                    ::AdjustWindowRectEx(&borderThickness, GetWindowLongPtr(m_WindowHandle, GWL_STYLE) & ~WS_CAPTION, FALSE, NULL);
                    borderThickness.left *= -1;
                    borderThickness.top *= -1;
                }
                else// if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_BORDER)
                {
                    ::SetRect(&borderThickness, 4, 4, 4, 4);
                }

                //::MARGINS margins = {0};
                //::DwmExtendFrameIntoClientArea(m_WindowHandle, &margins);
                ::SetWindowPos(m_WindowHandle, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

                break;
            }
            case WM_SYSCOMMAND:
            {
                switch (wParam & 0xfff0)
                {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                {
                    if (m_Settings.Mode == WindowMode::Fullscreen)
                    {
                        // We are running in full screen mode, so disallow
                        // screen saver and screen blanking
                        return 0;
                    }
                    else
                        break;
                }

                // User trying to access application menu using ALT?
                case SC_KEYMENU:
                {
                    if (m_Settings.HasMenu == false)
                        return 0;

                    break;
                }
                }
                break;
            }
            case WM_CAPTURECHANGED:
            {
                // HACK: Disable the cursor once the caption button action has been
                //       completed or cancelled
                /*if (lParam == 0 && m_->win32.frameAction)
                {
                    if (window->cursorMode == GLFW_CURSOR_DISABLED)
                        disableCursor(window);
                    else if (window->cursorMode == GLFW_CURSOR_CAPTURED)
                        captureCursor(window);

                    window->win32.frameAction = GLFW_FALSE;
                }
                */
                break;
            }
            case WM_ERASEBKGND:
            {
                return true;
            }
            case WM_ACTIVATE:
            {
                if (m_Settings.HasTitleBar)
                    break;
            }
            case WM_NCACTIVATE:
            case WM_NCPAINT:
            {
                // Prevent title bar from being drawn after restoring a minimized
                // undecorated window
                if (m_Settings.HasTitleBar == false)
                    return true;

                break;
            }
            case WM_NCHITTEST:
            {
                // TODO: Implement borderless nom maximized window titlebar for custom tile bars
                //if (m_Settings.m_HasTitleBar)
                //    break;

                //onyxU32 x = lParam & 0xFFFF;
                //onyxU32 y = (lParam >> 16) & 0xFFFF;
                //POINT pt = { x, y };
                //ScreenToClient(m_WindowHandle, &pt);
                //RECT rc;
                //GetClientRect(m_WindowHandle, &rc);

                //int titlebarHittest = 0;
                ////_glfwInputTitleBarHitTest(m_WindowHandle, pt.x, pt.y, &titlebarHittest);

                //if (titlebarHittest)
                //{
                //    return HTCAPTION;
                //}
                //else
                //{
                //    enum { left = 1, top = 2, right = 4, bottom = 8 };
                //    int hit = 0;
                //    if (pt.x < borderThickness.left)               hit |= left;
                //    if (pt.x > rc.right - borderThickness.right)   hit |= right;
                //    if (pt.y < borderThickness.top)                hit |= top;
                //    if (pt.y > rc.bottom - borderThickness.bottom) hit |= bottom;

                //    if (hit & top && hit & left)        return HTTOPLEFT;
                //    if (hit & top && hit & right)       return HTTOPRIGHT;
                //    if (hit & bottom && hit & left)     return HTBOTTOMLEFT;
                //    if (hit & bottom && hit & right)    return HTBOTTOMRIGHT;
                //    if (hit & left)                     return HTLEFT;
                //    if (hit & top)                      return HTTOP;
                //    if (hit & right)                    return HTRIGHT;
                //    if (hit & bottom)                   return HTBOTTOM;

                //    return HTCLIENT;
                //}
                break;
            }
            case WM_INPUTLANGCHANGE:
            {
                break;
            }
            case WM_SETFOCUS:
            {
                if (m_CursorMode == CursorMode::Disabled)
                    ToggleCursor(false);
                else if (m_CursorMode == CursorMode::Captured)
                    CaptureCursor();

                m_OnFocus(true);

                break;
            }
            case WM_KILLFOCUS:
            {
                if (m_CursorMode == CursorMode::Disabled)
                    ToggleCursor(true);
                else if (m_CursorMode == CursorMode::Captured)
                    ReleaseCursor();

                /*WindowState state = m_State;
                if (m_Settings.Mode == WindowMode::Fullscreen)
                {
                    state = m_Settings.Mode == WindowMode::Fullscreen ? WindowState::Minimized : WindowState::Background;
                }*/

                m_OnFocus(false);
                break;
            }
            case WM_PAINT:
            {
                LPPAINTSTRUCT lpPaint = {};
                if (::BeginPaint(m_WindowHandle, lpPaint) != nullptr)
                {
                    ::EndPaint(m_WindowHandle, lpPaint);
                    return true;
                }
                break;
            }
            case WM_NCCALCSIZE:
            {
                if (m_Settings.HasTitleBar || (wParam == false))
                    break;

                onyxU32 dpi = ::GetDpiForWindow(m_WindowHandle);

                onyxS32 frameWidth = ::GetSystemMetricsForDpi(SM_CXFRAME, dpi);
                onyxS32 frameHeight = ::GetSystemMetricsForDpi(SM_CYFRAME, dpi);
               

                ::NCCALCSIZE_PARAMS* params = (::NCCALCSIZE_PARAMS*)lParam;
                ::RECT* requested_client_rect = params->rgrc;

                requested_client_rect->right -= frameWidth;
                requested_client_rect->left += frameWidth;
                requested_client_rect->bottom -= frameHeight;

                // don't adjust top as otherwise we see a pixel border
                /*if (m_State == WindowState::Maximized)
                {
                    requested_client_rect->top += padding;
                }*/

                return false;
            }
            case WM_GETDPISCALEDSIZE :
            {
                //if (m_Settings.m_ScaleToMonitor)
                //    break;

                // Adjust the window size to keep the content area size constant
                ::RECT source;
                ::RECT target;
                ::SIZE* size = (::SIZE*)lParam;

                ::AdjustWindowRectExForDpi(&source, GetStyle(),false, GetExtendedStyle(), GetDpiForWindow(m_WindowHandle));
                ::AdjustWindowRectExForDpi(&target, GetStyle(), false, GetExtendedStyle(), LOWORD(wParam));

                size->cx += (target.right - target.left) -
                    (source.right - source.left);
                size->cy += (target.bottom - target.top) -
                    (source.bottom - source.top);
                return true;
            }
            
            case WM_SETCURSOR:
            {
                if (LOWORD(lParam) == HTCLIENT)
                {
                    UpdateCursorImage();
                    return true;
                }

                break;
            }
            case WM_DROPFILES:
            {
                return false;
            }
            case WM_UNICHAR:
            {
                if (wParam == UNICODE_NOCHAR)
                    return true;

                return false;
            }
            case WM_SIZE:
            {
                if (m_State != WindowState::Resizing)
                {
                    bool isMinimized = wParam == SIZE_MINIMIZED;
                    bool isMaximized = (wParam == SIZE_MAXIMIZED) || ((m_State == WindowState::Maximized) && wParam != SIZE_RESTORED);

                    if (isMinimized)
                    {
                        SetState(WindowState::Minimized);
                        return false;
                    }

                    if (isMaximized)
                        Maximize();

                    onyxU32 width = lParam & 0xFFFF;
                    onyxU32 height = (lParam >> 16) & 0xFFFF;
                    SetState(WindowState::Default);
                    SetSize(width, height);
                }

                break;
            }
            case WM_MOVE:
            {
                break;
            }
            case WM_WINDOWPOSCHANGING:
            {
                break;
            }
            case WM_WINDOWPOSCHANGED:
            {
                break;
            }
            case WM_GETMINMAXINFO:
            {
                LPMINMAXINFO minMaxInfo = std::bit_cast<LPMINMAXINFO>(lParam);
                minMaxInfo->ptMinTrackSize.x = 64;
                minMaxInfo->ptMinTrackSize.y = 64;
                break;
            }
            case WM_ENTERSIZEMOVE:
            {
                m_State = WindowState::Resizing;
                break;
            }
            case WM_EXITSIZEMOVE:
            {
                RECT clientRect;
                ::GetClientRect(m_WindowHandle, &clientRect);
                SetSize(clientRect.right, clientRect.bottom);
                SetState(WindowState::Default);
                break;
            }
        }

        if (m_WindowMessageHandler && m_WindowMessageHandler(messageType, wParam, lParam))
        {
            return true;
        }

        return false;
    }

    void Window::SetTitle(const std::string_view& title)
    {
        if (m_Settings.Title != title)
        {
            m_Settings.Title = title;

            String_U16 windowTitle = Encoding::utf8_to_utf16(m_Settings.Title);
            ::SetWindowTextW(m_WindowHandle, (LPCWSTR)windowTitle.data());
        }
    }

    void Window::SetIcon(const FileSystem::Filepath& path)
    {
        FileSystem::ImageFile iconFile(path);

        HICON icon = CreateIconFromBitmap(iconFile.GetData(), iconFile.GetSize());

        SendMessage(m_WindowHandle, WM_SETICON, ICON_BIG, (LPARAM)icon);
        SendMessage(m_WindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    }

    void Window::SetSize(onyxU32 width, onyxU32 height)
    {
        if ((width != m_Settings.Size[0]) || (height != m_Settings.Size[1]))
        {
            m_Settings.Size[0] = width;
            m_Settings.Size[1] = height;

            if (m_Settings.Mode == WindowMode::Windowed)
            {
                RECT rect { 0, 0, static_cast<onyxS32>(width), static_cast<onyxS32>(height) };
                ::AdjustWindowRectExForDpi(&rect, GetStyle(),FALSE, GetExtendedStyle(), GetDpiForWindow(m_WindowHandle));
                ::SetWindowPos(m_WindowHandle, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
            }

            m_OnResize(width, height);
        }
    }

    void Window::SetMinimumSize(const Vector2s32& minSize)
    {
        if (m_Settings.MinSize != minSize)
        {
            m_Settings.MinSize = minSize;
            VectorCompareMask compared = m_Settings.Size > m_Settings.MinSize;
            SetSize(Enums::HasAllFlags(compared, VectorCompareMask::X)  ? m_Settings.MinSize[1] : m_Settings.Size[0], Enums::HasAllFlags(compared, VectorCompareMask::Y) ? m_Settings.MinSize[1] : m_Settings.Size[1]);
        }
    }

    void Window::SetMaximumSize(const Vector2s32& maxSize)
    {
        if (m_Settings.MaxSize != maxSize)
        {
            m_Settings.MaxSize = maxSize;
            VectorCompareMask compared = m_Settings.Size > m_Settings.MaxSize;
            SetSize(Enums::HasAllFlags(compared, VectorCompareMask::X) ? m_Settings.MaxSize[1] : m_Settings.Size[0], Enums::HasAllFlags(compared, VectorCompareMask::Y) ? m_Settings.MaxSize[1] : m_Settings.Size[1]);
        }
    }

    void Window::SetWindowMode(WindowMode mode)
    {
        //if (m_Settings.m_Mode == mode)
        //    return;

        m_Settings.Mode = mode;

        HMONITOR monitor = ::MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX newMonitorInfo;
        ZeroMemory(&newMonitorInfo, sizeof(MONITORINFOEX));
        newMonitorInfo.cbSize = sizeof(MONITORINFOEX);
        ::GetMonitorInfo(monitor, &newMonitorInfo);

        //onyxS8 newMonitorIndex = static_cast<onyxS8>(Platform::GetMonitorIndexByName(newMonitorInfo.szDevice));
        //if (m_Settings.m_MonitorIndex != newMonitorIndex)
        //{
        //    RECT windowRect;
        //    windowRect.left = m_Settings.m_Position[0];
        //    windowRect.top = m_Settings.m_Position[1];
        //    windowRect.right = windowRect.left + m_Settings.m_Size[0];
        //    windowRect.bottom = windowRect.top + m_Settings.m_Size[1];

        //    HMONITOR normalWindowMonitor = ::MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
        //    MONITORINFOEX monitorInfo;
        //    ZeroMemory(&monitorInfo, sizeof(MONITORINFOEX));
        //    monitorInfo.cbSize = sizeof(MONITORINFOEX);
        //    ::GetMonitorInfo(normalWindowMonitor, &monitorInfo);

        //    onyxS8 normalMonitorIndex = static_cast<onyxS8>(Platform::GetMonitorIndexByName(monitorInfo.szDevice));
        //    if (normalMonitorIndex != newMonitorIndex)
        //    {
        //        // adjust window position to new monitor
        //        WINDOWPLACEMENT windowPlacement;
        //        GetWindowPlacement(m_WindowHandle, &windowPlacement);
        //        Vector2u32 monitorOffset(m_Settings.m_Position[0] - monitorInfo.rcWork.left, m_Settings.m_Position[1] - monitorInfo.rcWork.top);

        //        m_Settings.m_Position[0] = newMonitorInfo.rcWork.left + monitorOffset[0];
        //        m_Settings.m_Position[1] = newMonitorInfo.rcWork.top + monitorOffset[1];

        //        windowRect.left = m_Settings.m_Position[0];
        //        windowRect.top = m_Settings.m_Position[1];
        //        windowRect.right = windowRect.left + m_Settings.m_Size[0];
        //        windowRect.bottom = windowRect.top + m_Settings.m_Size[1];

        //        // fetch new monitor info
        //        monitor = ::MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
        //        ZeroMemory(&monitorInfo, sizeof(MONITORINFOEX));
        //        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        //        ::GetMonitorInfo(monitor, &monitorInfo);
        //    }
        //}

        // get current display mode
        DEVMODE devMode;
        ZeroMemory(&devMode, sizeof(devMode));
        devMode.dmSize = sizeof(devMode);
        ::EnumDisplaySettings(newMonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
        
        /*m_Settings.m_MonitorRefreshRate = static_cast<onyxU16>(devMode.dmDisplayFrequency);
        FindFullscreenMode(monitorIndex, Vector2u32(devMode.dmPelsWidth, devMode.dmPelsHeight), m_Settings.m_MonitorRefreshRate);

        if (myCurrentState.myAllowExclusive && myCurrentState.myFullscreen && myCurrentState.myExclusiveMode)
        {
            if (FindFullscreenMode(newMonitorIndex, myCurrentState.myFullscreenSize, myCurrentState.myFullscreenHz))
            {
                myCurrentState.myClientSize = myCurrentState.myFullscreenSize;
                myCurrentState.myHz = myCurrentState.myFullscreenHz;
            }
            else
            {
                myCurrentState.myExclusiveMode = false;
            }
        }*/

        DWORD style = WS_VISIBLE | GetStyle();
        if (mode == WindowMode::Fullscreen)
        {
            Vector2u32 pos(
                newMonitorInfo.rcMonitor.left,
                newMonitorInfo.rcMonitor.top);

            Vector2u32 size = { 1920, 1080 }; // init this fullscreen size properly

            ::SetWindowLongPtrW(m_WindowHandle, GWL_STYLE, style);
            ::SetWindowPos(m_WindowHandle, 0, pos[0], pos[1], size[0], size[1], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
        else if (mode == WindowMode::Borderless)
        {
            // fullscreen - resize our borderless window to fill closest monitor
            Vector2u32 pos(
                newMonitorInfo.rcMonitor.left,
                newMonitorInfo.rcMonitor.top);

            Vector2u32 size(
                devMode.dmPelsWidth ? devMode.dmPelsWidth : (newMonitorInfo.rcMonitor.right - newMonitorInfo.rcMonitor.left),
                devMode.dmPelsHeight ? devMode.dmPelsHeight : (newMonitorInfo.rcMonitor.bottom - newMonitorInfo.rcMonitor.top));

            // Hack to fix taskbar covering our window in fake fullscreen mode
            // Remove the WS_VISIBLE bit, then we set the window visible again (below with SWP_SHOWWINDOW).
            // Apparently this causes Windows to re-evaluate whether or not the window is "fullscreen",
            // and specifically, whether the taskbar should be behind our window or not.
            ::SetWindowLongPtrW(m_WindowHandle, GWL_STYLE, style & ~WS_VISIBLE);
            ::SetWindowPos(m_WindowHandle, 0, pos[0], pos[1], size[0], size[1], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        }
        else
        {
            // windowed - position and size the window, apply maximized status if needed.
            bool maximized = m_State == WindowState::Maximized;
            bool minimized = (m_State == WindowState::Minimized);

            Vector2u32 size = m_Settings.Size;
            // position of window in settings
            RECT rect = { m_Settings.Position[0], m_Settings.Position[1], m_Settings.Position[0] + static_cast<onyxS32>(size[0]), m_Settings.Position[1] + static_cast<onyxS32>(size[1])};

            // make sure the window is visible on at least one monitor
            /*if (!RectIntersectsAnyMonitor(rect))
            {
                rect.left = monitorInfo.rcMonitor.left + ((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) / 2) - (size.x / 2);
                rect.top = monitorInfo.rcMonitor.top + ((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) / 2) - (size.y / 2);
                rect.right = rect.left + size.x;
                rect.bottom = rect.top + size.y;
            }*/

            ::AdjustWindowRect(&rect, style, FALSE);
            ::SetWindowLongPtrW(m_WindowHandle, GWL_STYLE, style);
            ::SetWindowPos(m_WindowHandle, 0, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            ::ShowWindow(m_WindowHandle, minimized ? SW_SHOWMINIMIZED : (maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL));
        }

        // set topmost status
        LONG_PTR exStyle = ::GetWindowLongPtrW(m_WindowHandle, GWL_EXSTYLE);
        bool isTopMost = (exStyle & WS_EX_TOPMOST) != 0;
        bool wantTopMost = mode == WindowMode::Fullscreen && m_State >= WindowState::Background;
        if (isTopMost != wantTopMost)
        {
            if (!wantTopMost)
                ::SetWindowLongPtrW(m_WindowHandle, GWL_EXSTYLE, exStyle & ~WS_EX_TOPMOST);
            ::SetWindowPos(m_WindowHandle, wantTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOMOVE);
        }

        // make foreground window
        if (m_State == WindowState::Default)
        {
            Focus();
        }
    }

    void Window::SetState(WindowState state)
    {
        if (m_State != state)
        {
            switch (state)
            {
                case WindowState::None:
                case WindowState::Hidden:
                {
                    break;
                }
                case WindowState::Minimized:
                {
                    Minimize();
                    break;
                }
                case WindowState::Background:
                {
                    break;
                }
                case WindowState::Default:
                {
                    break;
                }
                case WindowState::Maximized:
                {
                    break;
                }
                case WindowState::Resizing:
                {
                    break;
                }
            }

            m_State = state;
        }
    }

    bool Window::GetRequiredExtensions(std::vector<const char*>& outExtensions) const
    {
        outExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        outExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        return true;
    }

    void Window::SetCursor(HCURSOR cursor)
    {
        m_Cursor = cursor;
        ::SetCursor(cursor);
    }
}
#endif