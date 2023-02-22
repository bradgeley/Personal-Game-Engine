// Bradley Christensen - 2022
#pragma once
#include "Vertex_PCU.h"
#include <vector>



struct ID3D11Buffer;



class VertexBuffer
{
    friend class Renderer;
    
public:

    VertexBuffer(VertexBuffer const& copy) = delete;

    void Initialize(int numExpectedVerts = 3);
    
    void AddVerts(std::vector<Vertex_PCU> const& verts);

    int GetStride() const;
	int GetNumVerts() const;

protected:
    
    ID3D11Buffer* m_handle = nullptr;

    std::vector<Vertex_PCU> m_verts;
};
