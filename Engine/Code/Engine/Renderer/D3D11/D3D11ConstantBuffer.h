// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
// D3D11ConstantBuffer
//
class D3D11ConstantBuffer : public ConstantBuffer
{
    friend class D3D11Renderer;

public:

    virtual void Initialize(size_t byteWidth) override;
};

#endif // RENDERER_D3D11