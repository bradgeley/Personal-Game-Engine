// Bradley Christensen - 2022
#pragma once
#include "Vertex_PCU.h"
#include <vector>



struct ID3D11Buffer;



class VertexBuffer
{
    friend class Renderer;
    
public:

    VertexBuffer() = default;
    VertexBuffer(VertexBuffer const& copy) = delete;

    void Initialize(int numExpectedVerts = 3);
    
    void AddVerts(std::vector<Vertex_PCU> const& verts);
    void UpdateGPUBuffer();

    int GetStride() const;
	int GetNumVerts() const;

private:


protected:
    
    ID3D11Buffer* m_handle = nullptr;
    size_t m_gpuBufferByteSize = 0;

    std::vector<Vertex_PCU> m_verts;
};
