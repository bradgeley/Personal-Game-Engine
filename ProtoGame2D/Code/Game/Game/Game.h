// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"



class Texture;
class VertexBuffer;



class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void Render() const override;
    void Shutdown() override;

    Camera m_camera;
    VertexBuffer* m_staticGeometryVerts;
    VertexBuffer* m_spinningTextureVerts;
    Texture* m_texture = nullptr;
};
