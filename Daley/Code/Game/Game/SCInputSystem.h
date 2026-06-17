// Bradley Christensen - 2022-2025
#pragma once



class InputSystem;



//----------------------------------------------------------------------------------------------------------------------
class SCInputSystem
{
public:

	InputSystem const* GetInputSystem() const		{ return m_inputSystem; }
	InputSystem* GetInputSystem()					{ return m_inputSystem; }
	void SetInputSystem(InputSystem* inputSystem)	{ m_inputSystem = inputSystem; }

private:

	InputSystem* m_inputSystem = nullptr;
};