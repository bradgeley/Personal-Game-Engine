// Bradley Christensen - 2022-2023
#pragma once
#include "KeyButtonState.hpp"
#include "Engine/Core/EngineSubsystem.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// THE Input System
//
extern class InputSystem* g_input;



//----------------------------------------------------------------------------------------------------------------------
struct InputSystemConfig
{
    
};



//----------------------------------------------------------------------------------------------------------------------
// Input System
//
class InputSystem : public EngineSubsystem
{
public:
    
    explicit InputSystem(InputSystemConfig const& config);
    
    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;

    bool IsValidKey(int key) const;
    bool IsKeyDown(int keyCode) const;
    bool WasKeyJustPressed(int keyCode) const;
    bool WasKeyJustReleased(int keyCode) const;

    bool IsValidMouseButton(int mouseButton) const;
    bool IsMouseButtonDown(int mouseButton) const;
    bool WasMouseButtonJustPressed(int mouseButton);
    bool WasMouseButtonJustReleased(int mouseButton);

private:

    // Event Handlers
    bool HandleChar(NamedProperties& args);
    bool HandleKeyDown(NamedProperties& args);
    bool HandleKeyUp(NamedProperties& args);
    bool HandleMouseButtonDown(NamedProperties& args);
    bool HandleMouseButtonUp(NamedProperties& args);
    bool HandleMouseWheel(NamedProperties& args);

private:

    static constexpr int MAX_KEYS = 256;
    static constexpr int MAX_MOUSE_BUTTONS = 3;
    
    InputSystemConfig const m_config;
    KeyButtonState m_keyStates[MAX_KEYS];
    KeyButtonState m_mouseButtonStates[MAX_MOUSE_BUTTONS];
    int m_frameMouseWheelChange = 0;
};




