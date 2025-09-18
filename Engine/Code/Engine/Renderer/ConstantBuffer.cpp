// Bradley Christensen - 2022-2025
#include "Engine/Renderer/ConstantBuffer.h"
#include "D3D11/D3D11GPUBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::ReleaseResources()
{
	if (m_gpuBuffer)
	{
		m_gpuBuffer->ReleaseResources();
		delete m_gpuBuffer;
		m_gpuBuffer = nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::Update(void const* data, size_t byteWidth, bool updateGPU /*= true*/)
{
	if (m_gpuBuffer == nullptr || byteWidth == 0)
	{
		return;
	}
	m_gpuBuffer->UpdateCPUBuffer(data, byteWidth);
	if (updateGPU)
	{
		m_gpuBuffer->UpdateGPUBuffer();
	}
}