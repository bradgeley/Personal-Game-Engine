﻿// Bradley Christensen - 2022-2023
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "Window.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Texture.h"




//----------------------------------------------------------------------------------------------------------------------
// The main game window
//
Window* g_window = nullptr;



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(Window* window, HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SharedWindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);



//----------------------------------------------------------------------------------------------------------------------
Window::Window(WindowConfig const& config) : EngineSubsystem("Window"), m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Window::Startup()
{
    CreateMainWindow();
    GiveFocus();
}



//----------------------------------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
    RunMessagePump();
}



//----------------------------------------------------------------------------------------------------------------------
void Window::Shutdown()
{
    if (IsValid())
    {
        SetWindowLongPtr((HWND) m_windowHandle, GWLP_USERDATA, 0);
        DestroyWindow((HWND) m_windowHandle);
        m_windowHandle = nullptr;
        m_displayContext = nullptr;
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::IsValid() const
{
    if (m_windowHandle != nullptr && m_displayContext != nullptr)
    {
        return IsWindow((HWND) m_windowHandle);
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::IsBeingCreated() const
{
    return m_isBeingCreated;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::SetIsBeingCreated(bool isBeingCreated)
{
    m_isBeingCreated = isBeingCreated;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::SetWindowTitle(std::string const& newTitle)
{
    std::wstring wNewTitle = std::wstring(newTitle.begin(), newTitle.end());
    SetWindowText((HWND) m_windowHandle, wNewTitle.c_str());
}



//----------------------------------------------------------------------------------------------------------------------
void* Window::GetHWND() const
{
    return m_windowHandle;
}



//----------------------------------------------------------------------------------------------------------------------
int Window::GetWidth() const
{
    return m_dimensions.x;
}



//----------------------------------------------------------------------------------------------------------------------
int Window::GetHeight() const
{
    return m_dimensions.y;
}



//----------------------------------------------------------------------------------------------------------------------
float Window::GetAspect() const
{
    return (float) GetWidth() / (float) GetHeight();
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 const& Window::GetDimensions() const
{
    return m_dimensions;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 Window::GetMouseClientPosition(bool originBottomLeft) const
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
Vec2 Window::GetMouseClientRelativePosition(bool originBottomLeft) const
{
    Vec2 relativePosition = Vec2(GetMouseClientPosition(originBottomLeft));
    RECT clientRect;
    ::GetClientRect((HWND) m_windowHandle, &clientRect);
    relativePosition.y /= static_cast<float>(clientRect.bottom);
    relativePosition.x /= static_cast<float>(clientRect.right);
    return relativePosition;
}



//----------------------------------------------------------------------------------------------------------------------
bool Window::GiveFocus()
{
    if (m_windowHandle)
    {
        ::SetFocus((HWND) m_windowHandle);
        ::SetForegroundWindow((HWND) m_windowHandle);
        SetHasFocus(true);
    }
    return m_windowHandle != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void Window::SetHasFocus(bool hasFocus)
{
    m_hasFocus = hasFocus;

    std::string const& baseTitle = m_config.m_windowTitle;
    if (m_hasFocus)
    {
        std::string focusedTitle = baseTitle + " (focused)";
        SetWindowTitle(focusedTitle);
    }
    else
    {
        SetWindowTitle(baseTitle);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Window::CreateMainWindow()
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
    RegisterClassEx(&wcex);
    
    DWORD const windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
    DWORD const windowStyleExFlags = WS_EX_APPWINDOW;

    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);
    float desktopWidth = (float) (desktopRect.right - desktopRect.left);
    float desktopHeight = (float) (desktopRect.bottom - desktopRect.top);
    float desktopAspect = desktopWidth / desktopHeight;

    float clientHeight = desktopHeight * m_config.m_windowScale;
    float clientWidth = desktopWidth * m_config.m_windowScale;
    if (m_config.m_clientAspect > desktopAspect)
    {
        clientHeight = clientWidth / m_config.m_clientAspect;
    }
    else if (m_config.m_clientAspect < desktopAspect)
    {
        clientWidth = clientHeight * m_config.m_clientAspect;
    }
    m_dimensions.x = static_cast<int>(clientWidth);
    m_dimensions.y = static_cast<int>(clientHeight);

    // Calculate client rect bounds by centering the client area
    float clientMarginX = 0.5f * (desktopWidth - clientWidth);
    float clientMarginY = 0.5f * (desktopHeight - clientHeight);
    RECT clientRect;
    clientRect.left = (int) clientMarginX;
    clientRect.right = clientRect.left + (int) clientWidth;
    clientRect.top = (int) clientMarginY;
    clientRect.bottom = clientRect.top + (int) clientHeight;
    
    // Calculate the outer dimensions of the physical window, including frame et. al.
    RECT windowRect = clientRect;
    AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

    // Convert window title to a wstring
    std::string titleString = m_config.m_windowTitle;
    std::wstring titleWString = std::wstring(titleString.begin(), titleString.end());

    SetIsBeingCreated(true);
    HWND hwnd = CreateWindowEx(
        windowStyleExFlags,
        wcex.lpszClassName,
        titleWString.data(),
        windowStyleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        wcex.hInstance,
        this);

    ASSERT_OR_DIE(hwnd != nullptr, "Failed to create window.");

    if (m_config.m_windowedBorderless)
    {
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(hwnd, HWND_TOP,
                     0, 0, desktopWidth, desktopHeight,
                     SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        m_dimensions.x = static_cast<int>(desktopWidth);
        m_dimensions.y = static_cast<int>(desktopHeight);
    }

    m_windowHandle = hwnd;
    m_displayContext = GetDC(hwnd);
    SetIsBeingCreated(false);

    ::ShowWindow(hwnd, SW_SHOW);
}



//----------------------------------------------------------------------------------------------------------------------
void Window::RunMessagePump()
{
   MSG queuedMessage;
   for (;;)
   {
      const BOOL wasMessagePresent = PeekMessage(&queuedMessage, nullptr, 0, 0, PM_REMOVE);
      if (!wasMessagePresent)
      {
         break;
      }

      TranslateMessage(&queuedMessage);
      DispatchMessage(&queuedMessage);
   }
}



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(Window* window, HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    ASSERT_OR_DIE(window != nullptr, "Could not find Window corresponding to the handle sent by windows message handling procedure.");
    UNUSED(lParam);
    UNUSED(windowHandle);

    NamedProperties args;
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
            window->m_quit.Broadcast(args);
            break;
        }
        case WM_MOVE:           break;
        case WM_MOVING:         break;
        case WM_SIZE:           break;
        case WM_CHAR:
        {
            int charCode = (int) wParam;
            args.Set("Char", charCode);
            window->m_charInputEvent.Broadcast(args);
            break;
        }
        case WM_KEYDOWN:
        {
            int keyCode = (int) wParam;
            args.Set("Key", keyCode);
            window->m_keyDownEvent.Broadcast(args);
            break;
        }
        case WM_KEYUP:
        {
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
                args.Set("MouseButton", 2);
                window->m_mouseButtonUpEvent.Broadcast(args);
            }
            break;
        }
        case WM_MOUSEWHEEL:
        {
            int wheelChange = ((int) wParam >> 16) / 120;
            args.Set("WheelChange", wheelChange);
            window->m_mouseWheelEvent.Broadcast(args);
            break;
        }
        default:
            break;
    }

    return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}



//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK SharedWindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;
    window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

    if (wmMessageCode == WM_NCCREATE)
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR) window);
    }
    else if (wmMessageCode == WM_NCDESTROY)
    {
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, 0);
        window = nullptr;
    }

    if (window)
    {
        return WindowsMessageHandlingProcedure(window, windowHandle, wmMessageCode, wParam, lParam);
    }

    return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}