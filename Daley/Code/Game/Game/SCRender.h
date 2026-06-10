// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Assets/AssetID.h"
#include "Engine/Renderer/RendererUtils.h"
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
class SCRender
{
public:

	AssetID m_spriteShaderAsset = AssetID::Invalid;				// Owned by SRenderEntities
	AssetID m_worldShaderAsset = AssetID::Invalid;				// Owned by SRenderWorld
	AssetID m_healthBarShaderAsset = AssetID::Invalid;			// Owned by SRenderUI

	ConstantBufferID m_spriteSheetConstantsBuffer	= RendererUtils::InvalidID; // Owned by SRenderEntities
	ConstantBufferID m_lightingConstantsBuffer		= RendererUtils::InvalidID; // Owned by SLighting
	ConstantBufferID m_staticWorldConstantsBuffer	= RendererUtils::InvalidID; // Owned by SRenderWorld
	ConstantBufferID m_healthBarConstantsBuffer		= RendererUtils::InvalidID; // Owned by SRenderUI

	std::unordered_map<AssetID, VertexBufferID> m_entityVBOsBySpriteSheet; // Owned by SRenderEntities
	std::unordered_map<AssetID, InstanceBufferID> instancesPerSpriteSheet; // Owned by SRenderEntities

	VertexBufferID m_uiVBO							= RendererUtils::InvalidID;	// Owned by SRenderUI
	InstanceBufferID m_healthBarInstanceBuffer		= RendererUtils::InvalidID;	// Owned by SRenderUI
};

