// Bradley Christensen - 2022-2023
#include "Engine/Renderer/ConstantBuffer.h"
#include "RendererInterface.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h"



//----------------------------------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
    ReleaseResources();
}



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::ReleaseResources()
{
    // Empty
}
