// Bradley Christensen - 2022-2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct KeyButtonState
{
	friend class InputSystem;

	KeyButtonState() = default;
	~KeyButtonState() = default;

	bool IsPressed() const;
	bool WasJustPressed() const;
	bool WasJustReleased() const;
	void Reset();

protected:

	void Press();
	void Release();
	void OnNextFrame();

	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;
};