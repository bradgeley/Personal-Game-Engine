﻿// Bradley Christensen - 2022-2023
#pragma once
#include "Vertex_PCU.h"
#include <vector>



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// VertexBuffer
//
// Storage for cpu verts and their connection to a gpu buffer
// Add verts via GetMutableVerts or AddVerts
// When the buffer is drawn, it will first update the gpu buffer if dirty.
//
class VertexBuffer
{
    friend class Renderer;
    
public:

    VertexBuffer() = default;
    VertexBuffer(VertexBuffer const& copy) = delete;
    ~VertexBuffer();

    void Initialize(int numExpectedVerts = 3);
    void ReleaseResources();
    
    std::vector<Vertex_PCU> const& GetVerts() const;
    std::vector<Vertex_PCU>& GetMutableVerts(); // Don't call unless you will be adding verts
    
    void AddVerts(std::vector<Vertex_PCU> const& verts);
    void ClearVerts();
    void UpdateGPUBuffer();

    int GetStride() const;
	int GetNumVerts() const;

    bool IsDirty() const;

protected:

    bool m_isDirty = false;
    std::vector<Vertex_PCU> m_verts;
    
    ID3D11Buffer* m_handle = nullptr;
    size_t m_gpuBufferByteWidth = 0;
};
