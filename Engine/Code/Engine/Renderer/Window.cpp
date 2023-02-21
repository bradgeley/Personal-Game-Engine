// Bradley Christensen - 2022
#include "Window.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Window (for now)
//
Window* g_window = nullptr;



// Forward Declaration
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);



Window::Window(WindowConfig const& config) : m_config(config)
{
}



void Window::Startup()
{
    CreateMainWindow();
}



void Window::BeginFrame()
{
    RunMessagePump();
}



void Window::CreateMainWindow()
{
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_OWNDC;
    wcex.lpfnWndProc    = WindowsMessageHandlingProcedure; // Register our Windows message-handling function
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = GetModuleHandle(NULL);
    wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = TEXT("Simple Window Class");
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
    const DWORD windowStyleExFlags = WS_EX_APPWINDOW;
 
    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);
    float desktopWidth = (float) (desktopRect.right - desktopRect.left);
    float desktopHeight = (float) (desktopRect.bottom - desktopRect.top);
    float desktopAspect = desktopWidth / desktopHeight;

    float clientHeight = desktopHeight;
    float clientWidth = desktopWidth;
    if (m_config.m_clientAspect > desktopAspect)
    {
        clientHeight = clientWidth / m_config.m_clientAspect;
    }
    else if (m_config.m_clientAspect < desktopAspect)
    {
        clientWidth = clientHeight * m_config.m_clientAspect;
    }
    m_dimensiions.x = static_cast<int>(clientWidth);
    m_dimensiions.y = static_cast<int>(clientHeight);

    // Calculate client rect bounds by centering the client area
    float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
    float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
    RECT clientRect;
    clientRect.left = (int) clientMarginX;
    clientRect.right = clientRect.left + (int) clientWidth;
    clientRect.top = (int) clientMarginY;
    clientRect.bottom = clientRect.top + (int) clientHeight;
    
    // Calculate the outer dimensions of the physical window, including frame et. al.
    RECT windowRect = clientRect;
    AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

    // Convert window title to a wstring
    std::string titleString = m_config.m_windowTitle;
    std::wstring titleWString = std::wstring(titleString.begin(), titleString.end());
    
    HWND hwnd = CreateWindowEx(
        windowStyleExFlags,
        wcex.lpszClassName,
        titleWString.data(),
        windowStyleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        wcex.hInstance,
        NULL);

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    
    m_windowHandle = hwnd;
    m_displayContext = GetDC(hwnd);
}



void Window::RunMessagePump()
{
   MSG queuedMessage;
   for (;;)
   {
      const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
      if (!wasMessagePresent)
      {
         break;
      }

      TranslateMessage(&queuedMessage);
      DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
   }
}



LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    switch (wmMessageCode)
    {
    case WM_ACTIVATE:       break;
    case WM_CLOSE:
        {
            g_window->m_isQuitting = true;
            break;
        }
    case WM_MOVE:           break;
    case WM_MOVING:         break;
    case WM_SIZE:           break;
    case WM_CHAR:           break;
    case WM_KEYDOWN:        break;
    case WM_KEYUP:          break;
    case WM_LBUTTONDOWN:    break; // Left Mouse Button
    case WM_LBUTTONUP:      break; // Left Mouse Button
    case WM_RBUTTONDOWN:    break; // Right Mouse Button
    case WM_RBUTTONUP:      break; // Right Mouse Button
    case WM_MBUTTONDOWN:    break; // Other Mouse Buttons
    case WM_MBUTTONUP:      break; // Other Mouse Buttons
    case WM_MOUSEWHEEL:     break; // Mouse Wheel
    }
    return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}