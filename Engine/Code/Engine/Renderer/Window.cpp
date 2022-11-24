// Bradley Christensen - 2022
#include "Window.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Window (for now)
//
Window* g_window = nullptr;



Window::Window(WindowConfig const& config) : m_config(config)
{
}



void Window::Startup()
{
    CreateMainWindow();
}



void Window::BeginFrame()
{
    
}

void Window::Update(float deltaSeconds)
{
    
}

void Window::Render()
{
    
}

void Window::EndFrame()
{
    
}

void Window::Shutdown()
{
    
}

void Window::CreateMainWindow()
{
    WNDCLASSEX wcex;

   wcex.cbSize         = sizeof(WNDCLASSEX);
   wcex.style          = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc    = WndProc;
   wcex.cbClsExtra     = 0;
   wcex.cbWndExtra     = 0;
   wcex.hInstance      = hInstance;
   wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
   wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName   = NULL;
   wcex.lpszClassName  = szWindowClass;
   wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

   if (!RegisterClassEx(&wcex))
   {
      MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
      return 1;
   }

   // The parameters to CreateWindowEx explained:
   // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
   // szWindowClass: the name of the application
   // szTitle: the text that appears in the title bar
   // WS_OVERLAPPEDWINDOW: the type of window to create
   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
   // 500, 100: initial size (width, length)
   // NULL: the parent of this window
   // NULL: this application dows not have a menu bar
   // hInstance: the first parameter from WinMain
   // NULL: not used in this application
   g_window = CreateWindowEx(
      WS_EX_OVERLAPPEDWINDOW,
      szWindowClass,
      szTitle,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      500, 100,
      NULL,
      NULL,
      hInstance,
      NULL
   );

   if (!g_window)
   {
      MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
      return 1;
   }

   // The parameters to ShowWindow explained:
   // hWnd: the value returned from CreateWindow
   // nCmdShow: the fourth parameter from WinMain
   ShowWindow(g_window, nCmdShow);
   UpdateWindow(g_window);

   // Main message loop:
   MSG msg;
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   return (int) msg.wParam;
}



void Window::RunMessagePump()
{
   MSG queuedMessage;
   for ( ;; )
   {
      const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
      if ( !wasMessagePresent )
      {
         break;
      }

      TranslateMessage( &queuedMessage );
      DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
   }
}
