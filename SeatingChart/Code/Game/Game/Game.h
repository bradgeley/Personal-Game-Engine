// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include <vector>



class Guest;
class GuestList;
class Camera;
class SeatingChart;
class SeatingChartGenerator;



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

    Camera* m_camera;
    GuestList* m_guestList;
    SeatingChart* m_seatingChart;
    SeatingChartGenerator* m_generator;
};
