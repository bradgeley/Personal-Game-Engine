// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Clock;
class Engine;
class GameFlow;
class Window;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class Game : public EngineSubsystem
{
public:

    Game();
    ~Game();

    void Startup() override;
    void Shutdown() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Render() const override;

    void ConfigureEngine(Engine* engine);

    bool Quit();

protected:

    bool QuitEvent(NamedProperties& args);

protected:

    GameFlow* m_gameFlow = nullptr;
};
