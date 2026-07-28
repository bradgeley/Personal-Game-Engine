// Bradley Christensen - 2022-2026
#include "Engine/Renderer/GPUBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
GPUBuffer::GPUBuffer(GpuBufferConfig const& config) : m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
void GPUBuffer::SetDirty()
{
	m_isDirty = true;
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
	m_isDirty = true;
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
	
	size_t oldSize = m_cpuBuffer.size();
	size_t newSize = oldSize + size;
	
	// Preserve vector's exponential growth strategy
	if (newSize > m_cpuBuffer.capacity())
	{
		size_t newCapacity = m_cpuBuffer.capacity() * 2; // 2x growth factor
		if (newCapacity < newSize)
		{
			newCapacity = newSize;
		}
		m_cpuBuffer.reserve(newCapacity);
	}
	
	// Now resize and copy (won't reallocate since we reserved)
	m_cpuBuffer.resize(newSize);
	uint8_t const* byteData = static_cast<uint8_t const*>(data);
	memcpy(&m_cpuBuffer[oldSize], byteData, size);
	
	m_isDirty = true;	
}
