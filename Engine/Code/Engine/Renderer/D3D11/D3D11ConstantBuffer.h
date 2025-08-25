﻿// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/ConstantBuffer.h"



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// D3D11 Renderer
//
class D3D11ConstantBuffer : public ConstantBuffer
{
    friend class D3D11Renderer;

public:

    virtual void ReleaseResources() override;

    virtual void Update(void const* data, size_t byteWidth) override;

protected:

    virtual void Initialize(size_t byteWidth) override;

public:

	ID3D11Buffer* m_handle = nullptr;
};

