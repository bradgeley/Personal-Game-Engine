// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11GPUBuffer.h"
#include "D3D11VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11VertexBuffer::InitializeInternal(size_t vertSize, size_t initialVertCount /*= 0*/)
{
	ReleaseResources();

	m_vertSize = vertSize;

	GpuBufferConfig config;
	config.m_bufferType = BufferType::VertexBuffer;
	m_gpuBuffer = new D3D11GPUBuffer(config);

	if (vertSize > 0 && initialVertCount > 0)
	{
		// Only create the gpu buffer if we have data to put in it
		// If not, that's ok and we can create it later after data is added.
		m_gpuBuffer->Initialize(m_vertSize * initialVertCount);
	}
}


#endif // RENDERER_D3D11