// Bradley Christensen - 2022-2023
#include "InputSystem.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Input System
//
InputSystem* g_input = nullptr;



//----------------------------------------------------------------------------------------------------------------------
InputSystem::InputSystem(InputSystemConfig const& config) : m_config(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::Startup()
{
    g_window->m_charInputEvent.SubscribeMethod(*this, &InputSystem::HandleChar);
    g_window->m_keyDownEvent.SubscribeMethod(*this, &InputSystem::HandleKeyDown);
    g_window->m_keyUpEvent.SubscribeMethod(*this, &InputSystem::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.SubscribeMethod(*this, &InputSystem::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.SubscribeMethod(*this, &InputSystem::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.SubscribeMethod(*this, &InputSystem::HandleMouseWheel);
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
    for (auto& key : m_keyStates)
    {
        key.OnNextFrame();
    }
    for (auto& mouseBtn : m_mouseButtonStates)
    {
        mouseBtn.OnNextFrame();
    }
    m_frameMouseWheelChange = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::Shutdown()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown(int keyCode) const
{
    if (keyCode >= 0 && keyCode < MAX_KEYS)
    {
        return m_keyStates->IsPressed();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed(int keyCode) const
{
    if (keyCode >= 0 && keyCode < MAX_KEYS)
    {
        return m_keyStates->WasJustPressed();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased(int keyCode) const
{
    if (keyCode >= 0 && keyCode < MAX_KEYS)
    {
        return m_keyStates->WasJustReleased();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsValidKey(int key) const
{
    return (key >= 0 && key < MAX_KEYS);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsValidMouseButton(int btn) const
{
    return (btn >= 0 && btn < MAX_MOUSE_BUTTONS);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleChar(NamedProperties& args)
{
    int character = args.Get("Char", -1);
    if (IsValidKey(character))
    {
        m_keyStates[character].Press();
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyDown(NamedProperties& args)
{
    int key = args.Get("Key", -1);
    if (IsValidKey(key))
    {
        m_keyStates[key].Press();
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyUp(NamedProperties& args)
{
    int key = args.Get("Key", -1);
    if (IsValidKey(key))
    {
        m_keyStates[key].Release();
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonDown(NamedProperties& args)
{
    int mouseButton = args.Get("MouseButton", -1);
    if (IsValidMouseButton(mouseButton))
    {
        m_mouseButtonStates[mouseButton].Press();    
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonUp(NamedProperties& args)
{
    int mouseButton = args.Get("MouseButton", -1);
    if (IsValidMouseButton(mouseButton))
    {
        m_mouseButtonStates[mouseButton].Release();    
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseWheel(NamedProperties& args)
{
	m_frameMouseWheelChange = args.Get("WheelChange", 0);
    return true;
}
