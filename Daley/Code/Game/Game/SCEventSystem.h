// Bradley Christensen - 2022-2025
#pragma once



class EventSystem;



//----------------------------------------------------------------------------------------------------------------------
class SCEventSystem
{
public:

	EventSystem const* GetEventSystem() const		{ return m_eventSystem; }
	EventSystem* GetEventSystem()					{ return m_eventSystem; }
	void SetEventSystem(EventSystem* eventSystem)	{ m_eventSystem = eventSystem; }

private:

	EventSystem* m_eventSystem = nullptr;
};