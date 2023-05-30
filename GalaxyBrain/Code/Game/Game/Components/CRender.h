﻿// Bradley Christensen - 2023
#pragma once
#include "CTransform.h"
#include "Engine/ECS/Component.h"
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
struct CRender : Component
{
    Component* DeepCopy() const override;
    
    CTransform m_renderTransform;
    VertexBuffer m_vertexBuffer;
    ModelConstants m_modelConstants;
};
