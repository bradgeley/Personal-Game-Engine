// Bradley Christensen - 2022-2026
#pragma once
#include "GameState.h"



class Clock;
struct NamedProperties; 



//----------------------------------------------------------------------------------------------------------------------
class TowerDefenseState : public GameState
{
public:

	TowerDefenseState();

	virtual void Enter(NamedProperties const& props) override;
	virtual void Exit(NamedProperties const& props) override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

protected:

	void StartGame(NamedProperties const& inProps);
	void ShutdownGame();
	void ConfigureECS();

	static bool Win(NamedProperties& props);
	static bool Lose(NamedProperties& props);
	static bool Restart(NamedProperties& props);

private:

	bool m_isGameOver = false;
};