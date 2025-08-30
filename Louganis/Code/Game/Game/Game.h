// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Clock;
class Engine;
class Window;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class Game : public EngineSubsystem
{
public:

    Game();
    ~Game();

    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Render() const override;
    void Shutdown() override;

    void ConfigureEngine(Engine* engine);
    void ConfigureECS();

protected:

    bool TimeDilation(NamedProperties& args);

protected:

    Clock* m_gameClock = nullptr;
    Clock* m_gameClock2 = nullptr;
    Clock* m_gameClock3 = nullptr;
    Clock* m_gameClock4 = nullptr;
};
