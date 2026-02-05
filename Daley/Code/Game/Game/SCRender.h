// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/AssetID.h"
#include "Engine/Renderer/RendererUtils.h"
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
class SCRender
{
public:

	AssetID m_spriteShaderAsset = AssetID::Invalid;			// Owned by SRenderEntities
	ShaderID m_spriteShaderID = RendererUtils::InvalidID;	// Owned by SRenderEntities

	AssetID m_worldShaderAsset = AssetID::Invalid;			// Owned by SRenderWorld
	ShaderID m_worldShaderID = RendererUtils::InvalidID;	// Owned by SRenderWorld

	ConstantBufferID m_spriteSheetConstantsBuffer	= RendererUtils::InvalidID; // Owned by SRenderEntities
	ConstantBufferID m_lightingConstantsBuffer		= RendererUtils::InvalidID; // Owned by SLighting
	ConstantBufferID m_staticWorldConstantsBuffer	= RendererUtils::InvalidID; // Owned by SRenderWorld

	std::unordered_map<AssetID, VertexBufferID> m_entityVBOsBySpriteSheet; // Owned by SRenderEntities
	std::unordered_map<AssetID, InstanceBufferID> instancesPerSpriteSheet; // Owned by SRenderEntities

	VertexBufferID m_uiVBO = RendererUtils::InvalidID;		// Owned by SRenderUI
};

