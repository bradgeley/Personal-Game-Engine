// Bradley Christensen - 2022-2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)
#include "Engine/Renderer/VertexBuffer.h"



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// D3D11VertexBuffer
//
class D3D11VertexBuffer : public VertexBuffer
{
    friend class D3D11Renderer;
    
public:

    virtual ~D3D11VertexBuffer();

    virtual void Initialize(int numExpectedVerts = 3) override;
    virtual void UpdateGPUBuffer() override;
    virtual void ReleaseResources() override;

protected:
    
    ID3D11Buffer* m_handle = nullptr;
};

#endif // RENDERER_D3D11