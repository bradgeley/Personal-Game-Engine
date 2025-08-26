// Bradley Christensen - 2022-2025
#include "Engine/Input/KeyButtonState.h"



//----------------------------------------------------------------------------------------------------------------------
bool KeyButtonState::IsPressed() const
{
	return m_isPressed;
}



//----------------------------------------------------------------------------------------------------------------------
bool KeyButtonState::WasJustPressed() const
{
	return (m_isPressed && !m_wasPressedLastFrame);
}



//----------------------------------------------------------------------------------------------------------------------
bool KeyButtonState::WasJustReleased() const
{
	return (!m_isPressed && m_wasPressedLastFrame);
}



//----------------------------------------------------------------------------------------------------------------------
void KeyButtonState::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}



//----------------------------------------------------------------------------------------------------------------------
void KeyButtonState::Press()
{
	m_isPressed = true;
}



//----------------------------------------------------------------------------------------------------------------------
void KeyButtonState::Release()
{
	m_isPressed = false;
}



//----------------------------------------------------------------------------------------------------------------------
void KeyButtonState::OnNextFrame()
{
	m_wasPressedLastFrame = m_isPressed;
}
