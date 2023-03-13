// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Texture;
class SudokuGrid;
class SudokuPlayer;



class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void Shutdown() override;

    SudokuGrid* m_grid;
    SudokuPlayer* m_player;
};
