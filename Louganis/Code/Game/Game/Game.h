// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Engine;
class Window;



//----------------------------------------------------------------------------------------------------------------------
class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Render() const override;
    void Shutdown() override;

    void ConfigureEngine(Engine* engine);
    void ConfigureECS();

private:

    Window* m_debugWindow = nullptr;
};
