// Bradley Christensen - 2022-2026
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11GPUBuffer.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
D3D11GPUBuffer::D3D11GPUBuffer(GpuBufferConfig const& config) : GPUBuffer(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11GPUBuffer::ReleaseResources()
{
	DX_SAFE_RELEASE(m_handle)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11GPUBuffer::UpdateGPUBuffer()
{
	if (!m_isDirty)
	{
		return;
	}

	if (!m_handle || GetCPUBufferSize() > m_gpuBufferSize)
	{
		// If we don't have a buffer yet, or if the size has increased, reinitialize it
		Initialize(m_cpuBuffer.size());
	}

	ID3D11DeviceContext* context = D3D11Renderer::Get()->GetDeviceContext();
	ASSERT_OR_DIE(context, "Invalid D3D11 device context");
	ASSERT_OR_DIE(m_handle, "Invalid D3D11 buffer handle");

	if (m_isDirty && !m_cpuBuffer.empty())
	{
		// GPU
		D3D11_MAPPED_SUBRESOURCE mapping;
		HRESULT result = context->Map(
			m_handle,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapping
		);
		ASSERT_OR_DIE(SUCCEEDED(result), "Failed to map data for GPU buffer");
		memcpy(mapping.pData, m_cpuBuffer.data(), m_cpuBuffer.size());
		context->Unmap(m_handle, 0);
		m_isDirty = false;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11GPUBuffer::Initialize(size_t byteWidth)
{
	ReleaseResources();

	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
	ASSERT_OR_DIE(device, "Invalid D3D11 device");

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT) byteWidth;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	switch (m_config.m_bufferType)
	{
		case BufferType::VertexBuffer:		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		break;
		case BufferType::IndexBuffer:		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;		break;
		case BufferType::InstanceBuffer:	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		break;
		case BufferType::ConstantBuffer:	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	break;
			default: ASSERT_OR_DIE(false, "Unsupported buffer type for D3D11GPUBuffer");	break;
	}
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT result;
	if (m_cpuBuffer.empty())
	{
		result = device->CreateBuffer(&desc, nullptr, &m_handle);
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = m_cpuBuffer.data();
		initialData.SysMemPitch = 0;
		ASSERT_OR_DIE(byteWidth <= m_cpuBuffer.size(), "CPU Buffer doesnt have enough data to fit the initial size.");
		result = device->CreateBuffer(&desc, &initialData, &m_handle);
	}

	ASSERT_OR_DIE(SUCCEEDED(result), StringUtils::StringF("Failed to create gpu buffer"));

	m_gpuBufferSize = byteWidth;

	#if defined(_DEBUG)
	m_handle->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) strlen("D3D11GpuBuffer"), "D3D11GpuBuffer");
	#endif
}

#endif // RENDERER_D3D11