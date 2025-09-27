// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"
#include "Engine/Renderer/GPUBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
GPUBuffer::GPUBuffer(GpuBufferConfig const& config) : m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool GPUBuffer::IsDirty() const
{
	return m_isDirty;
}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::Reserve(size_t byteWidth)
{
	m_cpuBuffer.reserve(byteWidth);
}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::Resize(size_t byteWidth)
{
	m_isDirty = m_cpuBuffer.size() != byteWidth;
	m_cpuBuffer.resize(byteWidth);
}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::ClearCPUBuffer()
{
	m_isDirty = !m_cpuBuffer.empty();
	m_cpuBuffer.clear();
}



//----------------------------------------------------------------------------------------------------------------------
size_t GPUBuffer::GetCPUBufferSize() const
{
	return m_cpuBuffer.size();
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t const* GPUBuffer::GetCPUBufferData() const
{
	return m_cpuBuffer.data();
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t* GPUBuffer::GetCPUBufferData()
{
	return m_cpuBuffer.data();
}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::UpdateCPUBuffer(void const* data, size_t size)
{
	if (size > m_cpuBuffer.size())
	{
		m_cpuBuffer.resize(size);
	}
	memcpy(m_cpuBuffer.data(), data, size);
	m_isDirty = true;
}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::AddToCPUBuffer(void const* data, size_t size)
{
	if (size == 0)
	{
		return;
	}
	uint8_t const* byteData = static_cast<uint8_t const*>(data);
	m_cpuBuffer.insert(m_cpuBuffer.end(), byteData, byteData + size);
	m_isDirty = true;
}
