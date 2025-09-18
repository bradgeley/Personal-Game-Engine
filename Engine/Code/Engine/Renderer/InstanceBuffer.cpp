// Bradley Christensen - 2022-2025
#include "InstanceBuffer.h"
#include "Engine/Core/ErrorUtils.h"
#include "GPUBuffer.h"
#include "Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::ReleaseResources()
{
    if (m_gpuBuffer)
    {
        m_gpuBuffer->ReleaseResources();
        delete m_gpuBuffer;
        m_gpuBuffer = nullptr;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::ReserveAdditional(int numExpectedAdditionalInstances)
{
	ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    size_t totalBytes = m_gpuBuffer->GetCPUBufferSize() + numExpectedAdditionalInstances * m_instanceSize;
    m_gpuBuffer->Reserve(totalBytes);
}



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::ClearInstances()
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
	m_gpuBuffer->ClearCPUBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
int InstanceBuffer::GetStride() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    return (int) m_instanceSize; // todo: templatize for different instance layouts?
}



//----------------------------------------------------------------------------------------------------------------------
int InstanceBuffer::GetNumInstances() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    return (int) (m_gpuBuffer->GetCPUBufferSize() / m_instanceSize);
}



//----------------------------------------------------------------------------------------------------------------------
bool InstanceBuffer::IsDirty() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    return m_gpuBuffer->IsDirty();
}



//----------------------------------------------------------------------------------------------------------------------
bool InstanceBuffer::IsEmpty() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    return m_gpuBuffer->GetCPUBufferSize() == 0;
}



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::UpdateGPUBuffer()
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    m_gpuBuffer->UpdateGPUBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::AddInstanceInternal(void const* instance, size_t instanceSize)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
	ASSERT_OR_DIE(instanceSize == m_instanceSize, "InstanceBuffer - Instance size does not match the initialized Instance size.");
    m_gpuBuffer->AddToCPUBuffer(instance, instanceSize);
}



//----------------------------------------------------------------------------------------------------------------------
void InstanceBuffer::AddInstancesInternal(void const* instance, size_t instanceSize, size_t numInstances)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    ASSERT_OR_DIE(instanceSize == m_instanceSize, "InstanceBuffer - Instance size does not match the initialized Instance size.");
    m_gpuBuffer->AddToCPUBuffer(instance, instanceSize * numInstances);
}



//----------------------------------------------------------------------------------------------------------------------
void* InstanceBuffer::GetInstanceInternal(size_t instanceSize, size_t index)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    ASSERT_OR_DIE(instanceSize == m_instanceSize, "InstanceBuffer - Instance size does not match the initialized Instance size.");
    uint8_t* data = m_gpuBuffer->GetCPUBufferData();
	size_t byteIndex = index * m_instanceSize;
	ASSERT_OR_DIE(data != nullptr && byteIndex < m_gpuBuffer->GetCPUBufferSize(), "InstanceBuffer - Index out of bounds.");
	return (void*) (data + byteIndex);
}



//----------------------------------------------------------------------------------------------------------------------
void const* InstanceBuffer::GetInstanceInternal(size_t instanceSize, size_t index) const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_instanceSize > 0, "InstanceBuffer - Instance buffer not properly initialized.");
    ASSERT_OR_DIE(instanceSize == m_instanceSize, "InstanceBuffer - Instance size does not match the initialized Instance size.");
    uint8_t* data = m_gpuBuffer->GetCPUBufferData();
    size_t byteIndex = index * m_instanceSize;
    ASSERT_OR_DIE(data != nullptr && byteIndex < m_gpuBuffer->GetCPUBufferSize(), "InstanceBuffer - Index out of bounds.");
    return (void*) (data + byteIndex);
}
