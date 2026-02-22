// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/Grid.h"
#include "SpriteAnimationGroup.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
class GridSpriteSheet : public Asset
{
public:

	GridSpriteSheet() = default;
	~GridSpriteSheet() = default;

	// Initialization
	bool CreateFromTexture(TextureID texture, IntVec2 const& layout, IntVec2 const& edgePadding = IntVec2::ZeroVector, IntVec2 const& innerPadding = IntVec2::ZeroVector);

	// Loading
	static Asset* Load(Name assetName);
	virtual bool CompleteAsyncLoad() override;
	virtual bool CompleteSyncLoad() override;
	virtual void ReleaseResources() override;

	// Getters
	AABB2 GetSpriteUVs(int spriteIndex) const;
	float GetSpriteAspect() const;
	IntVec2 GetTextureDimensions() const;
	SpriteAnimationDef const* GetAnimationDef(Name name) const;
	SpriteAnimationGroup const* GetAnimationGroup(Name name) const;
	IntVec2 GetLayout() const;
	IntVec2 GetEdgePadding() const;
	IntVec2 GetInnerPadding() const;
	AABB2 GetGenericSpriteQuad(float size = 1.f) const;		// In NDC space, centered on origin, with correct aspect ratio
	int CountNumAnimations() const;
	int GetNumAnimations() const;

	// Setters
	void SetRendererState() const;

protected:

	void ComputeSpriteUVs();

protected:

	TextureID	m_texture		= RendererUtils::InvalidID;		// Cached after load
	IntVec2		m_layout		= IntVec2::ZeroVector;			// Dimensions in sprites (not pixels)
	IntVec2		m_spriteDims	= IntVec2::ZeroVector;			// Dimensions of each sprite in pixels
	Grid<AABB2> m_spriteUVs;									// Precomputed UVs for each sprite
	IntVec2		m_edgePadding	= IntVec2::ZeroVector;			// Around the whole sheet
	IntVec2		m_innerPadding	= IntVec2::ZeroVector;			// Between sprites
	int			m_numAnimations = 0;
	std::vector<SpriteAnimationGroup> m_animationGroups;		// Animations that use this sprite sheet, loaded from the xml

	// Used during the load process
	Name		m_textureName	= Name::Invalid;
	AssetID		m_textureAsset	= AssetID::Invalid;				// Loaded texture asset id
};