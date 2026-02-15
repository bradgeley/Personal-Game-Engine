// Bradley Christensen - 2022-2025
#pragma once
#include "InputUtils.h"
#include "KeyButtonState.h"
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



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
	virtual ~InputSystem() override;
    
    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Shutdown() override;

    bool IsValidKey(int key) const;
    bool IsKeyDown(int keyCode) const;
    bool WasKeyJustPressed(int keyCode) const;
    bool WasKeyJustReleased(int keyCode) const;
    bool IsKeyDown(KeyCode keyCode) const;
    bool WasKeyJustPressed(KeyCode keyCode) const;
    bool WasKeyJustReleased(KeyCode keyCode) const;

    bool IsValidMouseButton(int mouseButton) const;
    bool IsMouseButtonDown(int mouseButton) const;
    bool WasMouseButtonJustPressed(int mouseButton);
    bool WasMouseButtonJustReleased(int mouseButton);
    int GetMouseWheelChange();
    
    IntVec2	GetMouseClientPosition() const;
    Vec2 GetMouseClientRelativePosition() const;
    Vec2 GetMouseClientCenterRelativePosition() const;
	Vec2 GetMouseViewportRelativePosition(float letterboxedViewportAspect) const;

private:

    // Event Handlers
    bool HandleChar(NamedProperties& args);
    bool HandleKeyDown(NamedProperties& args);
    bool HandleKeyUp(NamedProperties& args);
    bool HandleMouseButtonDown(NamedProperties& args);
    bool HandleMouseButtonUp(NamedProperties& args);
    bool HandleMouseWheel(NamedProperties& args);

    void CacheMouseClientPosition();
    void CacheMouseClientRelativePosition();

public:

    // Input events
    EventDelegate m_mouseButtonDownEvent;
    EventDelegate m_mouseButtonUpEvent;
    EventDelegate m_charInputEvent;
    EventDelegate m_keyDownEvent;
    EventDelegate m_keyUpEvent;
    EventDelegate m_mouseWheelEvent;
    EventDelegate m_mouseWheelUpEvent;
    EventDelegate m_mouseWheelDownEvent;

private:

    static constexpr int MAX_KEYS                   = 256;
    static constexpr int MAX_MOUSE_BUTTONS          = 3;
    
    InputSystemConfig const m_config;

    // Keys
    KeyButtonState m_keyStates[MAX_KEYS];

    // Mouse
    KeyButtonState m_mouseButtonStates[MAX_MOUSE_BUTTONS];
    int m_frameMouseWheelChange                     = 0;
	float m_timeOfLastMouseClick                    = -1.f;
	IntVec2 m_cachedMouseClientPosition             = IntVec2::ZeroVector;
	Vec2 m_cachedMouseClientRelativePosition        = Vec2::ZeroVector;
};




