// Bradley Christensen - 2022-2025
#include "VertexBuffer.h"
#include "Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "GPUBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReleaseResources()
{
    if (m_gpuBuffer)
    {
        m_gpuBuffer->ReleaseResources();
        delete m_gpuBuffer;
        m_gpuBuffer = nullptr;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::Resize(int numVerts)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    size_t totalBytes = numVerts * m_vertSize;
    m_gpuBuffer->Resize(totalBytes);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ReserveAdditional(int numExpectedAdditionalVerts)
{
	ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    size_t totalBytes = m_gpuBuffer->GetCPUBufferSize() + numExpectedAdditionalVerts * m_vertSize;
    m_gpuBuffer->Reserve(totalBytes);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::ClearVerts()
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
	m_gpuBuffer->ClearCPUBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
int VertexBuffer::GetStride() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    return (int) m_vertSize; // todo: templatize for different vert layouts?
}



//----------------------------------------------------------------------------------------------------------------------
int VertexBuffer::GetNumVerts() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    return (int) (m_gpuBuffer->GetCPUBufferSize() / m_vertSize);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::SetDirty()
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    m_gpuBuffer->SetDirty();
}



//----------------------------------------------------------------------------------------------------------------------
bool VertexBuffer::IsDirty() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    return m_gpuBuffer->IsDirty();
}



//----------------------------------------------------------------------------------------------------------------------
bool VertexBuffer::IsEmpty() const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    return m_gpuBuffer->GetCPUBufferSize() == 0;
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::UpdateGPUBuffer()
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    m_gpuBuffer->UpdateGPUBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::AddVertInternal(void const* vert, size_t vertSize)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
	ASSERT_OR_DIE(vertSize == m_vertSize, "VertexBuffer - Vertex size does not match the initialized vertex size.");
    m_gpuBuffer->AddToCPUBuffer(vert, vertSize);
}



//----------------------------------------------------------------------------------------------------------------------
void VertexBuffer::AddVertsInternal(void const* vert, size_t vertSize, size_t numVerts)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    ASSERT_OR_DIE(vertSize == m_vertSize, "VertexBuffer - Vertex size does not match the initialized vertex size.");
    m_gpuBuffer->AddToCPUBuffer(vert, vertSize * numVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void* VertexBuffer::GetVertInternal(size_t vertSize, size_t index)
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    ASSERT_OR_DIE(vertSize == m_vertSize, "VertexBuffer - Vertex size does not match the initialized vertex size.");
    uint8_t* data = m_gpuBuffer->GetCPUBufferData();
	size_t byteIndex = index * m_vertSize;
	ASSERT_OR_DIE(data != nullptr && byteIndex < m_gpuBuffer->GetCPUBufferSize(), "VertexBuffer - Index out of bounds.");
	return (void*) (data + byteIndex);
}



//----------------------------------------------------------------------------------------------------------------------
void const* VertexBuffer::GetVertInternal(size_t vertSize, size_t index) const
{
    ASSERT_OR_DIE(m_gpuBuffer != nullptr && m_vertSize > 0, "VertexBuffer - Vertex buffer not properly initialized.");
    ASSERT_OR_DIE(vertSize == m_vertSize, "VertexBuffer - Vertex size does not match the initialized vertex size.");
    uint8_t* data = m_gpuBuffer->GetCPUBufferData();
    size_t byteIndex = index * m_vertSize;
    ASSERT_OR_DIE(data != nullptr && byteIndex < m_gpuBuffer->GetCPUBufferSize(), "VertexBuffer - Index out of bounds.");
    return (void*) (data + byteIndex);
}
