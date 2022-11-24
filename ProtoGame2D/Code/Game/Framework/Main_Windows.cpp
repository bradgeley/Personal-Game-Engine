// Bradley Christensen - 2022
#include "WindowsApplication.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>



static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");



static WindowsApplication* g_application;
static HWND g_window;



LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   g_application = new WindowsApplication();
   g_application->Startup();
   g_application->Run();
   g_application->Shutdown();
   delete g_application;
   g_application = nullptr;
   
   return 0;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   HDC hdc;
   TCHAR greeting[] = _T("Hello, Windows desktop!");

   switch (message)
   {
   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);

      // Here your application is laid out.
      // For this introduction, we just print out "Hello, Windows desktop!"
      // in the top left corner.
      TextOut(hdc, 5, 5, greeting, (int) _tcslen(greeting));
      // End application specific layout section.

      EndPaint(hWnd, &ps);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
   }

   return 0;
}