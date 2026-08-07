// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"



class Clock;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class GameState
{
public:

	virtual ~GameState() = default;

	virtual void Enter();
	virtual void Exit();
	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;

	bool IsPaused() const;
	void TogglePaused();
	void Pop();
	void RequestStateChange(Name toState);

	bool TimeDilation(NamedProperties& args);
	bool TogglePausedEvent(NamedProperties& args);

public:

	Name m_name = "Unnamed GameState";
	Clock* m_clock = nullptr;
};