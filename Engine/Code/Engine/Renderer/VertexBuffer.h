// Bradley Christensen - 2022-2025
#pragma once
#include "Vertex_PCU.h"
#include <vector>



class GPUBuffer;



//----------------------------------------------------------------------------------------------------------------------
// VertexBuffer
//
// Storage for cpu verts and their connection to a gpu buffer
// When the buffer is drawn, it will first update the gpu buffer if dirty.
// Uses a template function to initialize and add verts of any type. After initialization, the verts must be the same size
//
class VertexBuffer
{
    friend class Renderer;

protected:

    VertexBuffer() = default;
    
public:

    VertexBuffer(VertexBuffer const& copy) = delete;
    virtual ~VertexBuffer() = default;

	virtual void ReleaseResources();

    template<typename T>
    void Initialize(size_t initialVertCount = 0);
    
    template<typename T>
    void AddVert(T const& vert);

	template<typename T>
    void AddVerts(std::vector<T> const& verts);

    template<typename T>
    void AddVerts(T const* vertData, size_t numVerts);

    template<typename T>
    T& GetVert(size_t index);

    template<typename T>
    T const& GetVert(size_t index) const;

    virtual void Resize(int numVerts);
    virtual void ReserveAdditional(int numExpectedAdditionalVerts);
    virtual void ClearVerts();

    virtual int GetStride() const;
	virtual int GetNumVerts() const;

	virtual bool IsDirty() const;
    virtual bool IsEmpty() const;
	virtual void UpdateGPUBuffer();

protected:

    virtual void InitializeInternal(size_t vertSize, size_t initialVertCount = 0) = 0;
    virtual void AddVertInternal(void const* vert, size_t vertSize);
    virtual void AddVertsInternal(void const* vert, size_t vertSize, size_t numVerts);
	virtual void* GetVertInternal(size_t vertSize, size_t index);
	virtual void const* GetVertInternal(size_t vertSize, size_t index) const;

protected:

	size_t m_vertSize = 0;
    GPUBuffer* m_gpuBuffer = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void VertexBuffer::Initialize(size_t initialVertCount /*= 0*/)
{
    InitializeInternal(sizeof(T), initialVertCount);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void VertexBuffer::AddVert(T const& vert)
{
	AddVertInternal(&vert, sizeof(T));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void VertexBuffer::AddVerts(std::vector<T> const& verts)
{
	AddVertsInternal(verts.data(), sizeof(T), verts.size());
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline void VertexBuffer::AddVerts(T const* vertData, size_t numVerts)
{
    AddVertsInternal(vertData, sizeof(T), numVerts);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T& VertexBuffer::GetVert(size_t index)
{
    return *reinterpret_cast<T*>(GetVertInternal(sizeof(T), index));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T const& VertexBuffer::GetVert(size_t index) const
{
    return *reinterpret_cast<T const*>(GetVertInternal(sizeof(T), index));
}
