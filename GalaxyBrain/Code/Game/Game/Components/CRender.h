// Bradley Christensen - 2023
#pragma once
#include "CTransform.h"
#include "Engine/Renderer/EngineConstantBuffers.h"



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
    CRender() = default;
    CRender(void const* xmlElement);
    
    CTransform m_renderTransform;
    ModelConstants m_modelConstants;
    int m_vboIndex = -1;
};
