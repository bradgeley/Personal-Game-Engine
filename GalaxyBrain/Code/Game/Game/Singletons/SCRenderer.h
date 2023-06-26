// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
enum class GameVboIndex
{
    Sprite,
    Sphere,
    Num,
};



//----------------------------------------------------------------------------------------------------------------------
struct SCRenderer
{
public:

    VertexBuffer m_vbos[(int) GameVboIndex::Num];
};
