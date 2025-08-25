// Bradley Christensen - 2022-2023
#pragma once
#include "Vertex_PCU.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// VertexBuffer
//
// Storage for cpu verts and their connection to a gpu buffer
// Add verts via GetMutableVerts or AddVerts
// When the buffer is drawn, it will first update the gpu buffer if dirty.
// 
// TODO: Make templated based on vertex type? if that's possible
//
class VertexBuffer
{
    friend class Renderer;

protected:

    VertexBuffer() = default;
    
public:

    VertexBuffer(VertexBuffer const& copy) = delete;
    virtual ~VertexBuffer();

    virtual void Initialize(int numExpectedVerts = 3) = 0;
    virtual void UpdateGPUBuffer() = 0;
    virtual void ReleaseResources();
    
    virtual std::vector<Vertex_PCU> const& GetVerts() const;
    virtual std::vector<Vertex_PCU>& GetMutableVerts(bool setDirty = true);
    
    virtual void AddVerts(std::vector<Vertex_PCU> const& verts);
    virtual void ReserveAdditional(int numExpectedAdditionalVerts);
    virtual void ClearVerts();

    virtual int GetStride() const;
	virtual int GetNumVerts() const;

    virtual bool IsDirty() const;
    virtual void SetDirty();
    virtual bool IsEmpty() const;

protected:

    bool m_isDirty = true;
    std::vector<Vertex_PCU> m_verts;
    size_t m_gpuBufferByteWidth = 0;
};
