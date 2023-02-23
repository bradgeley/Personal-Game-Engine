// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/Renderer/Camera.h"



class VertexBuffer;



class Game : public EngineSubsystem
{
public:

    void Startup() override;
    void Update(float deltaSeconds) override;
    void Render() const override;

    Camera m_camera = Camera(Vec3(-1.f,-1.f,-1.f), Vec3(1.f, 1.f, 1.f));
    VertexBuffer* m_vertexBuffer;
};
