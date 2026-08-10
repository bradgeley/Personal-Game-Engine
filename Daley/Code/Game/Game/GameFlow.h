// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <vector>



class GameState;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
enum class StateTransitionType : uint8_t
{
	None,
	Change,
	Push,
	Pop,
	Wipe,
};



//----------------------------------------------------------------------------------------------------------------------
struct StateTransition
{
	StateTransitionType m_type = StateTransitionType::None;
	Name m_toState = Name::Invalid;
};



//----------------------------------------------------------------------------------------------------------------------
class GameFlow
{
public:

	void Startup();
	void Shutdown();
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();

protected:

	void PushState(Name state);
	void PopState();
	void PopToState(Name state);
	void HandlePendingTransition();

public:

	bool PushStateEvent(NamedProperties& props);
	bool PopStateEvent(NamedProperties& props);
	bool ChangeStateEvent(NamedProperties& props);
	bool GameFlowDump(NamedProperties& props);

public:

	StateTransition m_pendingTransition;
	std::vector<GameState*> m_states;
};