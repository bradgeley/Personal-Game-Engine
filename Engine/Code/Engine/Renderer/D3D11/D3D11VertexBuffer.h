// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
// D3D11VertexBuffer
//
class D3D11VertexBuffer : public VertexBuffer
{
    friend class D3D11Renderer;
    
public:

    virtual void InitializeInternal(size_t vertSize, size_t initialVertCount) override;
};

#endif // RENDERER_D3D11