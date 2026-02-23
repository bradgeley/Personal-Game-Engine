// Bradley Christensen - 2022-2026

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "Win32Window.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(Window* window, HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SharedWindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);



//----------------------------------------------------------------------------------------------------------------------
Win32Window::Win32Window(WindowConfig const& config) : Window(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::BeginFrame()
{
    Window::BeginFrame();

    RunMessagePump();
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::Shutdown()
{
    Window::Shutdown();

    if (IsValid())
    {
        ::SetWindowLongPtr((HWND) m_windowHandle, GWLP_USERDATA, 0);
        DestroyWindow((HWND) m_windowHandle);
        m_windowHandle = nullptr;
        m_displayContext = nullptr;
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool Win32Window::IsValid() const
{
    if (m_windowHandle != nullptr && m_displayContext != nullptr)
    {
        return IsWindow((HWND) m_windowHandle);
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::SetWindowTitle(std::string const& newTitle)
{
    std::wstring wNewTitle = std::wstring(newTitle.begin(), newTitle.end());
    ::SetWindowText((HWND) m_windowHandle, wNewTitle.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
void* Win32Window::GetHWND() const
{
    return m_windowHandle;
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::Minimize()
{
    ::ShowWindow((HWND) GetHWND(), SW_MINIMIZE);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Win32Window::GetDesiredWindowResolution() const
{
    if (m_userSettings.m_windowMode == WindowMode::Windowed)
    {
        return m_userSettings.m_windowedResolution;
    }

    RECT desktopRect;
    HWND desktop = GetDesktopWindow();
    ::GetWindowRect(desktop, &desktopRect);

    return IntVec2(desktopRect.right, desktopRect.bottom);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Win32Window::GetMouseClientPosition(bool originBottomLeft) const
{
    POINT origin = { 0, 0 };
    ::ClientToScreen((HWND) m_windowHandle, &origin);
    
    POINT mousePos;
    ::GetCursorPos(&mousePos);
    IntVec2 result = IntVec2(mousePos.x - origin.x, mousePos.y - origin.y);
    if (originBottomLeft)
    {
        RECT clientRect;
        ::GetClientRect((HWND) m_windowHandle, &clientRect);
        result.y = clientRect.bottom - result.y;
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 Win32Window::GetMouseClientRelativePosition(bool originBottomLeft) const
{
    Vec2 relativePosition = Vec2(GetMouseClientPosition(originBottomLeft));
    RECT clientRect;
    ::GetClientRect((HWND) m_windowHandle, &clientRect);
    relativePosition.y /= static_cast<float>(clientRect.bottom);
    relativePosition.x /= static_cast<float>(clientRect.right);
    return relativePosition;
}



//----------------------------------------------------------------------------------------------------------------------
bool Win32Window::GiveFocus()
{
    if (m_windowHandle)
    {
        ::SetFocus((HWND) m_windowHandle);
    }
    return m_windowHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::RefreshWindowSize()
{
    SetWindowLongPtr((HWND) m_windowHandle, GWL_STYLE, GetWindowStyleFlags());
    SetWindowLongPtr((HWND) m_windowHandle, GWL_EXSTYLE, GetWindowStyleExFlags());

    IntVec2 windowResolution = GetActualWindowResolution();

    RECT rect = { 0,0, windowResolution.x, windowResolution.y };
    BOOL result = ::AdjustWindowRectEx(&rect, GetWindowStyleFlags(), FALSE, 0);

    result |= ::SetWindowPos(
        (HWND) m_windowHandle,
        nullptr,
        0, 0,
        windowResolution.x, windowResolution.y,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
    );

    ASSERT_OR_DIE(result == TRUE, StringUtils::StringF("Failed to set window size: error code %i", GetLastError()));
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::HandleManualMove()
{
    if (m_isManuallyResizing)
    {
        RECT rect;
        ::GetWindowRect((HWND) GetHWND(), &rect);
        m_lastKnownWindowedPos = IntVec2(rect.left, rect.top);
    }
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Win32Window::GetWindowStyleFlags() const
{
    uint32_t windowStyleFlags = 0;

    switch (m_userSettings.m_windowMode)
    {
    case WindowMode::Windowed:
    {
        windowStyleFlags |= WS_OVERLAPPEDWINDOW;
        break;
    }
    case WindowMode::Fullscreen:
        // Fallthrough
    case WindowMode::Borderless:
    {
        windowStyleFlags |= WS_POPUP;
        break;
    }
    }
    return windowStyleFlags;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Win32Window::GetWindowStyleExFlags() const
{
    uint32_t windowStyleExFlags = 0;
    switch (m_userSettings.m_windowMode)
    {
    case WindowMode::Windowed:
    {
        break;
    }
    case WindowMode::Fullscreen:
        // Fallthrough
    case WindowMode::Borderless:
    {
        windowStyleExFlags |= WS_EX_APPWINDOW;
        break;
    }
    }
    return windowStyleExFlags;
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::MakeChildOf(Window* parentWindow)
{
	Win32Window* win32Window = dynamic_cast<Win32Window*>(parentWindow);
    ::SetWindowLongPtr((HWND) m_windowHandle, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(win32Window->GetHWND()));
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::MakeWindow()
{
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_OWNDC;
    wcex.lpfnWndProc    = SharedWindowsMessageHandlingProcedure;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = GetModuleHandle(NULL);
    wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = TEXT("Main Window");
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION); 
    ::RegisterClassEx(&wcex);
    
    DWORD windowStyleFlags = GetWindowStyleFlags();
    DWORD windowStyleExFlags = GetWindowStyleExFlags();

    // Convert window title to a wstring
    std::string titleString = m_config.m_windowTitle;
    std::wstring titleWString = std::wstring(titleString.begin(), titleString.end());

    IntVec2 windowResolution = GetDesiredWindowResolution();

    HWND hwnd = ::CreateWindowEx(
        windowStyleExFlags,
        wcex.lpszClassName,
        titleWString.data(),
        windowStyleFlags,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowResolution.x,
        windowResolution.y,
        nullptr,
        nullptr,
        wcex.hInstance,
        this);

    ASSERT_OR_DIE(hwnd != nullptr, "Failed to create window.");

    m_windowHandle = hwnd;
    m_displayContext = GetDC(hwnd);
    m_renderTargetID = g_renderer->MakeSwapchainRenderTarget(m_windowHandle, windowResolution);

    if (m_config.m_takeFocusWhenCreated)
    {
        ::ShowWindow(hwnd, SW_SHOW);
    }
    else
    {
        ::ShowWindow(hwnd, SW_SHOW);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::RunMessagePump()
{
   MSG queuedMessage;
   for (;;)
   {
      const BOOL wasMessagePresent = ::PeekMessage(&queuedMessage, nullptr, 0, 0, PM_REMOVE);
      if (!wasMessagePresent)
      {
         break;
      }

      ::TranslateMessage(&queuedMessage);
      ::DispatchMessage(&queuedMessage);
   }
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::WindowModeChanged(WindowMode previousMode)
{
    RefreshWindowTitle();

    HWND hwnd = (HWND) m_windowHandle;

    // Restore window if minimized
    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(wp);
    ::GetWindowPlacement(hwnd, &wp);
    if (wp.showCmd == SW_SHOWMINIMIZED)
    {
        wp.showCmd = SW_SHOWNORMAL;
        ::SetWindowPlacement(hwnd, &wp);
    }

    // Get monitor rect
    RECT monitorRect = {};
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);
    monitorRect = mi.rcMonitor;

    if (IsFullscreen())
    {
        bool succeeded = g_renderer->SetFullscreenState(m_renderTargetID, false);
        ASSERT_OR_DIE(succeeded, "Failed to set fullscreen.");
    }

    // Set style
    DWORD style = GetWindowStyleFlags();
    ::SetWindowLongPtr(hwnd, GWL_STYLE, style);

    RECT targetRect = monitorRect;
    if (IsWindowed())
    {
        IntVec2 lastKnownPos = GetLastKnownWindowedPos();
        IntVec2 desiredResolution = GetDesiredWindowResolution();
        targetRect.left = lastKnownPos.x;
        targetRect.top = lastKnownPos.y;
        targetRect.right = targetRect.left + desiredResolution.x;
        targetRect.bottom = targetRect.top + desiredResolution.y;
        bool adjusted = ::AdjustWindowRect(&targetRect, style, FALSE);

        ASSERT_OR_DIE(adjusted, "Failed to adjust");
    }

    std::this_thread::yield();
    bool setPos = ::SetWindowPos(hwnd, HWND_TOP,
                               targetRect.left, targetRect.top,
                               targetRect.right - targetRect.left,
                               targetRect.bottom - targetRect.top,
                               SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ASSERT_OR_DIE(setPos, "Failed to set window Pos");

    if (IsFullscreen())
    {
        bool succeeded = g_renderer->SetFullscreenState(m_renderTargetID, true);
        ASSERT_OR_DIE(succeeded, "Failed to set fullscreen.");
    }

    g_renderer->ResizeSwapChainRenderTarget(m_renderTargetID, GetRenderResolution());

    NamedProperties args;
    args.Set("previousMode", previousMode);
    args.Set("window", this);
    args.Set("mode", m_userSettings.m_windowMode);
    m_windowModeChanged.Broadcast(args);
}



//----------------------------------------------------------------------------------------------------------------------
void Win32Window::WindowFocusChanged()
{
    RefreshWindowTitle();

    if (IsFullscreen())
    {
        if (!HasFocus())
        {
            Minimize();
        }
        else
        {
            ::ShowWindow((HWND) GetHWND(), SW_SHOW);
        }
    }
    else
    {
        if (HasFocus())
        {
            ::SetForegroundWindow((HWND) GetHWND());
            ::SetActiveWindow((HWND) GetHWND());
        }
    }

    NamedProperties args;
    args.Set("hasFocus", HasFocus());
    args.Set("window", this);
    m_focusChanged.Broadcast(args);
}



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(Window* window, HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    ASSERT_OR_DIE(window != nullptr, "Could not find Window corresponding to the handle sent by windows message handling procedure.");

    if (wmMessageCode == WM_MOUSEMOVE)
    {
        return ::DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
    }

    switch (wmMessageCode)
    {
        case WM_ACTIVATE:
        {
            int code = (int) wParam;
            window->SetHasFocus(code != WA_INACTIVE);
            break;
        }
        case WM_CLOSE:
        {
            NamedProperties args;
            args.Set("window", window);
            window->m_quit.Broadcast(args);
            break;
        }
        case WM_MOVE:
        {
            window->HandleManualMove();
            break;
        }
        case WM_MOVING:         break;
        case WM_ENTERSIZEMOVE:
        {
            window->ToggleUserManualResizing(true);
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            window->ToggleUserManualResizing(false);
            break;
        }
        case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED)
            {
                window->SetIsMinimized(true);
            }
            else if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
            {
                window->SetIsMinimized(false);
            }
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            window->HandleWindowResolutionChanged(width, height);
            break;
        }
        case WM_CHAR:
        {
            NamedProperties args;
            int charCode = (int) wParam;
            args.Set("Char", charCode);
            window->m_charInputEvent.Broadcast(args);
            break;
        }
        case WM_KEYDOWN:
        {
            NamedProperties args;
            int keyCode = (int) wParam;
            args.Set("Key", keyCode);
            window->m_keyDownEvent.Broadcast(args);
            break;
        }
        case WM_KEYUP:
        {
            NamedProperties args;
            int keyCode = (int) wParam;
            args.Set("Key", keyCode);
            window->m_keyUpEvent.Broadcast(args);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            bool bDown = (wParam & MK_LBUTTON) != 0;
            if (bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 0);
                window->m_mouseButtonDownEvent.Broadcast(args);
            }
            break;
        }
        case WM_LBUTTONUP:
        {
            bool bDown = (wParam & MK_LBUTTON) != 0;
            if (!bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 0);
                window->m_mouseButtonUpEvent.Broadcast(args);
            }
            break;
        }
        case WM_RBUTTONDOWN:
        {
            bool bDown = (wParam & MK_RBUTTON) != 0;
            if (bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 1);
                window->m_mouseButtonDownEvent.Broadcast(args);
            }
            break;
        }
        case WM_RBUTTONUP:
        {
            bool bDown = (wParam & MK_RBUTTON) != 0;
            if (!bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 1);
                window->m_mouseButtonUpEvent.Broadcast(args);
            }
            break;
        }
        case WM_MBUTTONDOWN:
        {
            bool bDown = (wParam & MK_MBUTTON) != 0;
            if (bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 2);
                window->m_mouseButtonDownEvent.Broadcast(args);
            }
            break;
        }
        case WM_MBUTTONUP:
        {
            bool bDown = (wParam & MK_MBUTTON) != 0;
            if (!bDown)
            {
                NamedProperties args;
                args.Set("MouseButton", 2);
                window->m_mouseButtonUpEvent.Broadcast(args);
            }
            break;
        }
        case WM_MOUSEWHEEL:
        {
            NamedProperties args;
            int wheelChange = ((int) wParam >> 16) / 120;
            args.Set("WheelChange", wheelChange);
            window->m_mouseWheelEvent.Broadcast(args);
            break;
        }
        default:
            break;
    }

    return ::DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK SharedWindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;
    window = reinterpret_cast<Window*>(::GetWindowLongPtr(windowHandle, GWLP_USERDATA));

    if (wmMessageCode == WM_NCCREATE)
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
        ::SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR) window);
    }
    else if (wmMessageCode == WM_NCDESTROY)
    {
        ::SetWindowLongPtr(windowHandle, GWLP_USERDATA, 0);
        window = nullptr;
    }

    if (window)
    {
        return WindowsMessageHandlingProcedure(window, windowHandle, wmMessageCode, wParam, lParam);
    }

    return ::DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

#endif // _WIN32