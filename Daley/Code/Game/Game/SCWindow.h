// Bradley Christensen - 2022-2025
#pragma once



class Window;



//----------------------------------------------------------------------------------------------------------------------
class SCWindow
{
public:

	Window const* GetWindow() const		{ return m_window; }
	Window* GetWindow()					{ return m_window; }
	void SetWindow(Window* window)		{ m_window = window; }

private:

	Window* m_window = nullptr;
};