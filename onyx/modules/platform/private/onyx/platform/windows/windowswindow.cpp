#include <onyx/filesystem/imagefile.h>
#include <onyx/platform/windows/windowswindow.h>

#if ONYX_IS_WINDOWS

#include <onyx/encoding.h>
#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/platform/windows/windowskeycodes.h>
#include <onyx/platform/windows/windowsplatformcontext.h>
#include <onyx/profiler/profiler.h>

#include <dwmapi.h>
#include <Windows.h>

namespace {
HWND g_MainHwnd = nullptr;

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

LRESULT CALLBACK LowLevelMouseProc( int nCode, WPARAM wParam, LPARAM lParam ) {
    if ( ( nCode >= 0 ) && ( g_MainHwnd != nullptr ) ) {
        if ( wParam == WM_LBUTTONDOWN ) {
            PostMessage( g_MainHwnd, onyx::platform::windows::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN, 0, 0 );
            return 1;
        }
        if ( wParam == WM_LBUTTONUP ) {
            PostMessage( g_MainHwnd, onyx::platform::windows::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEUP, 0, 0 );
            return 1;
        }
    }

    return CallNextHookEx( nullptr, nCode, wParam, lParam );
}

::HICON CreateIconFromBitmap( const onyx::Span< onyx::uint8_t >& imageData, onyx::Vector2s32 imageSize ) {
    int i;
    ::HDC dc;
    ::HBITMAP color, mask;
    ::BITMAPV5HEADER bi;
    ::ICONINFO ii;
    unsigned char* target = nullptr;
    unsigned char* source = imageData.data();

    ZeroMemory( &bi, sizeof( bi ) );
    bi.bV5Size = sizeof( bi );
    bi.bV5Width = imageSize[ 0 ];
    bi.bV5Height = -imageSize[ 1 ];
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00ff0000;
    bi.bV5GreenMask = 0x0000ff00;
    bi.bV5BlueMask = 0x000000ff;
    bi.bV5AlphaMask = 0xff000000;

    dc = ::GetDC( nullptr );
    color = ::CreateDIBSection( dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&target, nullptr, (DWORD)0 );
    ReleaseDC( nullptr, dc );

    if ( !color ) {
        return nullptr;
    }

    mask = ::CreateBitmap( imageSize[ 0 ], imageSize[ 1 ], 1, 1, NULL );
    if ( !mask ) {
        DeleteObject( color );
        return nullptr;
    }

    for ( i = 0; i < imageSize[ 0 ] * imageSize[ 1 ]; i++ ) {
        target[ 0 ] = source[ 2 ];
        target[ 1 ] = source[ 1 ];
        target[ 2 ] = source[ 0 ];
        target[ 3 ] = source[ 3 ];
        target += 4;
        source += 4;
    }

    ZeroMemory( &ii, sizeof( ii ) );
    ii.fIcon = true;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = mask;
    ii.hbmColor = color;

    ::HICON handle = ::CreateIconIndirect( &ii );

    ::DeleteObject( color );
    ::DeleteObject( mask );

    if ( !handle ) {
        // return error
        return nullptr;
    }

    return handle;
}
} // namespace

namespace onyx::platform::windows {
Window::Window( PlatformContext& context, const WindowSettings& settings )
    : m_Settings( settings )
    , m_Context( &context ) {
    m_IsInitialized = false;
    myWakeFromSleepEvent = CreateEvent( nullptr, FALSE, FALSE, "Local\\WindowThread_WakeFromSleepEvent" );
    Start();

    m_IsInitialized.wait( false );
}

Window::~Window() {
    Stop( true );
    if ( myWakeFromSleepEvent != nullptr ) {
        ::CloseHandle( myWakeFromSleepEvent );
    }
}

void Window::Show() {
    ::ShowWindow( m_WindowHandle, SW_SHOW );
}

void Window::Minimize() {
    ::ShowWindow( m_WindowHandle, SW_MINIMIZE );
}

void Window::Maximize() {
    if ( ::IsWindowVisible( m_WindowHandle ) )
        ::ShowWindow( m_WindowHandle, SW_MAXIMIZE );
    else
        FitToMonitor();
}

void Window::Hide() {
    ::ShowWindow( m_WindowHandle, SW_HIDE );
}

void Window::Focus() {
    ::BringWindowToTop( m_WindowHandle );
    ::SetForegroundWindow( m_WindowHandle );
    ::SetFocus( m_WindowHandle );
}

void Window::RequestWindowAttention() {
    ::FlashWindow( m_WindowHandle, true );
}

void Window::ToggleCursor( bool enable ) {
    if ( enable )
        ReleaseCursor();
    else
        CaptureCursor();
}

void Window::UpdateCursorImage() {
    if ( m_CursorMode == CursorMode::Normal || m_CursorMode == CursorMode::Captured ) {
        // add support for custom cursors
        ::SetCursor( m_Cursor );
    } else
        ::SetCursor( nullptr );
}

int64_t Window::OnWindowProc( HWND hWnd, uint32_t message, uint64_t wParam, int64_t lParam ) {
    using namespace onyx;
    Window* pThis; // our "this" pointer will go here
    if ( message == WM_NCCREATE ) {
        // Recover the "this" pointer which was passed as a parameter to CreateWindow(Ex).
        LPCREATESTRUCT lpcs = std::bit_cast< LPCREATESTRUCT >( lParam );
        pThis = static_cast< Window* >( lpcs->lpCreateParams );
        // Put the value in a safe place for future use
        SetWindowLongPtr( hWnd, GWLP_USERDATA, std::bit_cast< LONG_PTR >( pThis ) );
    } else {
        // Recover the "this" pointer from where our WM_NCCREATE handler
        // stashed it.
        pThis = std::bit_cast< Window* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
    }

    if ( pThis && pThis->HandleWindowMessages( message, wParam, lParam ) ) {
        return 1;
    }

    return ::DefWindowProcW( hWnd, message, wParam, lParam );
}

void Window::OnStart() {
    ONYX_PROFILE_SET_THREAD( WindowThread )
    m_Cursor = ::LoadCursor( nullptr, IDC_ARROW );

    CreateNativeWindow();

    /*g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandle(nullptr), 0);*/

    ONYX_ASSERT( m_WindowHandle, "Could not create window!" );
    SetState( WindowState::Default );
    SetWindowMode( m_Settings.Mode );

    ::UpdateWindow( m_WindowHandle );

    m_IsInitialized = true;
    m_IsInitialized.notify_one();
}

void Window::OnUpdate() {
    MSG msg;
    while ( IsRunning() ) {
        ONYX_PROFILE_SECTION( OnUpdate )

        while ( ::PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) ) {
            ::TranslateMessage( &msg );
            ::DispatchMessageW( &msg );

            if ( msg.message == WM_QUIT ) {
                break;
            }
        }

        ::MsgWaitForMultipleObjects( 1, &myWakeFromSleepEvent, false, 1000, QS_ALLINPUT );
    }
}

void Window::OnStop() {}

void Window::CreateNativeWindow() {
    HINSTANCE instance = ::GetModuleHandle( nullptr );

    String_U16 windowTitle = Encoding::utf8_to_utf16( m_Settings.Title );
    HICON icon = static_cast< HICON >( m_Settings.Icon );
    WNDCLASSEXW wndClass;

    wndClass.cbSize = sizeof( WNDCLASSEX );
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc = OnWindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = instance;
    wndClass.hIcon = icon;
    wndClass.hCursor = m_Cursor;
    wndClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = (LPCWSTR)windowTitle.data();
    wndClass.hIconSm = icon;

    if ( !::RegisterClassExW( &wndClass ) ) {
        ONYX_ASSERT( false, "Failed registering window class!" );
        return;
    }

    int32_t screenWidth = static_cast< int32_t >( ::GetSystemMetrics( SM_CXSCREEN ) );
    int32_t screenHeight = static_cast< int32_t >( ::GetSystemMetrics( SM_CYSCREEN ) );

    bool isFullscreen = ( m_Settings.Mode == WindowMode::Fullscreen ) || ( m_Settings.Mode == WindowMode::Borderless );

    DWORD dwExStyle = GetExtendedStyle();
    DWORD dwStyle = GetStyle();

    if ( m_Settings.Position.X == -1 ) {
        m_Settings.Position.X = std::clamp( ( screenWidth - m_Settings.Size.X ) / 2, 0, screenWidth );
    }

    if ( m_Settings.Position.Y == -1 ) {
        m_Settings.Position.Y = std::clamp( ( screenHeight - m_Settings.Size.Y ) / 2, 0, screenHeight );
    }

    RECT windowRect;
    windowRect.left = isFullscreen ? 0 : m_Settings.Position.X;
    windowRect.top = isFullscreen ? 0 : m_Settings.Position.Y;
    windowRect.right = isFullscreen ? screenWidth : m_Settings.Size.X;
    windowRect.bottom = isFullscreen ? screenHeight : m_Settings.Size.Y;

    ::AdjustWindowRectEx( &windowRect, dwStyle, false, dwExStyle );

    m_WindowHandle = ::CreateWindowW( (LPCWSTR)windowTitle.data(),
                                      (LPCWSTR)windowTitle.data(),
                                      dwStyle,
                                      std::max( windowRect.left, 0l ),
                                      std::max( windowRect.top, 0l ),
                                      windowRect.right - windowRect.left,
                                      windowRect.bottom - windowRect.top,
                                      NULL, // parent window
                                      NULL,
                                      instance,
                                      this );

    g_MainHwnd = m_WindowHandle;
}

void Window::FitToMonitor() {
    RECT rect;
    DWORD style;
    MONITORINFO mi;

    ::GetMonitorInfoW( MonitorFromWindow( m_WindowHandle, MONITOR_DEFAULTTONEAREST ), &mi );

    rect = mi.rcWork;

    if ( m_Settings.MaxSize[ 0 ] != 0 && m_Settings.MaxSize[ 1 ] != 0 ) {
        rect.right = std::min( rect.right, rect.left + m_Settings.MaxSize[ 0 ] );
        rect.bottom = std::min( rect.bottom, rect.top + m_Settings.MaxSize[ 1 ] );
    }

    style = ::GetWindowLongW( m_WindowHandle, GWL_STYLE );
    style |= WS_MAXIMIZE;
    ::SetWindowLongW( m_WindowHandle, GWL_STYLE, style );

    bool hasBorder = false;
    if ( hasBorder ) {
        const DWORD exStyle = ::GetWindowLongW( m_WindowHandle, GWL_EXSTYLE );

        const UINT dpi = GetDpiForWindow( m_WindowHandle );
        ::AdjustWindowRectExForDpi( &rect, style, FALSE, exStyle, dpi );
        ::OffsetRect( &rect, 0, GetSystemMetricsForDpi( SM_CYCAPTION, dpi ) );

        rect.bottom = std::min( rect.bottom, mi.rcWork.bottom );
    }

    SetWindowPos( m_WindowHandle,
                  HWND_TOP,
                  rect.left,
                  rect.top,
                  rect.right - rect.left,
                  rect.bottom - rect.top,
                  SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED );

    m_ResizeSignal.Dispatch( Vector2s32( rect.right - rect.left, rect.bottom - rect.top ) );
}

void Window::CaptureCursor() {
    RECT clipRect;
    ::GetClientRect( m_WindowHandle, &clipRect );
    ::ClientToScreen( m_WindowHandle, (POINT*)&clipRect.left );
    ::ClientToScreen( m_WindowHandle, (POINT*)&clipRect.right );
    ::ClipCursor( &clipRect );
}

void Window::ReleaseCursor() {
    ClipCursor( nullptr );
}

int32_t Window::GetStyle() {
    DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    if ( m_Settings.Mode == WindowMode::Fullscreen ) {
        windowStyle |= WS_POPUP;

    } else if ( m_Settings.Mode == WindowMode::Borderless ) {
        // borderless fullscreen
        windowStyle |= WS_POPUP;
    } else {
        // windowed
        bool allowBorder = true;
        bool allowResize = true;
        windowStyle |= allowBorder ? ( ( allowResize ? WS_OVERLAPPEDWINDOW
                                                     : ( WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX ) ) )
                                   : 0;
    }

    return windowStyle;
}

int32_t Window::GetExtendedStyle() {
    DWORD style = WS_EX_APPWINDOW;

    if ( m_Settings.Mode == WindowMode::Fullscreen )
        style |= WS_EX_TOPMOST;

    return style;
}

bool Window::HandleWindowMessages( uint32_t messageType, uint64_t wParam, int64_t lParam ) {
    ONYX_PROFILE_SECTION( HandleWindowMessages )

    // TODO: we need to return an outValue and handled bool as some events require to return 0 to the windows api
    static RECT borderThickness;

    switch ( messageType ) {
    case WM_CLOSE: {
        m_CloseSignal.Dispatch();
        ::DestroyWindow( m_WindowHandle );
        ::PostQuitMessage( 0 );
        break;
    }
    case WM_CREATE: {
        if ( m_Settings.HasTitleBar )
            break;

        // find border thickness
        ::SetRectEmpty( &borderThickness );
        if ( GetWindowLongPtr( m_WindowHandle, GWL_STYLE ) & WS_THICKFRAME ) {
            ::AdjustWindowRectEx( &borderThickness,
                                  GetWindowLongPtr( m_WindowHandle, GWL_STYLE ) & ~WS_CAPTION,
                                  FALSE,
                                  NULL );
            borderThickness.left *= -1;
            borderThickness.top *= -1;
        } else // if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_BORDER)
        {
            ::SetRect( &borderThickness, 4, 4, 4, 4 );
        }

        //::MARGINS margins = {0};
        //::DwmExtendFrameIntoClientArea(m_WindowHandle, &margins);
        ::SetWindowPos( m_WindowHandle, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED );

        break;
    }
    case WM_SYSCOMMAND: {
        switch ( wParam & 0xfff0 ) {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER: {
            if ( m_Settings.Mode == WindowMode::Fullscreen ) {
                // We are running in full screen mode, so disallow
                // screen saver and screen blanking
                return 0;
            } else
                break;
        }

        // User trying to access application menu using ALT?
        case SC_KEYMENU: {
            if ( m_Settings.HasMenu == false )
                return 0;

            break;
        }
        }
        break;
    }
    case WM_CAPTURECHANGED: {
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
    case WM_ERASEBKGND: {
        return true;
    }
    case WM_ACTIVATE: {
        if ( m_Settings.HasTitleBar )
            break;
    }
    case WM_NCACTIVATE:
    case WM_NCPAINT: {
        // Prevent title bar from being drawn after restoring a minimized
        // undecorated window
        if ( m_Settings.HasTitleBar == false )
            return true;

        break;
    }
    case WM_NCHITTEST: {
        // TODO: Implement borderless nom maximized window titlebar for custom tile bars
        // if (m_Settings.m_HasTitleBar)
        //    break;

        // uint32_t x = lParam & 0xFFFF;
        // uint32_t y = (lParam >> 16) & 0xFFFF;
        // POINT pt = { x, y };
        // ScreenToClient(m_WindowHandle, &pt);
        // RECT rc;
        // GetClientRect(m_WindowHandle, &rc);

        // int titlebarHittest = 0;
        ////_glfwInputTitleBarHitTest(m_WindowHandle, pt.x, pt.y, &titlebarHittest);

        // if (titlebarHittest)
        //{
        //     return HTCAPTION;
        // }
        // else
        //{
        //     enum { left = 1, top = 2, right = 4, bottom = 8 };
        //     int hit = 0;
        //     if (pt.x < borderThickness.left)               hit |= left;
        //     if (pt.x > rc.right - borderThickness.right)   hit |= right;
        //     if (pt.y < borderThickness.top)                hit |= top;
        //     if (pt.y > rc.bottom - borderThickness.bottom) hit |= bottom;

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
    case WM_INPUTLANGCHANGE: {
        break;
    }
    case WM_SETFOCUS: {
        if ( m_CursorMode == CursorMode::Disabled )
            ToggleCursor( false );
        else if ( m_CursorMode == CursorMode::Captured )
            CaptureCursor();

        m_FocusSignal.Dispatch( true );

        break;
    }
    case WM_KILLFOCUS: {
        if ( m_CursorMode == CursorMode::Disabled )
            ToggleCursor( true );
        else if ( m_CursorMode == CursorMode::Captured )
            ReleaseCursor();

        /*WindowState state = m_State;
        if (m_Settings.Mode == WindowMode::Fullscreen)
        {
            state = m_Settings.Mode == WindowMode::Fullscreen ? WindowState::Minimized : WindowState::Background;
        }*/

        m_FocusSignal.Dispatch( false );
        break;
    }
    case WM_PAINT: {
        LPPAINTSTRUCT lpPaint = {};
        if ( ::BeginPaint( m_WindowHandle, lpPaint ) != nullptr ) {
            ::EndPaint( m_WindowHandle, lpPaint );
            return true;
        }
        break;
    }
    case WM_NCCALCSIZE: {
        if ( m_Settings.HasTitleBar || ( wParam == false ) )
            break;

        uint32_t dpi = ::GetDpiForWindow( m_WindowHandle );

        int32_t frameWidth = ::GetSystemMetricsForDpi( SM_CXFRAME, dpi );
        int32_t frameHeight = ::GetSystemMetricsForDpi( SM_CYFRAME, dpi );

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
    case WM_GETDPISCALEDSIZE: {
        // if (m_Settings.m_ScaleToMonitor)
        //     break;

        // Adjust the window size to keep the content area size constant
        ::RECT source;
        ::RECT target;
        ::SIZE* size = (::SIZE*)lParam;

        ::AdjustWindowRectExForDpi( &source, GetStyle(), false, GetExtendedStyle(), GetDpiForWindow( m_WindowHandle ) );
        ::AdjustWindowRectExForDpi( &target, GetStyle(), false, GetExtendedStyle(), LOWORD( wParam ) );

        size->cx += ( target.right - target.left ) - ( source.right - source.left );
        size->cy += ( target.bottom - target.top ) - ( source.bottom - source.top );
        return true;
    }

    case WM_SETCURSOR: {
        if ( LOWORD( lParam ) == HTCLIENT ) {
            UpdateCursorImage();
            return true;
        }

        break;
    }
    case WM_DROPFILES: {
        return false;
    }
    case WM_UNICHAR: {
        if ( wParam == UNICODE_NOCHAR )
            return true;

        return false;
    }
    case WM_SIZE: {
        if ( m_State != WindowState::Resizing ) {
            bool isMinimized = wParam == SIZE_MINIMIZED;
            bool isMaximized = ( wParam == SIZE_MAXIMIZED ) ||
                               ( ( m_State == WindowState::Maximized ) && wParam != SIZE_RESTORED );

            if ( isMinimized ) {
                SetState( WindowState::Minimized );
                return false;
            }

            if ( isMaximized )
                Maximize();

            uint32_t width = lParam & 0xFFFF;
            uint32_t height = ( lParam >> 16 ) & 0xFFFF;
            SetState( WindowState::Default );
            SetSize( numericCast< int32_t >( width ), numericCast< int32_t >( height ) );
        }

        break;
    }
    case WM_MOVE: {
        break;
    }
    case WM_WINDOWPOSCHANGING: {
        break;
    }
    case WM_WINDOWPOSCHANGED: {
        break;
    }
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO minMaxInfo = std::bit_cast< LPMINMAXINFO >( lParam );
        minMaxInfo->ptMinTrackSize.x = 64;
        minMaxInfo->ptMinTrackSize.y = 64;
        break;
    }
    case WM_ENTERSIZEMOVE: {
        m_State = WindowState::Resizing;
        break;
    }
    case WM_EXITSIZEMOVE: {
        RECT clientRect;
        ::GetClientRect( m_WindowHandle, &clientRect );
        SetSize( clientRect.right, clientRect.bottom );
        SetState( WindowState::Default );
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP: {
        const bool hasReleased = ( ( ( lParam >> 16 ) & 0xFFFF ) & KF_UP ) == KF_UP;

        input::KeyboardEvent event;
        event.Key = ConvertWindowsKey( wParam, lParam );
        event.State = ( hasReleased
                            ? input::ButtonState::Up
                            : ( IsKeyRepeated( lParam ) ? input::ButtonState::Repeat : input::ButtonState::Down ) );
        event.Char = 0;

        input::InputSystem& inputSystem = m_Context->GetInputSystem();
        inputSystem.AddEvent( event );

        return true;
    }
    case WM_CHAR:
    case WM_SYSCHAR: {
        input::KeyboardEvent event;
        event.State = input::ButtonState::Down;
        event.Key = ConvertWindowsKey( wParam, lParam );
        event.Char = static_cast< uint16_t >( wParam );

        input::InputSystem& inputSystem = m_Context->GetInputSystem();
        inputSystem.AddEvent( event );
        return true;
    }
    case WM_MOUSEMOVE: {
        input::MousePositionEvent event;
        event.Position = { static_cast< int32_t >( lParam & 0xFFFF ),
                           static_cast< int32_t >( ( lParam >> 16 ) & 0xFFFF ) };

        input::InputSystem& inputSystem = m_Context->GetInputSystem();
        inputSystem.AddEvent( event );
        return true;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    case ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN:
    case ONYX_WM_SYSTEM_PRIMARY_MOUSEUP: {
        input::MouseButtonEvent event;
        if ( messageType == WM_LBUTTONDOWN || messageType == WM_LBUTTONUP ||
             messageType == ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN || messageType == ONYX_WM_SYSTEM_PRIMARY_MOUSEUP )
            event.Button = input::MouseButton::Button_1;
        else if ( messageType == WM_RBUTTONDOWN || messageType == WM_RBUTTONUP )
            event.Button = input::MouseButton::Button_2;
        else if ( messageType == WM_MBUTTONDOWN || messageType == WM_MBUTTONUP )
            event.Button = input::MouseButton::Button_3;
        else {
            const int32_t extraMouseButton = ( wParam >> 16 ) & 0xFFFF;
            event.Button = static_cast< input::MouseButton >( static_cast< int32_t >( input::MouseButton::Button_3 ) +
                                                              extraMouseButton );
        }

        const bool hasReleased = ( ( messageType == WM_LBUTTONUP ) || ( messageType == WM_RBUTTONUP ) ||
                                   ( messageType == WM_MBUTTONUP ) || ( messageType == WM_XBUTTONUP ) ||
                                   ( messageType == ONYX_WM_SYSTEM_PRIMARY_MOUSEUP ) );

        event.State = hasReleased ? input::ButtonState::Up : input::ButtonState::Down;

        input::InputSystem& inputSystem = m_Context->GetInputSystem();
        inputSystem.AddEvent( event );

        return true;
    }

    case WM_MOUSEWHEEL: {
        input::MouseAxisEvent event;
        event.Value = (int16_t)HIWORD( wParam ) / (int16_t)WHEEL_DELTA;

        input::InputSystem& inputSystem = m_Context->GetInputSystem();
        inputSystem.AddEvent( event );

        return true;
    }

    case WM_DEVICECHANGE: {
#if ONYX_USE_GAMEINPUT
        // if (wParam == DBT_DEVICEARRIVAL)
        {
            bool hasHandled = m_Input.OnDeviceChange();
            if ( hasHandled )
                return true;
        }
#else
        break;
#endif
    }
    case ONYX_WM_SYSTEM_MOUSEHOOK: {
        if ( wParam == 0 ) {
            UnhookWindowsHookEx( m_SystemMouseHook );
            m_SystemMouseHook = nullptr;
        } else {
            m_SystemMouseHook = SetWindowsHookEx( WH_MOUSE_LL, LowLevelMouseProc, ::GetModuleHandle( nullptr ), 0 );
        }
        return true;
    }
    }

    return false;
}

void Window::SetTitle( StringView title ) {
    if ( m_Settings.Title != title ) {
        m_Settings.Title = title;

        String_U16 windowTitle = Encoding::utf8_to_utf16( m_Settings.Title );
        ::SetWindowTextW( m_WindowHandle, (LPCWSTR)windowTitle.data() );
    }
}

void Window::SetIcon( const FilePath& path ) {
    file_system::ImageFile image( path );
    HICON icon = CreateIconFromBitmap( image.GetData(), image.GetSize() );

    SendMessage( m_WindowHandle, WM_SETICON, ICON_BIG, (LPARAM)icon );
    SendMessage( m_WindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)icon );
}

void Window::SetSize( int32_t width, int32_t height ) {
    if ( ( width != m_Settings.Size[ 0 ] ) || ( height != m_Settings.Size[ 1 ] ) ) {
        m_Settings.Size[ 0 ] = width;
        m_Settings.Size[ 1 ] = height;

        if ( m_Settings.Mode == WindowMode::Windowed ) {
            RECT rect{ 0, 0, static_cast< int32_t >( width ), static_cast< int32_t >( height ) };
            ::AdjustWindowRectExForDpi( &rect,
                                        GetStyle(),
                                        FALSE,
                                        GetExtendedStyle(),
                                        GetDpiForWindow( m_WindowHandle ) );
            ::SetWindowPos( m_WindowHandle,
                            HWND_TOP,
                            0,
                            0,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER );
        }

        m_ResizeSignal.Dispatch( Vector2s32{ width, height } );
    }
}

void Window::SetMinimumSize( const Vector2s32& minSize ) {
    if ( m_Settings.MinSize != minSize ) {
        m_Settings.MinSize = minSize;
        VectorComponentMask compared = m_Settings.Size > m_Settings.MinSize;
        SetSize(
            enums::HasAllFlags( compared, VectorComponentMask::X ) ? m_Settings.MinSize[ 1 ] : m_Settings.Size[ 0 ],
            enums::HasAllFlags( compared, VectorComponentMask::Y ) ? m_Settings.MinSize[ 1 ] : m_Settings.Size[ 1 ] );
    }
}

void Window::SetMaximumSize( const Vector2s32& maxSize ) {
    if ( m_Settings.MaxSize != maxSize ) {
        m_Settings.MaxSize = maxSize;
        VectorComponentMask compared = m_Settings.Size > m_Settings.MaxSize;
        SetSize(
            enums::HasAllFlags( compared, VectorComponentMask::X ) ? m_Settings.MaxSize[ 1 ] : m_Settings.Size[ 0 ],
            enums::HasAllFlags( compared, VectorComponentMask::Y ) ? m_Settings.MaxSize[ 1 ] : m_Settings.Size[ 1 ] );
    }
}

void Window::SetWindowMode( WindowMode mode ) {
    // if (m_Settings.m_Mode == mode)
    //     return;

    m_Settings.Mode = mode;

    HMONITOR monitor = ::MonitorFromWindow( m_WindowHandle, MONITOR_DEFAULTTONEAREST );
    MONITORINFOEX newMonitorInfo;
    ZeroMemory( &newMonitorInfo, sizeof( MONITORINFOEX ) );
    newMonitorInfo.cbSize = sizeof( MONITORINFOEX );
    ::GetMonitorInfo( monitor, &newMonitorInfo );

    // int8_t newMonitorIndex = static_cast<int8_t>(platform::GetMonitorIndexByName(newMonitorInfo.szDevice));
    // if (m_Settings.m_MonitorIndex != newMonitorIndex)
    //{
    //     RECT windowRect;
    //     windowRect.left = m_Settings.m_Position[0];
    //     windowRect.top = m_Settings.m_Position[1];
    //     windowRect.right = windowRect.left + m_Settings.m_Size[0];
    //     windowRect.bottom = windowRect.top + m_Settings.m_Size[1];

    //    HMONITOR normalWindowMonitor = ::MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
    //    MONITORINFOEX monitorInfo;
    //    ZeroMemory(&monitorInfo, sizeof(MONITORINFOEX));
    //    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    //    ::GetMonitorInfo(normalWindowMonitor, &monitorInfo);

    //    int8_t normalMonitorIndex = static_cast<int8_t>(platform::GetMonitorIndexByName(monitorInfo.szDevice));
    //    if (normalMonitorIndex != newMonitorIndex)
    //    {
    //        // adjust window position to new monitor
    //        WINDOWPLACEMENT windowPlacement;
    //        GetWindowPlacement(m_WindowHandle, &windowPlacement);
    //        Vector2u32 monitorOffset(m_Settings.m_Position[0] - monitorInfo.rcWork.left, m_Settings.m_Position[1] -
    //        monitorInfo.rcWork.top);

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
    ZeroMemory( &devMode, sizeof( devMode ) );
    devMode.dmSize = sizeof( devMode );
    ::EnumDisplaySettings( newMonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode );

    /*m_Settings.m_MonitorRefreshRate = static_cast<uint16_t>(devMode.dmDisplayFrequency);
    FindFullscreenMode(monitorIndex, Vector2u32(devMode.dmPelsWidth, devMode.dmPelsHeight),
    m_Settings.m_MonitorRefreshRate);

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
    DWORD exStyle = GetExtendedStyle();
    if ( mode == WindowMode::Fullscreen ) {
        Vector2u32 pos( newMonitorInfo.rcMonitor.left, newMonitorInfo.rcMonitor.top );

        Vector2u32 size = { 1920, 1080 }; // init this fullscreen size properly

        ::SetWindowLongPtrW( m_WindowHandle, GWL_STYLE, style );
        ::SetWindowPos( m_WindowHandle,
                        0,
                        pos[ 0 ],
                        pos[ 1 ],
                        size[ 0 ],
                        size[ 1 ],
                        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
    } else if ( mode == WindowMode::Borderless ) {
        // fullscreen - resize our borderless window to fill closest monitor
        Vector2u32 pos( newMonitorInfo.rcMonitor.left, newMonitorInfo.rcMonitor.top );

        Vector2u32 size( devMode.dmPelsWidth ? devMode.dmPelsWidth
                                             : ( newMonitorInfo.rcMonitor.right - newMonitorInfo.rcMonitor.left ),
                         devMode.dmPelsHeight ? devMode.dmPelsHeight
                                              : ( newMonitorInfo.rcMonitor.bottom - newMonitorInfo.rcMonitor.top ) );

        // Hack to fix taskbar covering our window in fake fullscreen mode
        // Remove the WS_VISIBLE bit, then we set the window visible again (below with SWP_SHOWWINDOW).
        // Apparently this causes Windows to re-evaluate whether or not the window is "fullscreen",
        // and specifically, whether the taskbar should be behind our window or not.
        ::SetWindowLongPtrW( m_WindowHandle, GWL_STYLE, style & ~WS_VISIBLE );
        ::SetWindowPos( m_WindowHandle,
                        0,
                        pos[ 0 ],
                        pos[ 1 ],
                        size[ 0 ],
                        size[ 1 ],
                        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
    } else {
        // windowed - position and size the window, apply maximized status if needed.
        bool maximized = m_State == WindowState::Maximized;
        bool minimized = ( m_State == WindowState::Minimized );

        // position of window in settings
        RECT rect = { m_Settings.Position.X,
                      m_Settings.Position.Y,
                      m_Settings.Position.X + m_Settings.Size.X,
                      m_Settings.Position.Y + m_Settings.Size.Y };

        // make sure the window is visible on at least one monitor
        /*if (!RectIntersectsAnyMonitor(rect))
        {
            rect.left = monitorInfo.rcMonitor.left + ((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) / 2) -
        (size.x / 2); rect.top = monitorInfo.rcMonitor.top + ((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)
        / 2) - (size.y / 2); rect.right = rect.left + size.x; rect.bottom = rect.top + size.y;
        }*/

        ::AdjustWindowRectEx( &rect, style, FALSE, exStyle );
        ::SetWindowLongPtrW( m_WindowHandle, GWL_STYLE, style );
        ::SetWindowPos( m_WindowHandle,
                        0,
                        std::max( rect.left, 0l ),
                        std::max( rect.top, 0l ),
                        ( rect.right - rect.left ),
                        ( rect.bottom - rect.top ),
                        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
        ::ShowWindow( m_WindowHandle, minimized ? SW_SHOWMINIMIZED : ( maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL ) );
    }

    // set topmost status
    LONG_PTR extendedStyle = ::GetWindowLongPtrW( m_WindowHandle, GWL_EXSTYLE );
    bool isTopMost = ( extendedStyle & WS_EX_TOPMOST ) != 0;
    bool wantTopMost = mode == WindowMode::Fullscreen && m_State >= WindowState::Background;
    if ( isTopMost != wantTopMost ) {
        if ( !wantTopMost )
            ::SetWindowLongPtrW( m_WindowHandle, GWL_EXSTYLE, extendedStyle & ~WS_EX_TOPMOST );
        ::SetWindowPos( m_WindowHandle,
                        wantTopMost ? HWND_TOPMOST : HWND_NOTOPMOST,
                        0,
                        0,
                        0,
                        0,
                        SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOMOVE );
    }

    // make foreground window
    if ( m_State == WindowState::Default ) {
        Focus();
    }
}

void Window::SetState( WindowState state ) {
    if ( m_State != state ) {
        switch ( state ) {
        case WindowState::None:
        case WindowState::Hidden: {
            break;
        }
        case WindowState::Minimized: {
            Minimize();
            break;
        }
        case WindowState::Background: {
            break;
        }
        case WindowState::Default: {
            break;
        }
        case WindowState::Maximized: {
            break;
        }
        case WindowState::Resizing: {
            break;
        }
        }

        m_State = state;
    }
}

bool Window::GetRequiredExtensions( std::vector< const char* >& outExtensions ) const {
    outExtensions.push_back( "VK_KHR_win32_surface" );
    return true;
}

void Window::SetCursor( HCURSOR cursor ) {
    m_Cursor = cursor;
    ::SetCursor( cursor );
}

void Window::EnableSystemMouseCapture( bool enable ) {
    ::PostMessage( m_WindowHandle, ONYX_WM_SYSTEM_MOUSEHOOK, enable ? 1 : 0, 0 );
}
} // namespace onyx::platform::windows
#endif