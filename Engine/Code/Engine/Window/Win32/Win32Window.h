// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// Win32Window
//
// A Windows window, used for rendering stuff.
//
class Win32Window : public Window
{
public:

    Win32Window(WindowConfig const& config);
    
    virtual void BeginFrame() override;
    virtual void Shutdown() override;

    bool IsValid() const;
    void SetWindowTitle(std::string const& newTitle);

    void* GetHWND() const;

    WindowMode GetCurrentWindowMode() const;
    void Minimize();

    IntVec2 GetDesiredWindowResolution() const;

    IntVec2 GetMouseClientPosition(bool originBottomLeft = true) const;
    Vec2 GetMouseClientRelativePosition(bool originBottomLeft = true) const;

    bool GiveFocus();

    void RefreshWindowSize();
    void HandleManualMove();

    // Win32 Specific Functions
    uint32_t GetWindowStyleFlags() const;
    uint32_t GetWindowStyleExFlags() const;
    void MakeChildOf(Window* parentWindow);

private:

    void MakeWindow();
    void RunMessagePump();

    void WindowModeChanged(WindowMode previousMode);
    void WindowFocusChanged();

protected:

    void* m_windowHandle            = nullptr; // HWND
    void* m_displayContext          = nullptr; // HDC
};
