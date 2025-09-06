// Bradley Christensen - 2022-2025
#include "GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Image/Image.h"
#include "Engine/Assets/Texture/TextureAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsole.h"
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
IAsset* GridSpriteSheet::Load(Name assetName)
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
		g_devConsole->LogErrorF("GridSpriteSheet::Load - Could not load xml file: %s, Error: %d", xmlPath.c_str(), error);
		return nullptr;
	}

	XmlElement* root = doc.RootElement();
	if (!root)
	{
		g_devConsole->LogErrorF("GridSpriteSheet::Load - Could not find xml root: %s", xmlPath.c_str());
		return nullptr;
	}
	
	GridSpriteSheet* spriteSheet = new GridSpriteSheet();
	spriteSheet->m_name = XmlUtils::ParseXmlAttribute(*root, "name", Name::s_invalidName);
	spriteSheet->m_textureName = XmlUtils::ParseXmlAttribute(*root, "texture", Name::s_invalidName);
	spriteSheet->m_layout = XmlUtils::ParseXmlAttribute(*root, "layout", IntVec2(1, 1));
	spriteSheet->m_edgePadding = XmlUtils::ParseXmlAttribute(*root, "edgePadding", IntVec2(0, 0));
	spriteSheet->m_innerPadding = XmlUtils::ParseXmlAttribute(*root, "innerPadding", IntVec2(0, 0));

	ASSERT_OR_DIE(spriteSheet->m_textureName != Name::s_invalidName, "GridSpriteSheet::Load - Sprite sheet texture name is invalid.");

	XmlElement* animationDefElem = root->FirstChildElement("SpriteAnimation");
	while (animationDefElem != nullptr)
	{
		size_t newIndex = spriteSheet->m_animations.size();
		spriteSheet->m_animations.emplace_back(SpriteAnimationDef());
		spriteSheet->m_animations[newIndex].LoadFromXml(animationDefElem);

		animationDefElem = animationDefElem->NextSiblingElement("SpriteAnimation");
	}

	return spriteSheet;
}



//----------------------------------------------------------------------------------------------------------------------
bool GridSpriteSheet::CompleteAsyncLoad()
{
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
	m_textureAsset = g_assetManager->LoadSynchronous<TextureAsset>(m_textureName);
	TextureAsset* textureAsset = g_assetManager->Get<TextureAsset>(m_textureAsset);
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
	g_assetManager->Release(m_texture);
	m_spriteUVs.Clear();
	m_animations.clear();
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
SpriteAnimationDef const* GridSpriteSheet::GetAnimationDef(Name name) const
{
	for (SpriteAnimationDef const& animDef : m_animations)
	{
		if (animDef.GetName() == name)
		{
			return &animDef;
		}
	}
	return nullptr;
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

			// Squeeze to avoid bleeding into neighboring sprites
			float minDim = MathUtils::Min(uvs.GetWidth(), uvs.GetHeight());
			uvs.Squeeze(minDim * 0.01f);

			m_spriteUVs.Set(index, uvs);
		}
	}
}
