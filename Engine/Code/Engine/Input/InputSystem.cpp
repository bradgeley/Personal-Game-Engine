﻿// Bradley Christensen - 2022-2023
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
    g_window->m_charInputEvent.SubscribeMethod(this, &InputSystem::HandleChar);
    g_window->m_keyDownEvent.SubscribeMethod(this, &InputSystem::HandleKeyDown);
    g_window->m_keyUpEvent.SubscribeMethod(this, &InputSystem::HandleKeyUp);
    g_window->m_mouseButtonDownEvent.SubscribeMethod(this, &InputSystem::HandleMouseButtonDown);
    g_window->m_mouseButtonUpEvent.SubscribeMethod(this, &InputSystem::HandleMouseButtonUp);
    g_window->m_mouseWheelEvent.SubscribeMethod(this, &InputSystem::HandleMouseWheel);
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)
}



//----------------------------------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
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
void InputSystem::Shutdown()
{
    
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsValidKey(int key) const
{
    return (key >= 0 && key < MAX_KEYS);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown(int keyCode) const
{
    if (IsValidKey(keyCode))
    {
        return m_keyStates[keyCode].IsPressed();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed(int keyCode) const
{
    if (IsValidKey(keyCode))
    {
        return m_keyStates[keyCode].WasJustPressed();
    } 
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased(int keyCode) const
{
    if (IsValidKey(keyCode))
    {
        return m_keyStates[keyCode].WasJustReleased();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown(KeyCode keyCode) const
{
    return IsKeyDown((int)keyCode);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed(KeyCode keyCode) const
{
    return WasKeyJustPressed((int)keyCode);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased(KeyCode keyCode) const
{
    return WasKeyJustReleased((int)keyCode);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsValidMouseButton(int mouseButton) const
{
    return (mouseButton >= 0 && mouseButton < MAX_MOUSE_BUTTONS);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::IsMouseButtonDown(int mouseButton) const
{
    if (IsValidMouseButton(mouseButton))
    {
        return m_mouseButtonStates[mouseButton].IsPressed();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasMouseButtonJustPressed(int mouseButton)
{
    if (IsValidMouseButton(mouseButton))
    {
        return m_mouseButtonStates[mouseButton].WasJustPressed();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::WasMouseButtonJustReleased(int mouseButton)
{
    if (IsValidMouseButton(mouseButton))
    {
        return m_mouseButtonStates[mouseButton].WasJustReleased();
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 InputSystem::GetMouseClientPosition(bool originBottomLeft) const
{
    return g_window->GetMouseClientPosition(originBottomLeft);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 InputSystem::GetMouseClientRelativePosition(bool originBottomLeft) const
{
    return g_window->GetMouseClientRelativePosition(originBottomLeft);
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleChar(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
    }
    
    int character = args.Get("Char", -1);
    if (IsValidKey(character))
    {
        m_keyStates[character].Press();
        m_charInputEvent.Broadcast(args);
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyDown(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
        
    }
    int key = args.Get("Key", -1);
    if (IsValidKey(key))
    {
        m_keyStates[key].Press();
        m_keyDownEvent.Broadcast(args);
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyUp(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
    }
    
    int key = args.Get("Key", -1);
    if (IsValidKey(key))
    {
        m_keyStates[key].Release();
        m_keyUpEvent.Broadcast(args);
        return true; 
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonDown(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
    }
    
    int mouseButton = args.Get("MouseButton", -1);
    if (IsValidMouseButton(mouseButton))
    {
        m_mouseButtonStates[mouseButton].Press();    
        m_mouseButtonDownEvent.Broadcast(args);
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonUp(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
    }
    
    int mouseButton = args.Get("MouseButton", -1);
    if (IsValidMouseButton(mouseButton))
    {
        m_mouseButtonStates[mouseButton].Release();
        m_mouseButtonUpEvent.Broadcast(args);
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseWheel(NamedProperties& args)
{
    if (!m_isEnabled)
    {
        return false;
        
    }
	m_frameMouseWheelChange = args.Get("WheelChange", 0);
    m_mouseWheelEvent.Broadcast(args);
    return true;
}
