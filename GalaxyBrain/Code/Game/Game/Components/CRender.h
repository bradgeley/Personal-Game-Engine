// Bradley Christensen - 2023
#pragma once
#include "CTransform.h"
#include "Engine/Renderer/EngineConstantBuffers.h"



class Texture;



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
    CRender() = default;
    CRender(void const* xmlElement);
    
    int m_layer = 0;
    CTransform m_renderTransform;
    float m_scale = 1.f;
    ModelConstants m_modelConstants;
    Texture* m_texture = nullptr;
    int m_vboIndex = -1;
};
