// Bradley Christensen - 2022-2026
#include "GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image.h"
#include "Engine/Assets/TextureAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "ThirdParty/tinyxml2/tinyxml2.h"



//----------------------------------------------------------------------------------------------------------------------
bool GridSpriteSheet::CreateFromTexture(TextureID texture, IntVec2 const& layout, IntVec2 const& edgePadding, IntVec2 const& innerPadding)
{
	m_texture = texture;
	m_layout = layout;
	m_spriteUVs.Initialize(layout, AABB2::ZeroToOne);
	m_edgePadding = edgePadding;
	m_innerPadding = innerPadding;

	ComputeSpriteUVs();

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
Asset* GridSpriteSheet::Load(Name assetName)
{
	std::string const& xmlPath = assetName.ToString();
	Strings pathAndExt = StringUtils::SplitStringOnDelimiter(xmlPath, '.');
	if (pathAndExt.size() == 1)
	{
		pathAndExt[0] += ".xml"; // Assume no extension means XML
	}

	if (pathAndExt.size() == 2)
	{
		ASSERT_OR_DIE(pathAndExt[1] == "xml", "GridSpriteSheet::Load - Invalid asset name format. Expected 'path/to/spritesheet.xml'");
	}

	if (pathAndExt.size() > 2)
	{
		ERROR_AND_DIE("GridSpriteSheet::Load - Invalid asset name format. Expected 'path/to/spritesheet.xml'");
	}

	XmlDocument doc;
	XmlError error = doc.LoadFile(xmlPath.c_str());
	if (error != tinyxml2::XML_SUCCESS)
	{
		DevConsoleUtils::LogError("GridSpriteSheet::Load - Failed to load xml file: %s, Error: %d", xmlPath.c_str(), error);
		return nullptr;
	}

	XmlElement* root = doc.RootElement();
	if (!root)
	{
		DevConsoleUtils::LogError("GridSpriteSheet::Load - Failed to find root element in xml file: %s", xmlPath.c_str());
		return nullptr;
	}
	
	GridSpriteSheet* spriteSheet = new GridSpriteSheet();
	spriteSheet->m_name = XmlUtils::ParseXmlAttribute(*root, "name", Name::Invalid);
	spriteSheet->m_textureName = XmlUtils::ParseXmlAttribute(*root, "texture", Name::Invalid);
	spriteSheet->m_layout = XmlUtils::ParseXmlAttribute(*root, "layout", IntVec2(1, 1));
	spriteSheet->m_edgePadding = XmlUtils::ParseXmlAttribute(*root, "edgePadding", IntVec2(0, 0));
	spriteSheet->m_innerPadding = XmlUtils::ParseXmlAttribute(*root, "innerPadding", IntVec2(0, 0));

	ASSERT_OR_DIE(spriteSheet->m_textureName != Name::Invalid, "GridSpriteSheet::Load - Sprite sheet texture name is invalid.");

	XmlElement* animationGrpElem = root->FirstChildElement("SpriteAnimationGroup");
	while (animationGrpElem != nullptr)
	{
		size_t newIndex = spriteSheet->m_animationGroups.size();
		spriteSheet->m_animationGroups.emplace_back(SpriteAnimationGroup());
		spriteSheet->m_animationGroups[newIndex].LoadFromXml(animationGrpElem);

		animationGrpElem = animationGrpElem->NextSiblingElement("SpriteAnimationGroup");
	}

	// Direct sprite animations that are not part of a group, just make a group for each
	XmlElement* animationElem = root->FirstChildElement("SpriteAnimation");
	while (animationElem != nullptr)
	{
		spriteSheet->m_animationGroups.emplace_back(SpriteAnimationGroup());
		SpriteAnimationGroup& newAnimGroup = spriteSheet->m_animationGroups.back();
		newAnimGroup.m_name = XmlUtils::ParseXmlAttribute(*animationElem, "name", Name::Invalid);

		newAnimGroup.m_animationDefs.emplace_back(SpriteAnimationDef());
		SpriteAnimationDef& newAnimDef = newAnimGroup.m_animationDefs.back();
		newAnimDef.LoadFromXml(animationElem);

		animationElem = animationElem->NextSiblingElement("SpriteAnimation");
	}

	spriteSheet->m_numAnimations = spriteSheet->CountNumAnimations();

	return spriteSheet;
}



//----------------------------------------------------------------------------------------------------------------------
bool GridSpriteSheet::CompleteAsyncLoad()
{
	ASSERT_OR_DIE(g_assetManager != nullptr, "GridSpriteSheet::CompleteSyncLoad - Asset manager is null.");

	if (m_textureAsset == RendererUtils::InvalidID)
	{
		m_textureAsset = g_assetManager->AsyncLoad<TextureAsset>(m_textureName);
	}

	TextureAsset* textureAsset = g_assetManager->Get<TextureAsset>(m_textureAsset);
	if (textureAsset == nullptr)
	{
		return false;
	}

	m_texture = textureAsset->GetTextureID();

	bool succeeded = CreateFromTexture(m_texture, m_layout, m_edgePadding, m_innerPadding);
	ASSERT_OR_DIE(succeeded, "GridSpriteSheet::CompleteAsyncLoad - Failed to create sprite sheet from texture.");

	return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
bool GridSpriteSheet::CompleteSyncLoad()
{
	ASSERT_OR_DIE(g_assetManager != nullptr, "GridSpriteSheet::CompleteSyncLoad - Asset manager is null.");

	m_textureAsset = g_assetManager->LoadSynchronous<TextureAsset>(m_textureName);
	TextureAsset* textureAsset = g_assetManager->Get<TextureAsset>(m_textureAsset);

	ASSERT_OR_DIE(textureAsset != nullptr, "GridSpriteSheet::CompleteSyncLoad - Failed to load texture asset synchronously.");
	if (textureAsset == nullptr)
	{
		return false;
	}

	m_texture = textureAsset->GetTextureID();
	bool succeeded = CreateFromTexture(m_texture, m_layout, m_edgePadding, m_innerPadding);
	ASSERT_OR_DIE(succeeded, "GridSpriteSheet::CompleteSyncLoad - Failed to create sprite sheet from texture.");

	return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
void GridSpriteSheet::ReleaseResources()
{
	if (g_renderer)
	{
		g_renderer->ReleaseTexture(m_texture);
	}
	g_assetManager->Release(m_textureAsset);
	m_spriteUVs.Clear();
	m_animationGroups.clear();
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
IntVec2 GridSpriteSheet::GetTextureDimensions() const
{
	Texture* texture = g_renderer->GetTexture(m_texture);
	if (texture)
	{
		return texture->GetDimensions();
	}
	ERROR_AND_DIE("GridSpriteSheet::GetTextureDimensions: Cannot determine texture dimensions because texture is invalid.")
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimationDef const* GridSpriteSheet::GetAnimationDef(Name name) const
{
	for (SpriteAnimationGroup const& animGroup : m_animationGroups)
	{
		for (SpriteAnimationDef const& animDef : animGroup.GetAnimationDefs())
		{
			if (animDef.GetName() == name)
			{
				return &animDef;
			}
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
SpriteAnimationGroup const* GridSpriteSheet::GetAnimationGroup(Name name) const
{
	for (auto& animGroup : m_animationGroups)
	{
		if (animGroup.GetName() == name)
		{
			return &animGroup;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 GridSpriteSheet::GetLayout() const
{
	return m_layout;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 GridSpriteSheet::GetEdgePadding() const
{
	return m_edgePadding;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 GridSpriteSheet::GetInnerPadding() const
{
	return m_innerPadding;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 GridSpriteSheet::GetGenericSpriteQuad(float size /*= 1.f*/) const
{
	AABB2 spriteAABB;
	float spriteAspect = GetSpriteAspect();
	float halfSize = size * 0.5f;
	if (spriteAspect <= 1.f)
	{
		spriteAABB.mins = Vec2(-halfSize, -halfSize * spriteAspect);
		spriteAABB.maxs = spriteAABB.mins + Vec2(2.f * halfSize, 2.f * halfSize / spriteAspect);
	}
	else
	{
		spriteAABB.mins = Vec2(-halfSize * spriteAspect, -halfSize);
		spriteAABB.maxs = spriteAABB.mins + Vec2(2.f * halfSize * spriteAspect, 2.f * halfSize);
	}
	return spriteAABB;
}



//----------------------------------------------------------------------------------------------------------------------
int GridSpriteSheet::CountNumAnimations() const
{
	int count = 0;
	for (auto& grp : m_animationGroups)
	{
		count += static_cast<int>(grp.m_animationDefs.size());
	}
	return count;
}



//----------------------------------------------------------------------------------------------------------------------
int GridSpriteSheet::GetNumAnimations() const
{
	return m_numAnimations;
}



//----------------------------------------------------------------------------------------------------------------------
void GridSpriteSheet::SetRendererState() const
{
	g_renderer->SetCullMode(CullMode::None);
	g_renderer->SetFillMode(FillMode::Solid);
	g_renderer->SetBlendMode(BlendMode::Alpha);
	g_renderer->BindTexture(m_texture);
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
			uvs.mins.x = static_cast<float>(m_edgePadding.x + (spriteX * m_innerPadding.x) + (spriteX * m_spriteDims.x));
			uvs.mins.y = static_cast<float>(m_edgePadding.y + (spriteY * m_innerPadding.y) + (spriteY * m_spriteDims.y));
			uvs.maxs = uvs.mins + Vec2(m_spriteDims);

			// Convert to UVs
			uvs.mins /= Vec2(textureDims);
			uvs.maxs /= Vec2(textureDims);

			// Squeeze to avoid bleeding into neighboring sprites
			float minDim = MathUtils::Min(uvs.GetWidth(), uvs.GetHeight());
			uvs.Squeeze(minDim * 0.01f);

			m_spriteUVs.Set(index, uvs);
		}
	}
}
