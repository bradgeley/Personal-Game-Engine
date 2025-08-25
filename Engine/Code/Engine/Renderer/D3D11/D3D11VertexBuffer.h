﻿// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/VertexBuffer.h"



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// D3D11VertexBuffer
//
class D3D11VertexBuffer : public VertexBuffer
{
    friend class D3D11Renderer;
    
public:

    virtual void Initialize(int numExpectedVerts = 3) override;
    virtual void UpdateGPUBuffer() override;
    virtual void ReleaseResources() override;

protected:
    
    ID3D11Buffer* m_handle = nullptr;
};
