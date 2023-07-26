// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Render() const override;
    void Shutdown() override;

private:

    void RegisterDevConsoleCommands() const;
    void UnRegisterDevConsoleCommands() const;

    static bool OnSystemActiveCommand(NamedProperties& eventArgs);
};
