// Bradley Christensen - 2023
#pragma once
#include "CTransform.h"
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
    CTransform m_renderTransform;
    VertexBuffer m_vertexBuffer;
    ModelConstants m_modelConstants;
};
