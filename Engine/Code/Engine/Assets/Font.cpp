// Bradley Christensen - 2022-2026
#include "Font.h"
#include "AssetManager.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Vec2 Font::AlignCentered		= Vec2(0.f, 0.f);
Vec2 Font::AlignTopCenter		= Vec2(0.f, 1.f);
Vec2 Font::AlignBottomCenter	= Vec2(0.f, -1.f);
Vec2 Font::AlignRightCenter		= Vec2(1.0f, 0.f);
Vec2 Font::AlignLeftCenter		= Vec2(-1.f, 0.f);
Vec2 Font::AlignTopRight		= Vec2(1.f, 1.f);
Vec2 Font::AlignTopLeft			= Vec2(-1.f, 1.f);
Vec2 Font::AlignBottomRight		= Vec2(1.f, -1.f);
Vec2 Font::AlignBottomLeft		= Vec2(-1.f, -1.f);



//----------------------------------------------------------------------------------------------------------------------
Asset* Font::Load(Name assetName)
{
	Font* font = new Font();

	XmlDocument resourcesXMLDoc;
	XmlError error = resourcesXMLDoc.LoadFile(assetName.ToCStr());
	if (error != XmlError::XML_SUCCESS)
	{
		ERROR_AND_DIE(StringUtils::StringF("Font: %s::Load - Failed to open Xml doc.", assetName.ToCStr()))
	}

	XmlElement const* fontElem = resourcesXMLDoc.RootElement();
	if (fontElem)
	{
		auto commonElem = fontElem->FirstChildElement("common");
		int lineHeight = commonElem->FindAttribute("lineHeight")->IntValue();
		float oneOverLightHeight = 1.f / (float) lineHeight;
		int imagePixelWidth = commonElem->FindAttribute("scaleW")->IntValue();
		int imagePixelHeight = commonElem->FindAttribute("scaleH")->IntValue();
		int pages = commonElem->FindAttribute("pages")->IntValue();
		ASSERT_OR_DIE(pages == 1, StringUtils::StringF("Font: %s::Load - more than 1 page for font not supported.", assetName.ToCStr()))

		// get texture name
		auto pagesElem = fontElem->FirstChildElement("pages");
		auto page = pagesElem->FirstChildElement("page");
		auto filename = page->FindAttribute("file");
		std::string fullFilePath = "Data/Fonts/" + std::string(filename->Value());

		// Set up texture, but don't create gpu resources yet
		font->m_textureName = fullFilePath;

		auto charsElem = fontElem->FirstChildElement("chars");
		auto count = charsElem->FindAttribute("count");

		int charCount = count->IntValue();
		ASSERT_OR_DIE(charCount <= 256, StringUtils::StringF("Font: %s::Load - char count exceeds supported amount (256)", assetName.ToCStr()))

		auto charElem = charsElem->FirstChildElement("char");
		for (int i = 0; i < charCount; ++i)
		{
			int charID = charElem->FindAttribute("id")->IntValue();
			int width = charElem->FindAttribute("width")->IntValue();
			int height = charElem->FindAttribute("height")->IntValue();
			int x = charElem->FindAttribute("x")->IntValue();
			int y = charElem->FindAttribute("y")->IntValue();
			int xoffset = charElem->FindAttribute("xoffset")->IntValue();
			int xadvance = charElem->FindAttribute("xadvance")->IntValue();
			int yoffset = charElem->FindAttribute("yoffset")->IntValue();

			float uvMinX = float(x) / float(imagePixelWidth);
			float uvMaxX = float(x + width) / float(imagePixelWidth);
			float uvMinY = float(imagePixelHeight - y - height) / float(imagePixelHeight); // min y uvs has the added height because 0,0 is in top left
			float uvMaxY = float(imagePixelHeight - y) / float(imagePixelHeight);

			GlyphData& charData = font->m_glyphData[charID];
			charData.m_aspect = float(width) / float(height);
			charData.m_uvMins = Vec2(uvMinX, uvMinY);
			charData.m_uvMaxs = Vec2(uvMaxX, uvMaxY);
			charData.a = float(xoffset) * oneOverLightHeight;
			charData.b = float(width) * oneOverLightHeight;
			charData.c = float(xadvance - width - xoffset) * oneOverLightHeight;
			charData.m_xAdvance = float(xadvance) * oneOverLightHeight;
			charData.m_yOffset = float(yoffset) * oneOverLightHeight;
			charData.m_width = float(width) * oneOverLightHeight;
			charData.m_height = float(height) * oneOverLightHeight;
			charElem = charElem->NextSiblingElement("char");
		}

		auto kerningsElem = fontElem->FirstChildElement("kernings");
		int kerningsCount = kerningsElem->FindAttribute("count")->IntValue();
		auto kerningElem = kerningsElem->FirstChildElement("kerning");

		for (int i = 0; i < kerningsCount; ++i)
		{
			int first = kerningElem->FindAttribute("first")->IntValue();
			int second = kerningElem->FindAttribute("second")->IntValue();
			int amount = kerningElem->FindAttribute("amount")->IntValue();

			float scaledKerning = (float) amount * oneOverLightHeight;

			font->m_kerningData[first].emplace_back(second, scaledKerning);
			kerningElem = kerningElem->NextSiblingElement("kerning");
		}
	}

	return font;
}



//----------------------------------------------------------------------------------------------------------------------
bool Font::CompleteAsyncLoad()
{
	if (m_texture == RendererUtils::InvalidID)
	{
		if (!g_assetManager->IsValid(m_textureAsset))
		{
			m_textureAsset = g_assetManager->AsyncLoad<TextureAsset>(m_textureName);
			ASSERT_OR_DIE(m_textureAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::CompleteAsyncLoad - failed to load texture %s", m_name.ToCStr(), m_textureName.ToCStr()));
		}

		TextureAsset const* asset = g_assetManager->Get<TextureAsset>(m_textureAsset);
		if (asset != nullptr)
		{
			m_texture = asset->GetTextureID();
			Texture* texture = g_renderer->GetTexture(m_texture);
			ASSERT_OR_DIE(texture != nullptr && texture->IsValid(), StringUtils::StringF("Font: %s::CompleteAsyncLoad - texture asset loaded but renderer failed to get texture %s", m_name.ToCStr(), m_textureName.ToCStr()));
		}
	}

	if (m_shader == RendererUtils::InvalidID)
	{
		if (!g_assetManager->IsValid(m_shaderAsset))
		{
			m_shaderAsset = g_assetManager->AsyncLoad<ShaderAsset>(Name("Data/Shaders/DefaultFontShader.xml"));
			ASSERT_OR_DIE(m_shaderAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::CompleteAsyncLoad - failed to load shader %s", m_name.ToCStr(), "Data/Shaders/DefaultFontShader.xml"));
		}

		ShaderAsset const* shaderAsset = g_assetManager->Get<ShaderAsset>(m_shaderAsset);
		if (shaderAsset != nullptr)
		{
			m_shader = shaderAsset->GetShaderID();
			Shader* shader = g_renderer->GetShader(m_shader);
			ASSERT_OR_DIE(shader != nullptr && shader->IsValid(), StringUtils::StringF("Font: %s::CompleteAsyncLoad - shader asset loaded but shader is invalid %s", m_name.ToCStr(), "Data/Shaders/DefaultFontShader.xml"));
		}
	}

	bool succeeded = (m_texture != RendererUtils::InvalidID) && (m_shader != RendererUtils::InvalidID);
	return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
bool Font::CompleteSyncLoad()
{
	// Load Texture
	m_textureAsset = g_assetManager->LoadSynchronous<TextureAsset>(m_textureName);
	ASSERT_OR_DIE(m_textureAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::CompleteSyncLoad - failed to load texture %s", m_name.ToCStr(), m_textureName.ToCStr()));

	TextureAsset const* asset = g_assetManager->Get<TextureAsset>(m_textureAsset);
	ASSERT_OR_DIE(asset != nullptr, StringUtils::StringF("Font: %s::CompleteSyncLoad - failed to get texture %s", m_name.ToCStr(), m_textureName.ToCStr()));

	m_texture = asset->GetTextureID();

	// Load Shader
	m_shaderAsset = g_assetManager->LoadSynchronous<ShaderAsset>(Name("Data/Shaders/DefaultFontShader.xml"));
	ASSERT_OR_DIE(m_shaderAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::CompleteSyncLoad - failed to load shader %s", m_name.ToCStr(), "Data/Shaders/DefaultFontShader.xml"));

	ShaderAsset const* shaderAsset = g_assetManager->Get<ShaderAsset>(m_shaderAsset);
	ASSERT_OR_DIE(shaderAsset != nullptr, StringUtils::StringF("Font: %s::CompleteSyncLoad - failed to get shader %s", m_name.ToCStr(), "Data/Shaders/DefaultFontShader.xml"));

	m_shader = shaderAsset->GetShaderID();

	bool succeeded = (m_texture != RendererUtils::InvalidID) && (m_shader != RendererUtils::InvalidID);
	return succeeded;
}



//----------------------------------------------------------------------------------------------------------------------
void Font::ReleaseResources()
{
	g_assetManager->Release(m_textureAsset);
	g_assetManager->Release(m_shaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
AssetID Font::GetLoadDependency() const
{
	if (m_texture == RendererUtils::InvalidID)
	{
		ASSERT_OR_DIE(m_textureAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::GetLoadDependency - Texture asset is invalid.", m_name.ToCStr()));
		TextureAsset const* asset = g_assetManager->Get<TextureAsset>(m_textureAsset);
		if (asset == nullptr)
		{
			return m_textureAsset;
		}
	}

	if (m_shader == RendererUtils::InvalidID)
	{
		ASSERT_OR_DIE(m_shaderAsset != AssetID::Invalid, StringUtils::StringF("Font: %s::GetLoadDependency - Shader asset is invalid.", m_name.ToCStr()));
		ShaderAsset const* shaderAsset = g_assetManager->Get<ShaderAsset>(m_shaderAsset);
		if (shaderAsset == nullptr)
		{
			return m_shaderAsset;
		}
	}

	return AssetID::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
void Font::SetRendererState(Renderer& renderer) const
{
	renderer.SetCullMode(CullMode::None);
	renderer.SetWindingOrder(Winding::CounterClockwise);
	renderer.SetFillMode(FillMode::Solid);
	renderer.SetBlendMode(BlendMode::Alpha);
	renderer.SetSamplerMode(SamplerFilter::Trilinear, SamplerAddressMode::Clamp);
	renderer.BindTexture(m_texture);
	renderer.BindShader(m_shader);
}



//----------------------------------------------------------------------------------------------------------------------
void Font::AddVertsForText2D(VertexBuffer& out_verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint) const
{
	float penPosition = 0.f;
	for (int glyphIndex = 0; glyphIndex < (int) text.size(); ++glyphIndex)
	{
		uint8_t const& currentChar = text[glyphIndex];
		GlyphData const& charData = m_glyphData[currentChar];
		
		float a = charData.a * cellHeight;
		float c = charData.c * cellHeight;
		float yOffset = charData.m_yOffset * cellHeight;
		
		penPosition += a;
		
		float glyphWidth = cellHeight * charData.m_width;
		float glyphHeight = cellHeight * charData.m_height;

		Vec2 currentGlyphPosTopLeft = textMins + Vec2(penPosition, cellHeight - yOffset); // start glyphs at the top left for some dumb reason
		Vec2 currentGlyphPosBotRight = currentGlyphPosTopLeft + Vec2(glyphWidth, -glyphHeight);
		
		AABB2 bounds, UVs;
		bounds.mins = Vec2(currentGlyphPosTopLeft.x, currentGlyphPosBotRight.y);
		bounds.maxs = Vec2(currentGlyphPosBotRight.x, currentGlyphPosTopLeft.y);

		UVs.mins = m_glyphData[currentChar].m_uvMins;
		UVs.maxs = m_glyphData[currentChar].m_uvMaxs;

		VertexUtils::AddVertsForAABB2(out_verts, bounds, tint, UVs);
		
		penPosition += glyphWidth;
		penPosition += c; // todo: don't add c for final character? seems right... maybe

		if (glyphIndex < (int) text.size() - 1)
		{
			uint8_t nextChar = text[glyphIndex + 1];
			float kerning = GetKerning(currentChar, nextChar);
			float scaledKerning = kerning * cellHeight;
			penPosition += scaledKerning;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Font::AddVertsForAlignedText2D(VertexBuffer& out_verts, Vec2 const& pivot, Vec2 const& alignment,
	float cellHeight, std::string const& text, Rgba8 const& tint, float lineSpacing /*= 0.5f*/) const
{
	Strings lines = StringUtils::SplitStringOnDelimiter(text, '\n');
	if (lines.empty())
	{
		return;
	}

	if (lines.back().empty())
	{
		// Trims the last trailing new line
		lines.pop_back();
	}

	float paragraphHeight = cellHeight * (float) lines.size() + (lineSpacing * cellHeight) * ((float) lines.size() - 1);
	float paragraphWidth = 0.f;

	for (int i = 0; i < (int) lines.size(); ++i)
	{
		std::string const& line = lines[i];
		float lineWidth = GetTextWidth(cellHeight, line);
		paragraphWidth = MathUtils::Max(paragraphWidth, lineWidth);
	}

	Vec2 topLeft;
	topLeft.x = pivot.x - (paragraphWidth * 0.5f);
	topLeft.x += alignment.x * paragraphWidth * 0.5f;
	topLeft.y = pivot.y + (paragraphHeight * 0.5f);
	topLeft.y += alignment.y * paragraphHeight * 0.5f;

	float lineSpacingHeight = lineSpacing * cellHeight;

	for (int i = 0; i < (int) lines.size(); ++i)
	{
		std::string const& line = lines[i];
		float yOffset = -1.f * ((float)(i + 1) * cellHeight + ((float)(i * lineSpacingHeight)));
		Vec2 textMins = topLeft + Vec2(0.f, yOffset);
		AddVertsForText2D(out_verts, textMins, cellHeight, line, tint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
//void Font::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& textMins, Vec2 const& textMaxs,
//	float cellHeight, std::string const& text, Rgba8 const& tint, Vec2 const& alignment)
//{
//	
//}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetTextWidth(float cellHeight, std::string const& text) const
{
	float penPosition = 0.f;
	for (int glyphIndex = 0; glyphIndex < (int) text.size(); ++glyphIndex)
	{
		uint8_t const& currentChar = text[glyphIndex];
		GlyphData const& charData = m_glyphData[currentChar];

		float a = charData.a * cellHeight;
		float glyphWidth = cellHeight * charData.m_width;
		float c = charData.c * cellHeight;

		penPosition += a;
		penPosition += glyphWidth;
		penPosition += c; // todo: don't add c for final character? seems right... maybe

		if (glyphIndex < (int) text.size() - 1)
		{

			uint8_t nextChar = text[glyphIndex + 1];
			float kerning = GetKerning(currentChar, nextChar);
			float scaledKerning = kerning * cellHeight;
			penPosition += scaledKerning;
		}
	}
	return penPosition;
}



//----------------------------------------------------------------------------------------------------------------------
GlyphData const& Font::GetGlyphData(uint8_t glyph) const
{
	return m_glyphData[glyph];
}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetKerning(uint8_t lhs, uint8_t rhs) const
{
	// Kernings are stored in a vector of pair of glyph id and kerning value
	for (auto& kerningPair : m_kerningData[(size_t) lhs])
	{
		if (kerningPair.glyph == rhs)
		{
			return kerningPair.value;
		}
	}
	return 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetOffsetXOfCharIndex(std::string const& line, int index, float cellHeight, float aspectMultiplier) const
{
	float cursorOffsetX = 0.f;
	for (int i = 0; i < index && i < (int) line.size(); ++i)
	{
		char c = line[i];
		cursorOffsetX += m_glyphData[(uint8_t)c].m_xAdvance * cellHeight * aspectMultiplier;
	}
	return cursorOffsetX;
}



//----------------------------------------------------------------------------------------------------------------------
TextureID Font::GetTexture() const
{
	return m_texture;
}



//----------------------------------------------------------------------------------------------------------------------
ShaderID Font::GetShader() const
{
	return m_shader;
}