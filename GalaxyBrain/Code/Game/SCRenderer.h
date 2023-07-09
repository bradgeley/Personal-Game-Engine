// Bradley Christensen - 2023
#pragma once
#include <unordered_map>



class Texture;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct SCRenderer
{
public:

    std::unordered_map<std::string, VertexBuffer*> m_vbos; // Key: descriptor (sphere_64)
    std::unordered_map<std::string, Texture*> m_textures;  // Key: path or generated name
};
