// Bradley Christensen - 2022-2026
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11ConstantBuffer.h"
#include "D3D11GPUBuffer.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::Initialize(size_t byteWidth)
{
    ReleaseResources();

	ASSERT_OR_DIE(byteWidth > 0, "D3D11ConstantBuffer::Initialize - Invalid byte width");
	ASSERT_OR_DIE(byteWidth % 16 == 0, "D3D11ConstantBuffer::Initialize - requested size is not 16 byte aligned.");

	GpuBufferConfig config;
	config.m_bufferType = BufferType::ConstantBuffer;
	m_gpuBuffer = new D3D11GPUBuffer(config);
	m_gpuBuffer->Initialize(byteWidth);
}

#endif // RENDERER_D3D11