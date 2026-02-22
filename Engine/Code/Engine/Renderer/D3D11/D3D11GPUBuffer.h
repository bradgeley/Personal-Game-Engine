// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "Engine/Renderer/GPUBuffer.h"



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// D3D11 GPU Buffer
//
class D3D11GPUBuffer : public GPUBuffer
{
	friend class D3D11Renderer;

public:

    D3D11GPUBuffer(GpuBufferConfig const& config);

    virtual void Initialize(size_t byteWidth) override;
     
    virtual void ReleaseResources() override;
    virtual void UpdateGPUBuffer() override;

protected:
    
    ID3D11Buffer* m_handle = nullptr;
};

#endif // RENDERER_D3D11