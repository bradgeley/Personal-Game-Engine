// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/AssetID.h"
#include "Engine/Renderer/RendererUtils.h"
#include <unordered_map>



class Renderer;



//----------------------------------------------------------------------------------------------------------------------
class SCRenderer
{
public:

	Renderer const* GetRenderer() const		{ return m_renderer; }
	Renderer* GetRenderer()					{ return m_renderer; }
	void SetRenderer(Renderer* renderer)	{ m_renderer = renderer; }

public:

	AssetID m_spriteShaderAsset						= AssetID::Invalid;				// Owned by SRenderEntities
	AssetID m_worldShaderAsset						= AssetID::Invalid;				// Owned by SRenderWorld
	AssetID m_healthBarShaderAsset					= AssetID::Invalid;				// Owned by SRenderUI
	AssetID m_discShaderAsset						= AssetID::Invalid;				// Owned by SRenderDiscs
	AssetID m_iconsSpriteSheet						= AssetID::Invalid;				// Owned by SRenderUI
	AssetID m_statusEffectsSpriteSheet				= AssetID::Invalid;				// Owned by SVisualEffects

	ConstantBufferID m_spriteSheetConstantsBuffer	= RendererUtils::InvalidID;		// Owned by SRenderEntities
	ConstantBufferID m_lightingConstantsBuffer		= RendererUtils::InvalidID;		// Owned by SLighting
	ConstantBufferID m_staticWorldConstantsBuffer	= RendererUtils::InvalidID;		// Owned by SRenderWorld
	ConstantBufferID m_healthBarConstantsBuffer		= RendererUtils::InvalidID;		// Owned by SRenderUI
	ConstantBufferID m_discConstantsBuffer			= RendererUtils::InvalidID;		// Owned by SRenderDiscs

	InstanceBufferID m_healthBarInstanceBuffer		= RendererUtils::InvalidID;		// Owned by SRenderUI
	InstanceBufferID m_iconsInstanceBuffer			= RendererUtils::InvalidID;		// Owned by SRenderUI
	InstanceBufferID m_discInstanceBuffer			= RendererUtils::InvalidID;		// Owned by SRenderDiscs

	VertexBufferID m_immediateVBO					= RendererUtils::InvalidID;		// Owned by SRenderUI

	VertexBufferID m_popupOverlayBackgroundVBO		= RendererUtils::InvalidID;		// Owned by SRenderPopupOverlay
	VertexBufferID m_popupOverlayTextVBO			= RendererUtils::InvalidID;		// Owned by SRenderPopupOverlay

	VertexBufferID m_hudUntexturedVBO				= RendererUtils::InvalidID;		// Owned by SRenderHUD
	VertexBufferID m_hudTextVBO						= RendererUtils::InvalidID;		// Owned by SRenderHUD

	std::unordered_map<AssetID, InstanceBufferID> m_instancesPerSpriteSheet;		// Owned by SRenderEntities

private:

	Renderer* m_renderer = nullptr;
};