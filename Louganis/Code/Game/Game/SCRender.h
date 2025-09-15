// Bradley Christensen - 2022-2025
#pragma once
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
class SCRender
{
public:

	std::unordered_map<AssetID, VertexBufferID> m_entityVBOsBySpriteSheet; // Owned by SRenderEntities
};

