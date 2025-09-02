// Bradley Christensen - 2022-2025
#include "GridSpriteSheet.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
bool GridSpriteSheet::CreateFromTexture(TextureID texture, IntVec2 const& layout, IntVec2 const& edgePadding, IntVec2 const& innerPadding)
{
	if (m_texture != RendererUtils::InvalidID) 
	{
		// Already initialized
		return false;
	}

	m_texture = texture;
	m_layout = layout;
	m_spriteUVs.Initialize(layout, AABB2::ZeroToOne);
	m_edgePadding = edgePadding;
	m_innerPadding = innerPadding;

	ComputeSpriteUVs();

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GridSpriteSheet::GetSpriteUVs(int spriteIndex) const
{
	if (m_spriteUVs.IsValidIndex(spriteIndex)) 
	{
		return m_spriteUVs.Get(spriteIndex);
	}
	return AABB2::ZeroToOne;
}



//----------------------------------------------------------------------------------------------------------------------
float GridSpriteSheet::GetSpriteAspect() const
{
	return static_cast<float>(m_spriteDims.x) / static_cast<float>(m_spriteDims.y);
}



//----------------------------------------------------------------------------------------------------------------------
void GridSpriteSheet::SetRendererState() const
{
	g_renderer->SetCullMode(CullMode::None);
	g_renderer->SetFillMode(FillMode::Solid);
	g_renderer->SetBlendMode(BlendMode::Alpha);
	g_renderer->BindTexture(m_texture);
	g_renderer->BindShader(nullptr);
}



//----------------------------------------------------------------------------------------------------------------------
void GridSpriteSheet::ComputeSpriteUVs()
{
	if (m_texture == RendererUtils::InvalidID)
	{
		ERROR_AND_DIE("Invalid texture ID.")
		return;
	}
	Texture* texture = g_renderer->GetTexture(m_texture);
	if (!texture)
	{
		ERROR_AND_DIE("Invalid texture.")
		return;
	}
	IntVec2 textureDims = texture->GetDimensions();
	if (textureDims == IntVec2::ZeroVector) 
	{
		ERROR_AND_DIE("Texture has no dimensions.")
		return;
	}

	m_spriteDims = (textureDims - (m_edgePadding * 2) - (m_innerPadding * (m_layout - IntVec2::OneVector))) / m_layout;
	for (int spriteY = 0; spriteY < m_layout.y; ++spriteY)
	{
		for (int spriteX = 0; spriteX < m_layout.x; ++spriteX)
		{
			int index = m_spriteUVs.GetIndexForCoords(spriteX, spriteY);

			AABB2 uvs; 

			// Compute location/dims in pixels
			uvs.mins.x = static_cast<float>(m_edgePadding.x + (spriteX - 1) * m_innerPadding.x + spriteX * m_spriteDims.x);
			uvs.mins.y = static_cast<float>(m_edgePadding.y + (spriteY - 1) * m_innerPadding.y + spriteY * m_spriteDims.y);
			uvs.maxs = uvs.mins + Vec2(m_spriteDims);

			// Convert to UVs
			uvs.mins /= Vec2(textureDims);
			uvs.maxs /= Vec2(textureDims);

			m_spriteUVs.Set(index, uvs);
		}
	}
}
