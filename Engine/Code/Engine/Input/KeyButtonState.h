// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct KeyButtonState
{
	KeyButtonState() = default;
	~KeyButtonState() = default;

	bool IsPressed() const;
	bool WasJustPressed() const;
	bool WasJustReleased() const;
	void Reset();

	void Press();
	void Release();
	void OnNextFrame();

protected:

	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;
};