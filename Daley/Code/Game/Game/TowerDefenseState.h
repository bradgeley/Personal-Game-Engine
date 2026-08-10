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

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

protected:

	void StartGame();
	void ShutdownGame();
	void ConfigureECS();

	static bool Win(NamedProperties& props);
	static bool Lose(NamedProperties& props);

private:

	bool m_isGameOver = false;
};