// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/DataStructures/NamedProperties.h"
#include <vector>



class VisualTest;



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

    bool Help(NamedProperties& args);

    int m_currentTestMode = 1;
    std::vector<VisualTest*> m_testModes;
};
