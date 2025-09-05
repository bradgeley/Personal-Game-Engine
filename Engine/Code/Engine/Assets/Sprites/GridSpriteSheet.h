// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/Grid.h"
#include "SpriteAnimation.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
class GridSpriteSheet : public IAsset
{
public:

	GridSpriteSheet() = default;
	~GridSpriteSheet() = default;

	static IAsset* Load(Name assetName);
	virtual bool CompleteLoad() override;
	virtual void ReleaseResources() override;

	bool CreateFromTexture(TextureID texture, IntVec2 const& layout, IntVec2 const& edgePadding = IntVec2::ZeroVector, IntVec2 const& innerPadding = IntVec2::ZeroVector);

	AABB2 GetSpriteUVs(int spriteIndex) const;
	float GetSpriteAspect() const;
	SpriteAnimationDef const* GetAnimationDef(Name name) const;

	void SetRendererState() const;

protected:

	void ComputeSpriteUVs();

protected:

	AssetID		m_image			= INVALID_ASSET_ID;
	TextureID	m_texture 		= RendererUtils::InvalidID;
	IntVec2		m_layout		= IntVec2::ZeroVector;			// Dimensions in sprites (not pixels)
	IntVec2		m_spriteDims	= IntVec2::ZeroVector;			// Dimensions of each sprite in pixels
	Grid<AABB2> m_spriteUVs;									// Precomputed UVs for each sprite
	IntVec2		m_edgePadding	= IntVec2::ZeroVector;			// Around the whole sheet
	IntVec2		m_innerPadding	= IntVec2::ZeroVector;			// Between sprites
	std::vector<SpriteAnimationDef> m_animations;				// Animations that use this sprite sheet
};