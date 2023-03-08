// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Math/Grid.h"



class Texture;
class VertexBuffer;



class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void Shutdown() override;

    void UpdateTextVerts();
    void UpdateSelectedCell(float deltaSeconds);
    void UpdateNumberEntry();

    Camera m_camera;
    Grid<int> m_grid;
    int m_selectedCellIndex = 0;
    VertexBuffer* m_staticGridVerts;
	VertexBuffer* m_textVerts;
	VertexBuffer* m_selectedCellVerts;
};
