// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "Engine/Renderer/InstanceBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
// D3D11InstanceBuffer
//
class D3D11InstanceBuffer : public InstanceBuffer
{
    friend class D3D11Renderer;
    
public:

    virtual void InitializeInternal(size_t instanceSize, size_t initialInstanceCount) override;
};

#endif // RENDERER_D3D11