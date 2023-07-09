// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include <string>



class Texture;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
    CRender() = default;
    CRender(void const* xmlElement);
    
    int m_layer = 0;
    float m_scale = 1.f;
    ModelConstants m_modelConstants;
    Texture* m_texture = nullptr;
    VertexBuffer* m_vbo = nullptr;
};
