// Bradley Christensen - 2022-2025
#pragma once
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
class SCRender
{
public:

	AssetID m_spriteShaderAsset = AssetID::Invalid;
	ShaderID m_spriteShaderID = RendererUtils::InvalidID;
	ConstantBufferID m_spriteSheetConstantsBuffer = RendererUtils::InvalidID;
	std::unordered_map<AssetID, VertexBufferID> m_entityVBOsBySpriteSheet; // Owned by SRenderEntities
	std::unordered_map<AssetID, InstanceBufferID> instancesPerSpriteSheet; // Owned by SRenderEntities
};

