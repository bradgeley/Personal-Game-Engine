// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Engine;
class Window;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class Game : public EngineSubsystem
{
public:

    Game();

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

    float m_currentTimeDilation = 1.f;
};
